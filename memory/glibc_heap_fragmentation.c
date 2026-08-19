#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>

#define BYTES_PER_KIB                   (1024ULL)
#define BYTES_PER_MIB                   (BYTES_PER_KIB * 1024)

#define DEFAULT_TOTAL_MIB               (4096ULL)
#define DEFAULT_HOLD_SECONDS            (10)
#define SMALL_REQUEST_BYTES             ((size_t)256)
#define PROBE_REQUEST_BYTES             ((size_t)4096)
#define PROTECTED_TAIL_CHUNKS           ((size_t)500)
#define PROBE_SHARE_OF_FREED            (4)
#define TOUCH_BYTE                      (1)

#define GLIBC_ALIGNMENT                 (2 * sizeof(size_t))
#define GLIBC_CHUNK_HEADER              (sizeof(size_t))
#define HEAP_BYTES_FOR(request)         (((request) + GLIBC_CHUNK_HEADER + GLIBC_ALIGNMENT - 1) \
                                         & ~(GLIBC_ALIGNMENT - 1))

#define RANDOM_SEED                     (0x243f6a8885a308d3ULL)
#define SPLITMIX64_INCREMENT            (0x9e3779b97f4a7c15ULL)
#define SPLITMIX64_FIRST_MULTIPLIER     (0xbf58476d1ce4e5b9ULL)
#define SPLITMIX64_SECOND_MULTIPLIER    (0x94d049bb133111ebULL)
#define SPLITMIX64_FIRST_SHIFT          (30)
#define SPLITMIX64_SECOND_SHIFT         (27)
#define SPLITMIX64_FINAL_SHIFT          (31)

#define PROC_SELF_STATUS_PATH           "/proc/self/status"
#define PROC_FIELD_RESIDENT             "VmRSS:"
#define PROC_FIELD_RESIDENT_LENGTH      (sizeof(PROC_FIELD_RESIDENT) - 1)
#define PROC_LINE_SIZE                  (256)

struct chunk_node {
    struct chunk_node *next;
};

static uint64_t random_state = RANDOM_SEED;
static unsigned int hold_seconds = DEFAULT_HOLD_SECONDS;

static int random_coin_flip(void)
{
    uint64_t mixed = (random_state += SPLITMIX64_INCREMENT);

    mixed = (mixed ^ (mixed >> SPLITMIX64_FIRST_SHIFT)) * SPLITMIX64_FIRST_MULTIPLIER;
    mixed = (mixed ^ (mixed >> SPLITMIX64_SECOND_SHIFT)) * SPLITMIX64_SECOND_MULTIPLIER;
    return (int)((mixed ^ (mixed >> SPLITMIX64_FINAL_SHIFT)) & 1);
}

static unsigned long long resident_bytes(void)
{
    FILE *status_file = fopen(PROC_SELF_STATUS_PATH, "r");

    if (!status_file) {
        return 0;
    }

    char line[PROC_LINE_SIZE];
    unsigned long long value_kb = 0;

    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, PROC_FIELD_RESIDENT, PROC_FIELD_RESIDENT_LENGTH) == 0) {
            if (sscanf(line + PROC_FIELD_RESIDENT_LENGTH, " %llu", &value_kb) != 1) {
                value_kb = 0;
            }
            break;
        }
    }

    fclose(status_file);
    return value_kb * BYTES_PER_KIB;
}

static void report_and_hold(const char *tag)
{
    printf("%-14s rss %llu MiB\n", tag, resident_bytes() / BYTES_PER_MIB);
    fflush(stdout);
    sleep(hold_seconds);
}

static struct chunk_node *push_chunk(struct chunk_node **head, size_t request_bytes)
{
    struct chunk_node *node = malloc(request_bytes);

    if (!node) {
        return NULL;
    }

    memset(node, TOUCH_BYTE, request_bytes);
    node->next = *head;
    *head = node;
    return node;
}

static void free_chunk_list(struct chunk_node *head)
{
    while (head) {
        struct chunk_node *next = head->next;

        free(head);
        head = next;
    }
}

static int usage(const char *program_name)
{
    fprintf(stderr, "usage: %s [total_mib] [hold_seconds]\n", program_name);
    return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
    unsigned long long total_mib = DEFAULT_TOTAL_MIB;
    char *suffix;

    if (argc > 3) {
        return usage(argv[0]);
    }

    if (argc >= 2) {
        total_mib = strtoull(argv[1], &suffix, 10);
        if (*suffix != '\0' || total_mib == 0) {
            return usage(argv[0]);
        }
    }

    if (argc == 3) {
        unsigned long long parsed_seconds = strtoull(argv[2], &suffix, 10);

        if (*suffix != '\0' || parsed_seconds > UINT_MAX) {
            return usage(argv[0]);
        }
        hold_seconds = (unsigned int)parsed_seconds;
    }

    size_t chunk_count = (size_t)(total_mib * BYTES_PER_MIB / SMALL_REQUEST_BYTES);

    if (chunk_count <= PROTECTED_TAIL_CHUNKS) {
        fprintf(stderr, "total_mib too small: %zu chunks\n", chunk_count);
        return EXIT_FAILURE;
    }

    unsigned long long heap_needed = (unsigned long long)chunk_count
                                     * HEAP_BYTES_FOR(SMALL_REQUEST_BYTES);

    printf("%zu chunks of %zu bytes, %llu MiB of heap, holding %u s per phase\n",
           chunk_count, SMALL_REQUEST_BYTES, heap_needed / BYTES_PER_MIB, hold_seconds);
    report_and_hold("start");

    struct chunk_node *live_head = NULL;

    for (size_t index = 0; index < chunk_count; index++) {
        if (!push_chunk(&live_head, SMALL_REQUEST_BYTES)) {
            fprintf(stderr, "allocation failed at chunk %zu\n", index);
            free_chunk_list(live_head);
            return EXIT_FAILURE;
        }
    }

    report_and_hold("allocated");

    struct chunk_node *previous = NULL;
    struct chunk_node *current = live_head;
    size_t position = 0;
    size_t freed_count = 0;

    while (current) {
        struct chunk_node *next = current->next;

        if (position >= PROTECTED_TAIL_CHUNKS && random_coin_flip()) {
            if (previous) {
                previous->next = next;
            } else {
                live_head = next;
            }
            free(current);
            freed_count++;
        } else {
            previous = current;
        }

        current = next;
        position++;
    }

    unsigned long long freed_bytes = (unsigned long long)freed_count
                                     * HEAP_BYTES_FOR(SMALL_REQUEST_BYTES);

    printf("freed %zu of %zu chunks, %llu MiB now free inside the heap\n",
           freed_count, chunk_count, freed_bytes / BYTES_PER_MIB);
    report_and_hold("after free");

    size_t probe_count = (size_t)(freed_bytes / PROBE_SHARE_OF_FREED
                                  / HEAP_BYTES_FOR(PROBE_REQUEST_BYTES));
    struct chunk_node *probe_head = NULL;
    size_t probe_done = 0;
    unsigned long long rss_before_probe = resident_bytes();

    while (probe_done < probe_count && push_chunk(&probe_head, PROBE_REQUEST_BYTES)) {
        probe_done++;
    }

    unsigned long long probe_bytes = (unsigned long long)probe_done
                                     * HEAP_BYTES_FOR(PROBE_REQUEST_BYTES);
    unsigned long long rss_after_probe = resident_bytes();
    unsigned long long rss_growth = rss_after_probe > rss_before_probe
                                    ? rss_after_probe - rss_before_probe : 0;
    unsigned long long reused = probe_bytes > rss_growth ? probe_bytes - rss_growth : 0;

    printf("probe asked for %llu MiB of %zu byte chunks\n",
           probe_bytes / BYTES_PER_MIB, PROBE_REQUEST_BYTES);
    report_and_hold("after probe");
    printf("verdict: %llu MiB reused from the free heap, %llu MiB taken from the OS\n",
           reused / BYTES_PER_MIB, rss_growth / BYTES_PER_MIB);

    free_chunk_list(probe_head);
    free_chunk_list(live_head);
    report_and_hold("after cleanup");
    return EXIT_SUCCESS;
}