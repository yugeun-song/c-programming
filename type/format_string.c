#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define UNSIGNED_MAX(type) ((type) - 1)
#define SIGNED_MIN(type, utype) ((type)((utype)1 << (sizeof(type) * CHAR_BIT - 1)))
#define SIGNED_MAX(type, utype) ((type) ~((utype)1 << (sizeof(type) * CHAR_BIT - 1)))

int main(void)
{
    char char_min = (char)(((char)-1 < 0) ? SIGNED_MIN(char, unsigned char) : 0);
    char char_max =
        (char)(((char)-1 < 0) ? SIGNED_MAX(char, unsigned char) : UNSIGNED_MAX(unsigned char));
    unsigned char uchar_max = UNSIGNED_MAX(unsigned char);

    short short_min = SIGNED_MIN(short, unsigned short);
    short short_max = SIGNED_MAX(short, unsigned short);
    unsigned short ushort_max = UNSIGNED_MAX(unsigned short);

    int int_min = SIGNED_MIN(int, unsigned int);
    int int_max = SIGNED_MAX(int, unsigned int);
    unsigned int uint_max = UNSIGNED_MAX(unsigned int);

    long long_min = SIGNED_MIN(long, unsigned long);
    long long_max = SIGNED_MAX(long, unsigned long);
    unsigned long ulong_max = UNSIGNED_MAX(unsigned long);

    long long llong_min = SIGNED_MIN(long long, unsigned long long);
    long long llong_max = SIGNED_MAX(long long, unsigned long long);
    unsigned long long ullong_max = UNSIGNED_MAX(unsigned long long);

    float f = 3.141592F;
    double d = 2.718282;
    long double ld = 1.618034L;

    const char *str = "Hello, Architecture!";
    size_t pointer_size = sizeof(void *);

#if defined(_WIN32) || defined(_WIN64)
# define TARGET_OS_NAME "Windows"
#elif defined(__linux__)
# define TARGET_OS_NAME "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
# define TARGET_OS_NAME "macOS"
#elif defined(__FreeBSD__)
# define TARGET_OS_NAME "FreeBSD"
#elif defined(__NetBSD__)
# define TARGET_OS_NAME "NetBSD"
#elif defined(__OpenBSD__)
# define TARGET_OS_NAME "OpenBSD"
#elif defined(__DragonFly__)
# define TARGET_OS_NAME "DragonFly BSD"
#else
# define TARGET_OS_NAME "unknown"
#endif
    printf("Current OS           : %s\n", TARGET_OS_NAME);

#if defined(__x86_64__) || defined(_M_X64)
    printf("Current Architecture : x64 (64-bit)\n\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("Current Architecture : ARM64 (64-bit)\n\n");
#elif defined(__riscv) && (__riscv_xlen == 64)
    printf("Current Architecture : RISC-V RV64 (64-bit)\n\n");
#else
    printf("Current Architecture : unknown\n\n");
#endif

    printf("%-20s (%%d,   %2zu bytes)   MIN: %-22d   MAX: %-20d\n"
           "%-20s (%%hhu, %2zu bytes)   MIN: %-22hhu   MAX: %-20hhu\n\n",
           "char", sizeof(char_min), (int)char_min, (int)char_max, "unsigned char",
           sizeof(uchar_max), (unsigned char)0, uchar_max);

    printf("%-20s (%%hd,  %2zu bytes)   MIN: %-22hd   MAX: %-20hd\n"
           "%-20s (%%hu,  %2zu bytes)   MIN: %-22hu   MAX: %-20hu\n\n",
           "short", sizeof(short_min), (short)short_min, (short)short_max, "unsigned short",
           sizeof(ushort_max), (unsigned short)0, ushort_max);

    printf("%-20s (%%d,   %2zu bytes)   MIN: %-22d   MAX: %-20d\n"
           "%-20s (%%u,   %2zu bytes)   MIN: %-22u   MAX: %-20u\n\n",
           "int", sizeof(int_min), int_min, int_max, "unsigned int", sizeof(uint_max), 0U,
           uint_max);

    printf("%-20s (%%ld,  %2zu bytes)   MIN: %-22ld   MAX: %-20ld\n"
           "%-20s (%%lu,  %2zu bytes)   MIN: %-22lu   MAX: %-20lu\n\n",
           "long", sizeof(long_min), long_min, long_max, "unsigned long", sizeof(ulong_max), 0UL,
           ulong_max);

    printf("%-20s (%%lld, %2zu bytes)   MIN: %-22lld   MAX: %-20lld\n"
           "%-20s (%%llu, %2zu bytes)   MIN: %-22llu   MAX: %-20llu\n\n",
           "long long", sizeof(llong_min), llong_min, llong_max, "unsigned long long",
           sizeof(ullong_max), 0ULL, ullong_max);

    printf("%-20s (%%f,   %2zu bytes)   VAL: %-20f\n"
           "%-20s (%%lf,  %2zu bytes)   VAL: %-20lf\n",
           "float", sizeof(f), f, "double", sizeof(d), d);

#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__USE_MINGW_ANSI_STDIO))
    printf("%-20s (%%f,   %2zu bytes)   VAL: %-20f\n\n", "long double", sizeof(ld), (double)ld);
#else
    printf("%-20s (%%Lf,  %2zu bytes)   VAL: %-20Lf\n\n", "long double", sizeof(ld), ld);
#endif

    printf("%-20s (%%s,   %2zu bytes)   VAL: %s\n"
           "%-20s (%%p,   %2zu bytes)   ADR: %p\n",
           "char*", sizeof(str), str, "void*", sizeof(void *), (void *)&char_min);

#if SIZE_MAX == ULONG_MAX
    printf("%-20s (%%lu,  %2zu bytes)   VAL: %lu\n", "size_t", sizeof(pointer_size),
           (unsigned long)pointer_size);
#elif SIZE_MAX == ULLONG_MAX
    printf("%-20s (%%llu, %2zu bytes)   VAL: %llu\n", "size_t", sizeof(pointer_size),
           (unsigned long long)pointer_size);
#else
    printf("%-20s (%%zu,  %2zu bytes)   VAL: %zu\n", "size_t", sizeof(pointer_size), pointer_size);
#endif

    return 0;
}
