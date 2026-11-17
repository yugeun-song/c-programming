#define _DEFAULT_SOURCE

#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>

#define SMALL 256
#define PROBE 4096
#define KEEP 500
#define CHUNK(n) (((n) + sizeof(size_t) + 15) & ~(size_t)15)

struct node {
    struct node *next;
};

static unsigned int gs_hold_seconds = 10;

static int flip_coin(void)
{
    static uint64_t s_state = 0x243F6A8885A308D3ULL;

    s_state += 0x9E3779B97F4A7C15ULL;

    uint64_t z = s_state;

    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return (int)((z ^ (z >> 31)) & 1);
}

static size_t read_rss(void)
{
    char buf[4096] = { 0 };
    const char *line = NULL;

    int fd = open("/proc/self/smaps_rollup", O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf) - 1) > 0) {
            line = strstr(buf, "\nRss:");
        }
        close(fd);
    }

    return (line != NULL) ? (size_t)strtoull(line + 5, NULL, 10) * 1024 : 0;
}

static void report(const char *const tag)
{
    printf("%-14s rss %zu bytes\n", tag, read_rss());
    fflush(stdout);
    sleep(gs_hold_seconds);
}

static struct node *push_node(struct node **const mut_head, size_t size)
{
    struct node *const node = malloc(size);
    if (node != NULL) {
        memset(node, 1, size);
        node->next = *mut_head;
        *mut_head = node;
    }

    return node;
}

static void free_nodes(struct node *const mut_head_nullable)
{
    struct node *next = mut_head_nullable;

    while (next != NULL) {
        struct node *node = next;

        next = node->next;
        free(node);
        node = NULL;
    }
}

int main(int argc, char *argv[])
{
    unsigned long long mib = 4096;
    unsigned long long secs = gs_hold_seconds;
    char *end = NULL;
    int is_invalid = (argc > 3);

    if (argc > 1) {
        mib = strtoull(argv[1], &end, 10);
        if (mib == 0 || *end != '\0') {
            is_invalid = 1;
        }
    }
    if (argc > 2) {
        secs = strtoull(argv[2], &end, 10);
        if (secs > UINT_MAX || *end != '\0') {
            is_invalid = 1;
        }
    }
    if (is_invalid != 0) {
        fprintf(stderr, "usage: %s [total_mib] [hold_seconds]\n", argv[0]);
        return EXIT_FAILURE;
    }
    gs_hold_seconds = (unsigned int)secs;

    size_t count = (size_t)(mib * 1024 * 1024 / SMALL);

    if (count <= KEEP) {
        fprintf(stderr, "total_mib too small: %zu chunks\n", count);
        return EXIT_FAILURE;
    }
    prctl(PR_SET_THP_DISABLE, 1, 0, 0, 0);
    printf("%zu chunks of %d bytes, %zu bytes of heap, holding %u s per phase\n", count, SMALL,
           count * CHUNK(SMALL), gs_hold_seconds);
    report("start");

    struct node *live = NULL;

    for (size_t i = 0; i < count; ++i) {
        if (push_node(&live, SMALL) == NULL) {
            fprintf(stderr, "allocation failed at chunk %zu\n", i);
            free_nodes(live);
            live = NULL;
            return EXIT_FAILURE;
        }
    }
    report("allocated");

    size_t freed = 0;
    size_t pos = 0;

    for (struct node **link = &live; *link != NULL; ++pos) {
        struct node *node = *link;

        if (pos >= KEEP && flip_coin() != 0) {
            *link = node->next;
            free(node);
            node = NULL;
            ++freed;
        } else {
            link = &node->next;
        }
    }

    size_t holes = freed * CHUNK(SMALL);

    printf("freed %zu of %zu chunks, %zu bytes now free inside the heap\n", freed, count, holes);
    report("after free");

    size_t want = holes / 4 / CHUNK(PROBE);
    size_t done = 0;
    struct node *probe = NULL;
    size_t before = read_rss();

    while (done < want && push_node(&probe, PROBE) != NULL) {
        ++done;
    }

    size_t after = read_rss();
    size_t asked = done * CHUNK(PROBE);
    size_t grown = (after > before) ? after - before : 0;

    printf("probe asked for %zu bytes of %d byte chunks\n", asked, PROBE);
    report("after probe");
    printf("verdict: %zu bytes reused from the free heap, %zu bytes taken from the OS\n",
           (asked > grown) ? asked - grown : 0, grown);
    free_nodes(probe);
    probe = NULL;
    free_nodes(live);
    live = NULL;
    report("after cleanup");
    return 0;
}
