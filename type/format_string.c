#include <stdio.h>
#include <stddef.h>

#define UNSIGNED_MAX(type) ((type)-1)
#define SIGNED_MIN(type, utype) ((type)((utype)1 << (sizeof(type) * 8 - 1)))
#define SIGNED_MAX(type, utype) ((type)~((utype)1 << (sizeof(type) * 8 - 1)))

int main(void)
{
    char c_min = (char)((char)-1 < 0 ? SIGNED_MIN(char, unsigned char) : 0);
    char c_max = (char)((char)-1 < 0 ? SIGNED_MAX(char, unsigned char) : UNSIGNED_MAX(unsigned char));
    unsigned char uc_max = UNSIGNED_MAX(unsigned char);

    short s_min = SIGNED_MIN(short, unsigned short);
    short s_max = SIGNED_MAX(short, unsigned short);
    unsigned short us_max = UNSIGNED_MAX(unsigned short);

    int i_min = SIGNED_MIN(int, unsigned int);
    int i_max = SIGNED_MAX(int, unsigned int);
    unsigned int ui_max = UNSIGNED_MAX(unsigned int);

    long l_min = SIGNED_MIN(long, unsigned long);
    long l_max = SIGNED_MAX(long, unsigned long);
    unsigned long ul_max = UNSIGNED_MAX(unsigned long);

    long long ll_min = SIGNED_MIN(long long, unsigned long long);
    long long ll_max = SIGNED_MAX(long long, unsigned long long);
    unsigned long long ull_max = UNSIGNED_MAX(unsigned long long);

    float f = 3.141592f;
    double d = 2.718282;
    long double ld = 1.618034L;

    char* str = "Hello, Architecture!";
    size_t sz = sizeof(void*);

#if defined(_WIN32) || defined(_WIN64)
    printf("Current OS           : Windows\n");
#elif defined(__linux__)
    printf("Current OS           : Linux\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("Current OS           : macOS\n");
#elif defined(__FreeBSD__)
    printf("Current OS           : FreeBSD\n");
#elif defined(__NetBSD__)
    printf("Current OS           : NetBSD\n");
#elif defined(__OpenBSD__)
    printf("Current OS           : OpenBSD\n");
#elif defined(__DragonFly__)
    printf("Current OS           : DragonFly BSD\n");
#else
#error "Unsupported OS! This program only supports Windows, Linux, macOS, and Major BSDs."
#endif

#if defined(__x86_64__) || defined(_M_X64)
    printf("Current Architecture : x64 (64-bit)\n\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("Current Architecture : ARM64 (64-bit)\n\n");
#elif defined(__i386__) || defined(_M_IX86)
    printf("Current Architecture : x86 (32-bit)\n\n");
#elif defined(__arm__) || defined(_M_ARM)
    printf("Current Architecture : ARM (32-bit)\n\n");
#else
#error "Unsupported architecture! This program only supports x86, x86_64(amd64), 32bit ARM, and ARM64."
#endif

    printf("%-20s (%%d,   %2zu bytes)   MIN: %-22d   MAX: %-20d\n"
           "%-20s (%%hhu, %2zu bytes)   MIN: %-22hhu   MAX: %-20hhu\n\n",
           "char", sizeof(c_min), (int)c_min, (int)c_max,
           "unsigned char", sizeof(uc_max), (unsigned char)0, uc_max);

    printf("%-20s (%%hd,  %2zu bytes)   MIN: %-22hd   MAX: %-20hd\n"
           "%-20s (%%hu,  %2zu bytes)   MIN: %-22hu   MAX: %-20hu\n\n",
           "short", sizeof(s_min), (short)s_min, (short)s_max,
           "unsigned short", sizeof(us_max), (unsigned short)0, us_max);

    printf("%-20s (%%d,   %2zu bytes)   MIN: %-22d   MAX: %-20d\n"
           "%-20s (%%u,   %2zu bytes)   MIN: %-22u   MAX: %-20u\n\n",
           "int", sizeof(i_min), i_min, i_max,
           "unsigned int", sizeof(ui_max), 0U, ui_max);

    printf("%-20s (%%ld,  %2zu bytes)   MIN: %-22ld   MAX: %-20ld\n"
           "%-20s (%%lu,  %2zu bytes)   MIN: %-22lu   MAX: %-20lu\n\n",
           "long", sizeof(l_min), l_min, l_max,
           "unsigned long", sizeof(ul_max), 0UL, ul_max);

    printf("%-20s (%%lld, %2zu bytes)   MIN: %-22lld   MAX: %-20lld\n"
           "%-20s (%%llu, %2zu bytes)   MIN: %-22llu   MAX: %-20llu\n\n",
           "long long", sizeof(ll_min), ll_min, ll_max,
           "unsigned long long", sizeof(ull_max), 0ULL, ull_max);

    printf("%-20s (%%f,   %2zu bytes)   VAL: %-20f\n"
           "%-20s (%%lf,  %2zu bytes)   VAL: %-20lf\n",
           "float", sizeof(f), f,
           "double", sizeof(d), d);

#if defined(_WIN32) || defined(_WIN64)
    printf("%-20s (%%f,   %2zu bytes)   VAL: %-20f\n\n", "long double", sizeof(ld), (double)ld);
#else
    printf("%-20s (%%Lf,  %2zu bytes)   VAL: %-20Lf\n\n", "long double", sizeof(ld), ld);
#endif

    printf("%-20s (%%s,   %2zu bytes)   VAL: %s\n"
           "%-20s (%%p,   %2zu bytes)   ADR: %p\n",
           "char*", sizeof(str), str,
           "void*", sizeof(void*), (void*)&c_min);

#if defined(_WIN64)
    printf("%-20s (%%llu, %2zu bytes)   VAL: %llu\n", "size_t", sizeof(sz), (unsigned long long)sz);
#elif defined(__x86_64__) || defined(__aarch64__)
    printf("%-20s (%%lu,  %2zu bytes)   VAL: %lu\n", "size_t", sizeof(sz), (unsigned long)sz);
#else
    printf("%-20s (%%u,   %2zu bytes)   VAL: %u\n", "size_t", sizeof(sz), (unsigned int)sz);
#endif

    return 0;
}