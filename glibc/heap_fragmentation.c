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

static unsigned int hold = 10;

static int coin(void)
{
    static uint64_t state = 0x243f6a8885a308d3ULL;
    uint64_t z = (state += 0x9e3779b97f4a7c15ULL);

    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return (int)((z ^ (z >> 31)) & 1);
}

static size_t rss(void)
{
    char buf[4096] = { 0 };
    char *line = NULL;
    int fd = open("/proc/self/smaps_rollup", O_RDONLY);

    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf) - 1) > 0) {
            line = strstr(buf, "\nRss:");
        }
        close(fd);
    }
    return line ? (size_t)strtoull(line + 5, NULL, 10) * 1024 : 0;
}

static void report(const char *tag)
{
    printf("%-14s rss %zu bytes\n", tag, rss());
    fflush(stdout);
    sleep(hold);
}

static struct node *push(struct node **head, size_t size)
{
    struct node *n = malloc(size);

    if (n) {
        memset(n, 1, size);
        n->next = *head;
        *head = n;
    }
    return n;
}

static void drop(struct node *n)
{
    while (n) {
        struct node *next = n->next;

        free(n);
        n = next;
    }
}

int main(int argc, char *argv[])
{
    unsigned long long mib = 4096;
    unsigned long long secs = hold;
    char *end;
    int bad = argc > 3;

    if (argc > 1) {
        mib = strtoull(argv[1], &end, 10);
        bad |= mib == 0 || *end != '\0';
    }
    if (argc > 2) {
        secs = strtoull(argv[2], &end, 10);
        bad |= secs > UINT_MAX || *end != '\0';
    }
    if (bad) {
        fprintf(stderr, "usage: %s [total_mib] [hold_seconds]\n", argv[0]);
        return EXIT_FAILURE;
    }
    hold = (unsigned int)secs;

    size_t count = (size_t)(mib * 1024 * 1024 / SMALL);

    if (count <= KEEP) {
        fprintf(stderr, "total_mib too small: %zu chunks\n", count);
        return EXIT_FAILURE;
    }
    prctl(PR_SET_THP_DISABLE, 1, 0, 0, 0);
    printf("%zu chunks of %d bytes, %zu bytes of heap, holding %u s per phase\n", count, SMALL,
           count * CHUNK(SMALL), hold);
    report("start");

    struct node *live = NULL;

    for (size_t i = 0; i < count; i++) {
        if (!push(&live, SMALL)) {
            fprintf(stderr, "allocation failed at chunk %zu\n", i);
            drop(live);
            return EXIT_FAILURE;
        }
    }
    report("allocated");

    size_t freed = 0;
    size_t pos = 0;

    for (struct node **link = &live; *link; pos++) {
        struct node *n = *link;

        if (pos >= KEEP && coin()) {
            *link = n->next;
            free(n);
            freed++;
        } else {
            link = &n->next;
        }
    }

    size_t holes = freed * CHUNK(SMALL);

    printf("freed %zu of %zu chunks, %zu bytes now free inside the heap\n", freed, count, holes);
    report("after free");

    size_t want = holes / 4 / CHUNK(PROBE);
    size_t done = 0;
    struct node *probe = NULL;
    size_t before = rss();

    while (done < want && push(&probe, PROBE)) {
        done++;
    }

    size_t after = rss();
    size_t asked = done * CHUNK(PROBE);
    size_t grown = after > before ? after - before : 0;

    printf("probe asked for %zu bytes of %d byte chunks\n", asked, PROBE);
    report("after probe");
    printf("verdict: %zu bytes reused from the free heap, %zu bytes taken from the OS\n",
           asked > grown ? asked - grown : 0, grown);
    drop(probe);
    drop(live);
    report("after cleanup");
    return EXIT_SUCCESS;
}