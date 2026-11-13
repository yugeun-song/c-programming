# C Coding Style

Contents

- **1. Scope**
  - 1.1. Applicability
  - 1.2. Coding style differences between project and dependencies
- **2. Principles**
  - 2.1. Simple, intuitive, conventional code
  - 2.2. Avoiding name collisions
- **3. Language standard and platform**
  - 3.1. Language standard
  - 3.2. Platform APIs
  - 3.3. Marking extensions
  - 3.4. Implementation-defined behavior
  - 3.5. Choosing standard functions
  - 3.6. Standard functions not to use
- **4. Files**
  - 4.1. File names
  - 4.2. Encoding and line endings
  - 4.3. Character set
  - 4.4. Final newline
  - 4.5. Whitespace characters
  - 4.6. Order within a file
  - 4.7. Feature test macros
  - 4.8. Include order
  - 4.9. Include guards
  - 4.10. Header contents
- **5. Formatting**
  - 5.1. Indentation
  - 5.2. Line length
  - 5.3. Breaking long expressions
  - 5.4. User-visible strings
  - 5.5. Function headers
  - 5.6. Function braces
  - 5.7. Control statement and loop braces
  - 5.8. Mandatory braces
  - 5.9. struct, union, and enum braces
  - 5.10. Scope-narrowing blocks
  - 5.11. Brace initialization
  - 5.12. switch and case
  - 5.13. goto labels
  - 5.14. Spaces
  - 5.15. Pointer notation
  - 5.16. One statement per line where possible
  - 5.17. Blank lines
  - 5.18. Alignment
  - 5.19. Preprocessing directives
  - 5.20. GCC/Clang inline assembly
- **6. Naming**
  - 6.1. Function and variable names
  - 6.2. Macro and enumeration constant names
  - 6.3. Function-like macros
  - 6.4. Names of const objects
  - 6.5. Name length and content
  - 6.6. First word of function names
  - 6.7. Names for truth values
  - 6.8. Global and static variables
  - 6.9. typedef
  - 6.10. Reserved identifiers
  - 6.11. Win32 API symbols and types
- **7. Declarations and types**
  - 7.1. One declaration per line
  - 7.2. Declaration placement
  - 7.3. Scope and shadowing
  - 7.4. Pointers to objects whose lifetime has ended
  - 7.5. Integer types
  - 7.6. Truth values
  - 7.7. Constants
  - 7.8. const
  - 7.9. register and volatile
  - 7.10. VLA
  - 7.11. Initialization
- **8. Expressions**
  - 8.1. Literal notation
  - 8.2. Increment and decrement operators
  - 8.3. sizeof
  - 8.4. Casts
  - 8.5. Parentheses for precedence
  - 8.6. Comparisons
  - 8.7. Null pointers and truth tests
  - 8.8. Conditional operator
  - 8.9. Assignment
  - 8.10. Comma operator
- **9. Statements**
  - 9.1. switch
  - 9.2. Empty loop bodies
  - 9.3. goto
- **10. Functions**
  - 10.1. Parameter lists
  - 10.2. Parameter order
  - 10.3. Pointer parameters
  - 10.4. Return value conventions
  - 10.5. Function size
  - 10.6. inline
  - 10.7. Function placement
  - 10.8. main
- **11. Preprocessor and macros**
  - 11.1. Conditional compilation
  - 11.2. Alternatives to macros
  - 11.3. Parentheses
  - 11.4. Multi-statement macros
  - 11.5. Macros to avoid
  - 11.6. Local names in macros
- **12. Comments**
  - 12.1. Amount and content of comments
  - 12.2. Comment format
  - 12.3. Function comments and documentation comments
- **13. Error handling and resources**
  - 13.1. Checking return values
  - 13.2. Unused diagnostics
  - 13.3. errno
  - 13.4. Resource cleanup
  - 13.5. Dynamic memory
  - 13.6. assert
  - 13.7. Static assertions
  - 13.8. Termination
- **14. Tools**
  - 14.1. clang-format
  - 14.2. clang-tidy
  - 14.3. GCC/Clang compiler warnings
  - 14.4. Editor settings
- **15. References**

## 1. Scope

### 1.1. Applicability

This coding style is the convention for code on the Linux platform and for cross-platform code whose targets include Linux. It covers C sources (`.c`, `.h`) and the inline assembly in them.

Your own Linux kernel modules and drivers may use this coding style, independently of the original kernel source. However, when customizing the Linux kernel source itself, follow the kernel coding style as it is. This keeps different styles from mixing within the kernel source.

A project that targets only a specific OS or architecture, such as a Windows-only, FreeBSD-only, or macOS-only project, follows the coding style of that OS or architecture. In a cross-platform project, by contrast, apply this coding style to OS-specific code as well. Trees that each have their own structure and binaries for one platform are not a cross-platform project, and each such tree follows the style of its own platform.

### 1.2. Coding style differences between project and dependencies

Bring all code written by the project itself, including existing code, into line with these rules. In contrast, dependency code imported from outside keeps its original style.

Draw a clear boundary between dependency code and project code. Keep dependency code in a directory separate from project code where possible. If it must share a directory, draw the boundary at the file level. In either case, do not apply the project's coding style to dependency code files. When dependency code needs a change, make it in the file's original style. This way, even when two styles coexist in a project, each file holds only one style. When different styles mix in one file, readers are unsure which rules to follow, and when the dependency code is updated to a new version, the changed parts are hard to pick out.

Incorrect (a project function added to the dependency code `third_party/tinyini/ini.c`):

```c
static int
ini_skip_space (const char *s)
{
  int n = 0;
  while (s[n] == ' ' || s[n] == '\t')
    n++;
  return n;
}

static size_t count_sections(const char *const text)
{
    size_t count = 0;

    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '[') {
            ++count;
        }
    }

    return count;
}
```

Correct (dependency code kept in a separate directory):

```text
src/
    main.c
    settings.c
    settings.h
third_party/
    tinyini/
        ini.c
        ini.h
```

Correct (the dependency code `ini.c` and `ini.h` kept in the same directory):

```text
src/
    ini.c
    ini.h
    main.c
    settings.c
    settings.h
```

Correct (`src/settings.c`):

```c
#include <stddef.h>

static size_t count_sections(const char *const text)
{
    size_t count = 0;

    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '[') {
            ++count;
        }
    }

    return count;
}
```

## 2. Principles

This coding style is built on the Linux kernel coding style and on some ideas from the MISRA C rules. The shape of braces, names, declarations, and comments follows the kernel, but indentation uses four spaces instead of tabs. From MISRA C, it adopts rules that leave less room for mistakes, such as mandatory braces, restrictions on the comma operator and on assignment in conditions, and explicit comparison of integers and pointers.

### 2.1. Simple, intuitive, conventional code

Keep code short, but do not write it to show off. Avoid anything open to more than one reading as far as possible, and build the implementation from very simple, clear lines, one at a time. Write each line so that it maps to assembly instructions as close to 1:1 as possible.

Do not write code that is deliberately complicated, made to look difficult, or makes the reader think twice. Do not write code that leaves much room for thought or mistakes, either.

Write code in widely used forms. Do not use a technique that is not common practice, even if it reduces mistakes. For example, a comparison with the constant on the left (`5 == value`) turns `==` mistyped as `=` into a compile error. However, it is not a widely used form, so it clashes with what readers are used to, and the burden of that clash outweighs the gain. Techniques like this, whose side effects can outweigh their actual benefit, are not part of this coding style.

Incorrect:

```c
static long sum_clamped(const int *v, int n, int lo, int hi, int *clipped)
{
    long sum;

    for (sum = *clipped = 0; n-- > 0; ++v)
        sum += *v < lo ? (++*clipped, lo) : *v > hi ? (++*clipped, hi) : *v;
    return sum;
}
```

This function clips each of the `n` elements that `v` points to so that it lies between `lo` and `hi`, adds them up, and leaves the number of clipped elements in `*clipped`. The code is syntactically valid in every standard from C89 to the current one and has no undefined behavior. Its result also matches the correct example below. However, within a single line, an element read, two comparisons, an increment, and an addition are tangled together through the comma operator and nested conditional operators. The reader has to work out one by one how the conditional operators group, when `n--` and `++v` execute, and in what order `sum = *clipped = 0` assigns. Do not write such code: even when it is syntactically sound, it adds things to think about and wastes time in reading and changing it.

Correct:

```c
static long sum_clamped(const int *const samples, size_t count, int low, int high,
                        int *const mut_clipped)
{
    long sum = 0;
    int clipped = 0;

    for (size_t i = 0; i < count; ++i) {
        int value = samples[i];

        if (value < low) {
            value = low;
            ++clipped;
        } else if (value > high) {
            value = high;
            ++clipped;
        }

        sum += value;
    }

    *mut_clipped = clipped;
    return sum;
}
```

Each line does only one of reading, comparing, assigning, or incrementing, so it maps directly to a few assembly instructions. The changes from the incorrect example are as follows.

- Read elements through the subscript `samples[i]` instead of advancing the pointer parameter. Fix the pointer parameter itself with `*const`.
- Take the element count as `size_t`. Declare the loop variable `i` in the init clause of the `for` statement and change it only in the increment expression. Do not change values in the condition, as `n--` does.
- Unfold the comma operator and the nested conditional operators into `if` and `else if`, so that each line does only one thing.
- Split the chained assignment `sum = *clipped = 0` into a declaration with an initializer for each variable.
- Count the clipped elements in the local variable `clipped`, and write the count to `*mut_clipped` only once, at the end. The name of a pointer parameter through which the target is modified starts with `mut_`.
- Wrap the body of the `for` statement in braces.
- Use descriptive names (`samples`, `low`, `high`) instead of abbreviated names (`v`, `lo`, `hi`).

### 2.2. Avoiding name collisions

Do not use names that are likely to collide with names from widely used standards such as the C standard and POSIX, or with C keywords. Even if a name does not collide today, the build breaks once a new standard or library adds the same name. For example, C23 made `bool`, `true`, `false`, `nullptr`, and `typeof` keywords, and POSIX.1-2024 added `strlcpy` and `strlcat` as standard functions.

Incorrect:

```c
#define nullptr ((void *)0)

static const char *gs_last_error = nullptr;
```

```c
#include <string.h>

static size_t strlcpy(char *const mut_dst, const char *const src, size_t dst_size)
{
    size_t length = strlen(src);
    size_t copy_length = (length < dst_size) ? length : dst_size - 1;

    memcpy(mut_dst, src, copy_length);
    mut_dst[copy_length] = '\0';
    return length;
}
```

Correct:

```c
static const char *gs_last_error = NULL;
```

```c
#include <string.h>

static size_t copy_string(char *const mut_dst, const char *const src, size_t dst_size)
{
    size_t length = strlen(src);
    size_t copy_length = (length < dst_size) ? length : dst_size - 1;

    memcpy(mut_dst, src, copy_length);
    mut_dst[copy_length] = '\0';
    return length;
}
```

## 3. Language standard and platform

### 3.1. Language standard

The default standard is C99. A project may choose another standard or a GNU dialect if needed. State the chosen standard explicitly in the build configuration.

- For GCC and Clang, select it with `-std` (`-std=c99`, `-std=c11`, `-std=c17`, `-std=c23`, `-std=gnu99`).
- MSVC has no C99 mode. Its default mode, without `/std`, is C89 plus Microsoft extensions, and some of those extensions are C99 features. Select C11 or later with `/std:c11`, `/std:c17`, or `/std:clatest`.
- For CMake, select it with `CMAKE_C_STANDARD`. With MSVC 19.27 or later, `CMAKE_C_STANDARD 99` passes no standard option, so the build uses the default mode.

The standard documents and their WG14 public drafts are as follows. The mapping follows the [WG14 projects page](https://www.open-std.org/jtc1/sc22/wg14/www/projects).

- C99: [ISO/IEC 9899:1999](https://www.iso.org/standard/29237.html), draft [N1256](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf)
- C11: [ISO/IEC 9899:2011](https://www.iso.org/standard/57853.html), draft [N1570](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
- C17: [ISO/IEC 9899:2018](https://www.iso.org/standard/74528.html), draft [N2310](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf)
- C23: [ISO/IEC 9899:2024](https://www.iso.org/standard/82075.html), draft [N3220](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3220.pdf)

Incorrect:

```make
CFLAGS += -Wall -Wextra -pedantic -Wshadow
```

Correct (GCC/Clang):

```make
CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wshadow
```

Correct (selecting C11 with MSVC):

```bat
cl /std:c11 /W4 /c main.c
```

Correct (CMake):

```cmake
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
```

### 3.2. Platform APIs

For features whose implementation differs by platform, such as threads, synchronization, and atomic operations, prefer the APIs that the platform and the compiler provide directly. Use POSIX threads (pthread) on Linux and the Win32 API on Windows. For atomic operations, use the `__atomic` builtins with GCC/Clang and the Interlocked functions of the Win32 API on Windows. Avoid standard libraries that add another layer on top of the platform APIs, such as C11 `<threads.h>` and `<stdatomic.h>`.

C is chosen even though languages such as C++ and Rust exist, because it offers transparency that maps almost 1:1 to assembly and the platform, with only minimal abstraction on top. Calling platform APIs directly avoids needless abstraction layers and allows precise, transparent control of behavior.

Discouraged:

```c
#include <threads.h>

static mtx_t gs_hit_lock;
static unsigned long gs_hit_count;

static void record_hit(void)
{
    mtx_lock(&gs_hit_lock);
    ++gs_hit_count;
    mtx_unlock(&gs_hit_lock);
}
```

```c
#include <stdatomic.h>

static atomic_ulong gs_hit_count;

static void record_hit(void)
{
    atomic_fetch_add_explicit(&gs_hit_count, 1, memory_order_relaxed);
}
```

Correct (Linux):

```c
#include <pthread.h>

static pthread_mutex_t gs_hit_lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned long gs_hit_count;

static void record_hit(void)
{
    pthread_mutex_lock(&gs_hit_lock);
    ++gs_hit_count;
    pthread_mutex_unlock(&gs_hit_lock);
}
```

Correct (Windows):

```c
#include <windows.h>

static SRWLOCK gs_hit_lock = SRWLOCK_INIT;
static unsigned long gs_hit_count;

static void record_hit(void)
{
    AcquireSRWLockExclusive(&gs_hit_lock);
    ++gs_hit_count;
    ReleaseSRWLockExclusive(&gs_hit_lock);
}
```

Correct (GCC/Clang atomic builtins):

```c
static unsigned long gs_hit_count;

static void record_hit(void)
{
    __atomic_add_fetch(&gs_hit_count, 1, __ATOMIC_RELAXED);
}
```

Correct (Windows Interlocked functions):

```c
#include <windows.h>

static LONG gs_hit_count;

static void record_hit(void)
{
    InterlockedIncrement(&gs_hit_count);
}
```

### 3.3. Marking extensions

Make the use of features outside the chosen language standard visible.

- Wrap GNU extension syntax with `__extension__`.
- Spell extension keywords with underscores, as in `__asm__` and `__typeof__`. In ISO standard modes such as `-std=c99`, `asm` is not a keyword, and `typeof` is a keyword only from C23. So in those modes the spellings without underscores do not compile.

Incorrect:

```c
#define ABS_DIFF(a, b)                     \
    ({                                     \
        __typeof__(a) a_ = (a);            \
        __typeof__(b) b_ = (b);            \
        (a_ > b_) ? (a_ - b_) : (b_ - a_); \
    })
```

```c
static inline void compiler_barrier(void)
{
    asm volatile("" ::: "memory");
}

#define SWAP_VALUES(a, b)     \
    do {                      \
        typeof(a) tmp_ = (a); \
        (a) = (b);            \
        (b) = tmp_;           \
    } while (0)
```

Correct:

```c
#define ABS_DIFF(a, b)                     \
    __extension__({                        \
        __typeof__(a) a_ = (a);            \
        __typeof__(b) b_ = (b);            \
        (a_ > b_) ? (a_ - b_) : (b_ - a_); \
    })
```

```c
static inline void compiler_barrier(void)
{
    __asm__ volatile("" ::: "memory");
}

#define SWAP_VALUES(a, b)         \
    do {                          \
        __typeof__(a) tmp_ = (a); \
        (a) = (b);                \
        (b) = tmp_;               \
    } while (0)
```

### 3.4. Implementation-defined behavior

Do not depend on implementation-defined behavior, such as integer widths, the signedness of `char`, endianness, alignment, and padding, or on platform-specific internal behavior. If code must depend on it, state so in a comment. However, code with a clearly defined target platform may rely on that platform's behavior without hesitation.

Incorrect:

```c
struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t question_count;
    uint16_t answer_count;
};

static void read_dns_header(struct dns_header *const mut_header, const unsigned char *const packet)
{
    memcpy(mut_header, packet, sizeof(*mut_header));
}
```

DNS header fields are transmitted in big-endian order. On little-endian x86-64 and AArch64 Linux, this code reads every field with its bytes reversed. It also relies on the assumption that the structure has no padding.

Correct:

```c
struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t question_count;
    uint16_t answer_count;
};

static uint16_t decode_be16(const unsigned char *const bytes)
{
    return (uint16_t)((bytes[0] << 8) | bytes[1]);
}

static void read_dns_header(struct dns_header *const mut_header, const unsigned char *const packet)
{
    mut_header->id = decode_be16(packet);
    mut_header->flags = decode_be16(packet + 2);
    mut_header->question_count = decode_be16(packet + 4);
    mut_header->answer_count = decode_be16(packet + 6);
}
```

Correct (the relied-on behavior stated in a comment):

```c
static int32_t scale_down_by_16(int32_t value)
{
    /* relies on >> sign-extending negative values, as GCC documents and Clang also does */
    return value >> 4;
}
```

Correct (little-endian-only firmware that reads a little-endian sensor):

```c
struct sensor_sample {
    uint16_t temperature;
    uint16_t humidity;
};

static void read_sensor_sample(struct sensor_sample *const mut_sample,
                               const unsigned char *const bytes)
{
    memcpy(mut_sample, bytes, sizeof(*mut_sample));
}
```

### 3.5. Choosing standard functions

Use C standard functions, string functions in particular, after weighing the trade-offs in implementation, string and memory safety, and performance. When batch processing, parallel processing, thread safety, or similar properties must be guaranteed, write that functionality yourself.

For example, `strtok` saves internally the position where the next search starts and resumes from it on the next call. C11 states that `strtok` need not avoid data races with other calls to `strtok` (C11 §7.24.5.8 ¶6). Also, `strncat` searches for the end of the destination string from the beginning on every call. So appending many strings one after another takes time proportional to the square of the concatenated length.

Incorrect:

```c
struct field_job {
    char line[256];
    size_t field_count;
};

static void *count_fields(void *const mut_arg)
{
    struct field_job *job = mut_arg;

    for (char *field = strtok(job->line, ","); field != NULL; field = strtok(NULL, ",")) {
        ++job->field_count;
    }

    return NULL;
}
```

Discouraged:

```c
static void join_words(char *const mut_dst, size_t dst_size, const char *const *const words,
                       size_t count)
{
    mut_dst[0] = '\0';

    for (size_t i = 0; i < count; ++i) {
        strncat(mut_dst, words[i], dst_size - strlen(mut_dst) - 1);
    }
}
```

Correct:

```c
struct field_job {
    char line[256];
    size_t field_count;
};

static char *take_field(char **const mut_cursor)
{
    char *field = *mut_cursor;

    if (field == NULL) {
        return NULL;
    }

    char *comma = strchr(field, ',');
    if (comma != NULL) {
        *comma = '\0';
        *mut_cursor = comma + 1;
    } else {
        *mut_cursor = NULL;
    }

    return field;
}

static void *count_fields(void *const mut_arg)
{
    struct field_job *job = mut_arg;
    char *cursor = job->line;

    for (char *field = take_field(&cursor); field != NULL; field = take_field(&cursor)) {
        ++job->field_count;
    }

    return NULL;
}
```

```c
static void join_words(char *const mut_dst, size_t dst_size, const char *const *const words,
                       size_t count)
{
    size_t used = 0;

    for (size_t i = 0; i < count; ++i) {
        size_t length = strlen(words[i]);

        if (length > dst_size - used - 1) {
            length = dst_size - used - 1;
        }

        memcpy(mut_dst + used, words[i], length);
        used += length;
    }

    mut_dst[used] = '\0';
}
```

### 3.6. Standard functions not to use

Do not use `gets`. It takes no buffer size, so it cannot prevent overflow, and it was removed in the C11 standard.

Incorrect:

```c
static int read_command(char *const mut_line)
{
    if (gets(mut_line) == NULL) {
        return -1;
    }

    return 0;
}
```

Correct:

```c
static int read_command(char *const mut_line, size_t line_size)
{
    if (fgets(mut_line, (int)line_size, stdin) == NULL) {
        return -1;
    }

    return 0;
}
```

## 4. Files

### 4.1. File names

C source files end in `.c`, and headers end in `.h`. Write names in lowercase snake_case. Do not use Windows reserved names (`CON`, `PRN`, `AUX`, `NUL`, `COM1`, `LPT1`, and so on), even with an extension.

Incorrect:

```text
text_buffer.C
word_count.hpp
```

```text
TextBuffer.c
wordCount.c
wordCount.h
```

```text
prn.c
```

Correct:

```text
text_buffer.c
word_count.h
```

```text
text_buffer.c
word_count.c
word_count.h
```

```text
printer.c
```

### 4.2. Encoding and line endings

Use UTF-8 without a BOM and LF line endings in files. Use CRLF only in Visual Studio project files (`.sln`, `.slnx`, `.vcxproj`, `.vcxproj.filters`). `.gitattributes` and `.editorconfig` can enforce this rule.

Do not use Git's `core.autocrlf`. `core.autocrlf` is a local setting that each person can set differently, so it cannot pin down the project's line endings. Set line endings with the `.gitattributes` that is part of the project. A file whose `eol` is set in `.gitattributes` is checked out with that line ending regardless of the `core.autocrlf` value.

Do not use settings such as `text=auto` that let Git decide by itself whether a file is text, either. Automatic detection decides from the contents of each file, so the path alone does not tell which line ending a file gets. Set `text` and `eol` explicitly for each path, and mark binary files with `binary`.

Incorrect:

```text
$ file word_count.c
word_count.c: C source, Unicode text, UTF-8 (with BOM) text, with CRLF line terminators
```

```sh
git config --global core.autocrlf true
```

```gitattributes
* text=auto eol=lf
*.sln text eol=crlf
*.slnx text eol=crlf
*.vcxproj text eol=crlf
*.vcxproj.filters text eol=crlf
```

Correct:

```text
$ file word_count.c
word_count.c: C source, ASCII text
```

```gitattributes
* text eol=lf
*.sln text eol=crlf
*.slnx text eol=crlf
*.vcxproj text eol=crlf
*.vcxproj.filters text eol=crlf
*.png binary
```

### 4.3. Character set

Write code and comments in ASCII. Use characters outside ASCII in output strings only when a meaningful symbol is needed. Quotation marks in messages are the ASCII `'` and `"`.

Incorrect:

```c
/* returns a value ≥ 0 on success */
int parse_unit(const char *const unit);
```

```c
static void report_unknown_unit(const char *const program_name, const char *const unit)
{
    fprintf(stderr, "%s: unknown unit ‘%s’\n", program_name, unit);
}
```

Correct:

```c
/* returns a value >= 0 on success */
int parse_unit(const char *const unit);
```

```c
static void report_unknown_unit(const char *const program_name, const char *const unit)
{
    fprintf(stderr, "%s: unknown unit '%s'\n", program_name, unit);
}
```

Correct (printing a meaningful symbol):

```c
static void print_temperature(double celsius)
{
    printf("%.1f °C\n", celsius);
}
```

### 4.4. Final newline

Every file ends with a newline character.

For C source files, the C standard requires this. C99, the default standard, contains the following sentence in §5.1.1.2 ¶1, translation phase 2.

> A source file that is not empty shall end in a new-line character, which shall not be immediately preceded by a backslash character before any such splicing takes place.

Violating this requirement is undefined behavior under C99 §4 ¶2. C99 Annex J.2 also lists this case as undefined behavior. C11 and C23 contain the same sentence.

Incorrect:

```text
$ tail -c 2 word_count.c | od -c
0000000  \n   }
0000002
```

Correct:

```text
$ tail -c 2 word_count.c | od -c
0000000   }  \n
0000002
```

### 4.5. Whitespace characters

Do not leave spaces at the end of a line. Do not leave spaces or tabs on blank lines. Use tab characters only in Makefile recipes. Do not use form feeds (`^L`) either.

Incorrect:

```text
$ cat -A word_count.c
static int is_all_spaces(const char *const text)  $
{$
^Ifor (size_t i = 0; text[i] != '\0'; ++i) {$
^I^Iif (text[i] != ' ') {$
^I^I^Ireturn 0;$
^I^I}$
^I}$
    $
^Ireturn 1;$
}$
^L$
```

Correct:

```text
$ cat -A word_count.c
static int is_all_spaces(const char *const text)$
{$
    for (size_t i = 0; text[i] != '\0'; ++i) {$
        if (text[i] != ' ') {$
            return 0;$
        }$
    }$
$
    return 1;$
}$
```

```text
$ cat -A Makefile
word_count: word_count.o$
^I$(CC) $(LDFLAGS) -o $@ word_count.o$
```

### 4.6. Order within a file

Write a source file in the following order. Put one blank line between groups.

1. Feature test macros
2. `#include`
3. Macros
4. Types (struct, union, enum)
5. File scope variables
6. Static functions
7. Functions with external linkage
8. `main`

Correct (`scoreboard.h`):

```c
#ifndef SCOREBOARD_H
#define SCOREBOARD_H

void reset_scores(void);

#endif /* SCOREBOARD_H */
```

Correct (`scoreboard.c`):

```c
#define _POSIX_C_SOURCE 200809L

#include "scoreboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PLAYERS 8
#define MAX_NAME_LEN 16

struct player {
    char name[MAX_NAME_LEN];
    int score;
};

static struct player gs_players[MAX_PLAYERS];
static size_t gs_player_count;

static int add_player(const char *const name, int score)
{
    if (gs_player_count == MAX_PLAYERS) {
        return -1;
    }

    struct player *player = &gs_players[gs_player_count];
    size_t length = strnlen(name, MAX_NAME_LEN - 1);

    memcpy(player->name, name, length);
    player->name[length] = '\0';
    player->score = score;
    ++gs_player_count;
    return 0;
}

void reset_scores(void)
{
    for (size_t i = 0; i < gs_player_count; ++i) {
        gs_players[i].score = 0;
    }
}

int main(void)
{
    if (add_player("ada", 7) != 0 || add_player("grace", 9) != 0) {
        return EXIT_FAILURE;
    }

    reset_scores();

    for (size_t i = 0; i < gs_player_count; ++i) {
        printf("%s: %d\n", gs_players[i].name, gs_players[i].score);
    }

    return 0;
}
```

In a project that separates headers and sources, an exception is allowed only where the structure makes this order impossible to keep.

### 4.7. Feature test macros

In a file that uses POSIX or glibc features, enable them with a feature test macro. A feature test macro determines which declarations the system headers expose. Define it in the source file before any `#include`, and put one blank line below it. Do not define it inside a header, because it has no effect on headers included before that header.

Choose the narrowest macro that covers the features needed. Do not define one in a file that uses only standard C features.

- `_POSIX_C_SOURCE`: Define it when using POSIX interfaces. The default value is `200809L`, for POSIX.1-2008. The value for POSIX.1-2024 is `202405L`.
- `_XOPEN_SOURCE`: Define it when also using the POSIX XSI extension. The value is `700` for POSIX.1-2008 and `800` for POSIX.1-2024.
- `_DEFAULT_SOURCE`: Define it when using the default glibc feature set. This set is POSIX.1-2008 plus some BSD and SVID features. glibc turns this set off when another feature test macro is defined or when compiling in a standard mode such as `-std=c99`.
- `_GNU_SOURCE`: It turns on all glibc features and GNU extensions. `_POSIX_C_SOURCE 200809L` and `_XOPEN_SOURCE 700` are turned on as well. Use it only in Linux-only files.
- `WIN32_LEAN_AND_MEAN`, `_WIN32_WINNT`: These are not feature test macros, but put them in the same place. `WIN32_LEAN_AND_MEAN` excludes APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets from `<windows.h>`. `_WIN32_WINNT` sets the lowest Windows version to support. For Windows 10, it is `0x0A00`.

Incorrect:

```c
#include <stdio.h>
#include <stdlib.h>

static long count_lines(FILE *const mut_input)
{
    char *line = NULL;
    size_t capacity = 0;
    long count = 0;

    while (getline(&line, &capacity, mut_input) != -1) {
        ++count;
    }

    free(line);
    line = NULL;
    return count;
}
```

```c
#include <stdio.h>
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

static long count_lines(FILE *const mut_input)
{
    char *line = NULL;
    size_t capacity = 0;
    long count = 0;

    while (getline(&line, &capacity, mut_input) != -1) {
        ++count;
    }

    free(line);
    line = NULL;
    return count;
}
```

```c
#ifndef TEXT_LINES_H
#define TEXT_LINES_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

long count_lines(FILE *const mut_input);

#endif /* TEXT_LINES_H */
```

```c
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

static long count_lines(FILE *const mut_input)
{
    char *line = NULL;
    size_t capacity = 0;
    long count = 0;

    while (getline(&line, &capacity, mut_input) != -1) {
        ++count;
    }

    free(line);
    line = NULL;
    return count;
}
```

```c
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

static void print_total(long total)
{
    printf("total: %ld\n", total);
}
```

Correct (using POSIX interfaces):

```c
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

static long count_lines(FILE *const mut_input)
{
    char *line = NULL;
    size_t capacity = 0;
    long count = 0;

    while (getline(&line, &capacity, mut_input) != -1) {
        ++count;
    }

    free(line);
    line = NULL;
    return count;
}
```

Correct (`text_lines.h`):

```c
#ifndef TEXT_LINES_H
#define TEXT_LINES_H

#include <stdio.h>

long count_lines(FILE *const mut_input);

#endif /* TEXT_LINES_H */
```

Correct (using the XSI extension):

```c
#define _XOPEN_SOURCE 700

#include <time.h>

static int parse_date(struct tm *const mut_date, const char *const text)
{
    if (strptime(text, "%Y-%m-%d", mut_date) == NULL) {
        return -1;
    }

    return 0;
}
```

Correct (using the default glibc features):

```c
#define _DEFAULT_SOURCE

#include <stdlib.h>

static int grow_scores(int **const mut_scores, size_t count)
{
    int *scores = reallocarray(*mut_scores, count, sizeof(**mut_scores));
    if (scores == NULL) {
        return -1;
    }

    *mut_scores = scores;
    return 0;
}
```

Correct (a Linux-only file that uses GNU extensions):

```c
#define _GNU_SOURCE

#include <string.h>

static size_t measure_key(const char *const entry)
{
    return (size_t)(strchrnul(entry, '=') - entry);
}
```

Correct (using only standard C features):

```c
#include <stdio.h>

static void print_total(long total)
{
    printf("total: %ld\n", total);
}
```

Correct (Windows):

```c
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00

#include <windows.h>
```

### 4.8. Include order

Divide `#include` directives into three categories. Sort them alphabetically within each category. However, headers with include-order dependencies on each other follow that order. Put one blank line between categories.

1. Main header: the header that the source file implements (`"foo.h"` for `foo.c`)
2. `<...>` form: standard headers, system headers, and external library headers
3. `"..."` form: headers within the project

Place a header that has a dependency after the headers it needs, at the end of the same group. For example, GNU Readline's `<readline/readline.h>` uses `FILE` but does not include `<stdio.h>` itself. It comes before `<stdio.h>` alphabetically, so leaving it there causes a compile error.

Incorrect:

```c
#include <string.h>
#include "util/text.h"
#include <stdio.h>
#include "report/summary.h"
#include <stdlib.h>
#include "store/inventory.h"
```

```c
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
```

Correct (the includes of `report/summary.c`):

```c
#include "report/summary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "store/inventory.h"
#include "util/text.h"
```

Correct:

```c
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
```

### 4.9. Include guards

Wrap every header in an include guard. Build the guard name from the path relative to the project root by converting it to uppercase and replacing every character other than a letter or digit with `_`. Do not use `#pragma once`.

Always put the guard name in a comment after `#endif`, because in a long header it is not obvious which `#ifndef` the `#endif` pairs with.

Do not start a guard name with `_`. The C standard reserves names that start with `_` followed by an uppercase letter for the implementation, and system headers use them often. Indeed, glibc's `<net/if.h>` uses `_NET_IF_H` as its guard. If a project has a header named `net/if.h` and uses the same name as its guard, the contents of whichever of the two headers is included later are dropped. The compiler reports this with neither an error nor a warning.

Incorrect (the guard name of `util/text.h` not derived from its path):

```c
#ifndef TEXT_H
#define TEXT_H

#include <stddef.h>

size_t count_words(const char *const text);

#endif /* TEXT_H */
```

```c
#pragma once

#include <stddef.h>

size_t count_words(const char *const text);
```

```c
#ifndef UTIL_TEXT_H
#define UTIL_TEXT_H

#include <stddef.h>

size_t count_words(const char *const text);

#endif
```

```c
#ifndef _UTIL_TEXT_H
#define _UTIL_TEXT_H

#include <stddef.h>

size_t count_words(const char *const text);

#endif /* _UTIL_TEXT_H */
```

Correct (`util/text.h`):

```c
#ifndef UTIL_TEXT_H
#define UTIL_TEXT_H

#include <stddef.h>

size_t count_words(const char *const text);

#endif /* UTIL_TEXT_H */
```

### 4.10. Header contents

Put only declarations, type definitions, macros, and inline functions in headers. Define variables and all other functions in a single `.c` file. Declare a variable used in several files with `extern` in a header, and define and initialize it in one `.c` file. Do not add `extern` to function declarations.

Do not put the following definitions in headers.

- Variable definitions: Every translation unit that includes the header gets a definition, which causes a `multiple definition` error at link time.
- `static` variables: Every translation unit that includes the header gets a separate variable. A value changed in one file is not visible in other files.
- `static` functions: Like `static inline` functions, they produce a copy in every translation unit. However, GCC and Clang issue a `-Wunused-function` warning for every file that does not use the function. They do not issue this warning for `static inline`.

Incorrect (`store/inventory.h`):

```c
#ifndef STORE_INVENTORY_H
#define STORE_INVENTORY_H

#define MAX_ITEM_NAME_LEN 32

struct item {
    char name[MAX_ITEM_NAME_LEN];
    int quantity;
};

int g_low_stock_limit = 5;
static int gs_item_count;

static int is_low_stock(const struct item *const item)
{
    return item->quantity < g_low_stock_limit;
}

extern int add_item(const char *const name, int quantity);

#endif /* STORE_INVENTORY_H */
```

Correct (`store/inventory.h`):

```c
#ifndef STORE_INVENTORY_H
#define STORE_INVENTORY_H

#define MAX_ITEM_NAME_LEN 32

struct item {
    char name[MAX_ITEM_NAME_LEN];
    int quantity;
};

extern int g_low_stock_limit;

static inline int is_low_stock(const struct item *const item)
{
    return item->quantity < g_low_stock_limit;
}

int add_item(const char *const name, int quantity);

#endif /* STORE_INVENTORY_H */
```

Correct (`store/inventory.c`):

```c
#include "store/inventory.h"

#include <string.h>

#define MAX_ITEMS 64

int g_low_stock_limit = 5;

static struct item gs_items[MAX_ITEMS];
static int gs_item_count;

int add_item(const char *const name, int quantity)
{
    size_t length = strlen(name);
    if (gs_item_count == MAX_ITEMS || length >= MAX_ITEM_NAME_LEN) {
        return -1;
    }

    struct item *item = &gs_items[gs_item_count];

    memcpy(item->name, name, length + 1);
    item->quantity = quantity;
    ++gs_item_count;
    return 0;
}
```

## 5. Formatting

### 5.1. Indentation

Indent with four spaces. Do not indent with tabs.

Incorrect:

```c
static const char *describe_temperature(int celsius)
{
  if (celsius < 0) {
    return "freezing";
  }
  if (celsius < 25) {
    return "mild";
  }
  return "hot";
}
```

Correct:

```c
static const char *describe_temperature(int celsius)
{
    if (celsius < 0) {
        return "freezing";
    }
    if (celsius < 25) {
        return "mild";
    }
    return "hot";
}
```

### 5.2. Line length

As a rule, keep lines within 100 columns. However, a line may exceed this if the excess is small.

Incorrect:

```c
static void print_order_summary(const char *const customer, unsigned int item_count, unsigned long total_cents, unsigned int delivery_days)
{
    printf("order for %s: %u items, total %lu.%02lu, delivery in %u business days\n", customer, item_count, total_cents / 100, total_cents % 100, delivery_days);
}
```

Correct:

```c
static void print_order_summary(const char *const customer, unsigned int item_count,
                                unsigned long total_cents, unsigned int delivery_days)
{
    printf("order for %s: %u items, total %lu.%02lu, delivery in %u business days\n", customer,
           item_count, total_cents / 100, total_cents % 100, delivery_days);
}
```

### 5.3. Breaking long expressions

When breaking a line, use the following layout.

- When breaking inside parentheses, start the next line in the column right after the innermost opening parenthesis that is still unclosed. That is, the first character of the next line and the first character after that parenthesis sit in the same column. This applies to function call arguments, function declaration parameters, and the conditions of `if` and `while`.
- When breaking an expression outside parentheses after an operator, start the next line in the same column as the first operand of that expression.
- By default, break an assignment inside the parentheses of the right-hand side, not after the assignment operator. Only when the right-hand side is so long that breaking inside its parentheses leaves no room for the next line, break right after the assignment operator, and indent the next line four spaces more than the first line of the statement.
- Leave binary operators and commas at the end of the line.

Incorrect:

```c
static int format_location(char *const mut_buffer, size_t buffer_size, const char *const file_name,
                           int line_number, int column_number)
{
    int written = snprintf(mut_buffer, buffer_size, "%s:%d:%d: unexpected token\n", file_name,
        line_number, column_number);

    return written;
}
```

```c
static int format_location(char *const mut_buffer, size_t buffer_size, const char *const file_name,
                           int line_number, int column_number)
{
    int written =
        snprintf(mut_buffer, buffer_size, "%s:%d:%d: unexpected token\n", file_name, line_number,
                 column_number);

    return written;
}
```

```c
static long sum_section_bytes(long metadata_bytes, long thumbnail_bytes, long pixel_bytes,
                              long color_profile_bytes, long annotation_bytes, long checksum_bytes)
{
    long total = metadata_bytes + thumbnail_bytes + pixel_bytes + color_profile_bytes
                 + annotation_bytes + checksum_bytes;

    return total;
}
```

Correct:

```c
static int format_location(char *const mut_buffer, size_t buffer_size, const char *const file_name,
                           int line_number, int column_number)
{
    int written = snprintf(mut_buffer, buffer_size, "%s:%d:%d: unexpected token\n", file_name,
                           line_number, column_number);

    return written;
}
```

```c
static long sum_section_bytes(long metadata_bytes, long thumbnail_bytes, long pixel_bytes,
                              long color_profile_bytes, long annotation_bytes, long checksum_bytes)
{
    long total = metadata_bytes + thumbnail_bytes + pixel_bytes + color_profile_bytes +
                 annotation_bytes + checksum_bytes;

    return total;
}
```

```c
int is_supported_format(int format, int bits_per_pixel);

static int can_display(int width, int height, int format, int bits_per_pixel, int max_width,
                       int max_height)
{
    if (width <= max_width && height <= max_height &&
        is_supported_format(format, bits_per_pixel) != 0) {
        return 1;
    }

    return 0;
}
```

Correct when the right-hand side is very long:

```c
unsigned long estimate_remaining_conversion_time_ms(unsigned long first_job, unsigned long last_job,
                                                    unsigned int worker_count);

static unsigned long report_remaining_time(unsigned long first_job, unsigned long last_job,
                                           unsigned int worker_count)
{
    unsigned long remaining_conversion_time_ms =
        estimate_remaining_conversion_time_ms(first_job, last_job, worker_count);

    return remaining_conversion_time_ms;
}
```

### 5.4. User-visible strings

Do not split output message strings, even when they exceed the line length. Messages must remain findable with string search tools such as grep. A string that prints several lines may be split at output line boundaries and concatenated.

Incorrect:

```c
static void report_missing_value(const char *const key, int line_number)
{
    fprintf(stderr, "settings: key '%s' on line %d has no value, "
                    "expected key=value\n",
            key, line_number);
}
```

Correct:

```c
static void report_missing_value(const char *const key, int line_number)
{
    fprintf(stderr, "settings: key '%s' on line %d has no value, expected key=value\n", key, line_number);
}
```

```c
static void show_help(const char *const program_name)
{
    printf("usage: %s [options] <file>\n"
           "  -c  count characters\n"
           "  -w  count words\n",
           program_name);
}
```

### 5.5. Function headers

Write the return type, function name, and parameter list on one line. If the line is too long, break it inside the parameter list.

Incorrect:

```c
static int
clamp_channel(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        return 255;
    }
    return value;
}
```

```c
static void blend_rows(uint8_t *const restrict mut_dst, const uint8_t *const restrict src, size_t count, unsigned int alpha)
{
    for (size_t i = 0; i < count; ++i) {
        mut_dst[i] = (uint8_t)((src[i] * alpha + mut_dst[i] * (255U - alpha)) / 255U);
    }
}
```

Correct:

```c
static int clamp_channel(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        return 255;
    }
    return value;
}
```

```c
static void blend_rows(uint8_t *const restrict mut_dst, const uint8_t *const restrict src,
                       size_t count, unsigned int alpha)
{
    for (size_t i = 0; i < count; ++i) {
        mut_dst[i] = (uint8_t)((src[i] * alpha + mut_dst[i] * (255U - alpha)) / 255U);
    }
}
```

### 5.6. Function braces

Put the opening and closing braces of a function body each on a new line. Their indentation matches the function header.

Incorrect:

```c
static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}
```

Correct:

```c
static int is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}
```

### 5.7. Control statement and loop braces

Put the opening brace of `if`, `else`, `for`, `while`, `do`, and `switch` at the end of the same line. Put the closing brace on a new line. If the same statement continues, write it right after the closing brace (`} else {`, `} while (...);`).

In Windows-only projects, however, follow the Windows convention and use the Allman style, which puts the opening brace on a new line.

Incorrect:

```c
static int adjust_level(int level, const int *const history, size_t count)
{
    if (level < 0)
    {
        level = 0;
    }
    else if (level > 100)
    {
        level = 100;
    }
    else
    {
        level += 1;
    }

    for (size_t i = 0; i < count; ++i)
    {
        level += history[i];
    }

    while (level > 100)
    {
        level -= 10;
    }

    do
    {
        level += 2;
    }
    while (level % 5 != 0);

    switch (level / 25)
    {
    case 0:
        level = 25;
        break;
    default:
        break;
    }

    return level;
}
```

Correct:

```c
static int adjust_level(int level, const int *const history, size_t count)
{
    if (level < 0) {
        level = 0;
    } else if (level > 100) {
        level = 100;
    } else {
        level += 1;
    }

    for (size_t i = 0; i < count; ++i) {
        level += history[i];
    }

    while (level > 100) {
        level -= 10;
    }

    do {
        level += 2;
    } while (level % 5 != 0);

    switch (level / 25) {
    case 0:
        level = 25;
        break;
    default:
        break;
    }

    return level;
}
```

### 5.8. Mandatory braces

Always wrap the body of every control statement in braces. This applies to `if`, `else`, `for`, `while`, and `do`. It applies even when the body is a single statement.

Omitting braces makes it easy to introduce a bug when adding a statement to the body. For example, if `do_after();` is added to the body of `if (cond) do_something();` at the same indentation, both statements look as if they belong to the `if`. However, only `do_something();` belongs to the `if`, and `do_after();` always runs regardless of the condition. Braces are always used to prevent such bugs.

Incorrect:

```c
void stop_motor(void);
void log_event(const char *const message);

static void check_motor(int temperature, int limit)
{
    if (temperature > limit)
        stop_motor();
        log_event("motor stopped");
}
```

```c
static int compute_absolute_value(int value)
{
    if (value < 0)
        return -value;
    else
        return value;
}

static long sum_values(const int *const values, size_t count)
{
    long sum = 0;

    for (size_t i = 0; i < count; ++i)
        sum += values[i];

    return sum;
}

static size_t skip_spaces(const char *const text)
{
    size_t i = 0;

    while (text[i] == ' ')
        ++i;

    return i;
}

static long find_next_power_of_ten(long value)
{
    long scale = 1;

    do
        scale *= 10;
    while (scale <= value);

    return scale;
}
```

Correct:

```c
void stop_motor(void);
void log_event(const char *const message);

static void check_motor(int temperature, int limit)
{
    if (temperature > limit) {
        stop_motor();
        log_event("motor stopped");
    }
}
```

```c
static int compute_absolute_value(int value)
{
    if (value < 0) {
        return -value;
    } else {
        return value;
    }
}

static long sum_values(const int *const values, size_t count)
{
    long sum = 0;

    for (size_t i = 0; i < count; ++i) {
        sum += values[i];
    }

    return sum;
}

static size_t skip_spaces(const char *const text)
{
    size_t i = 0;

    while (text[i] == ' ') {
        ++i;
    }

    return i;
}

static long find_next_power_of_ten(long value)
{
    long scale = 1;

    do {
        scale *= 10;
    } while (scale <= value);

    return scale;
}
```

### 5.9. struct, union, and enum braces

Put the opening brace of a struct, union, or enum on the same line.

Incorrect:

```c
enum traffic_light
{
    TRAFFIC_LIGHT_RED,
    TRAFFIC_LIGHT_YELLOW,
    TRAFFIC_LIGHT_GREEN,
};

struct rgb_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

union float_bits
{
    float value;
    uint32_t bits;
};
```

Correct:

```c
enum traffic_light {
    TRAFFIC_LIGHT_RED,
    TRAFFIC_LIGHT_YELLOW,
    TRAFFIC_LIGHT_GREEN,
};

struct rgb_color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

union float_bits {
    float value;
    uint32_t bits;
};
```

### 5.10. Scope-narrowing blocks

For a block made to narrow a scope, put the opening and closing braces each on a new line.

Incorrect:

```c
static void print_totals(long online_sales, long store_sales, long returns)
{
    { long sold = online_sales + store_sales;
        printf("sold: %ld\n", sold); }

    printf("returned: %ld\n", returns);
}
```

Correct:

```c
static void print_totals(long online_sales, long store_sales, long returns)
{
    {
        long sold = online_sales + store_sales;

        printf("sold: %ld\n", sold);
    }

    printf("returned: %ld\n", returns);
}
```

### 5.11. Brace initialization

Put spaces just inside the braces of an initializer list. Put a comma after the last element only in a list that may grow. In a list that will grow, such as an enum or a table, put each element on its own line and put a comma after the last element as well. Adding or removing an element then changes only one line in the diff.

When initializing a structure with a designated initializer, put each member on its own line by default.

Incorrect:

```c
static const int gs_first_primes[] = {2, 3, 5, 7};
```

```c
enum image_format { IMAGE_FORMAT_PNG, IMAGE_FORMAT_JPEG, IMAGE_FORMAT_GIF };

static const char *const gs_image_extensions[] = { ".png", ".jpg", ".gif" };
```

```c
struct window_size {
    int width;
    int height;
};

static const struct window_size gs_default_size = { .width = 800, .height = 600 };
```

Correct:

```c
static const int gs_first_primes[] = { 2, 3, 5, 7 };
```

```c
enum image_format {
    IMAGE_FORMAT_PNG,
    IMAGE_FORMAT_JPEG,
    IMAGE_FORMAT_GIF,
};

static const char *const gs_image_extensions[] = {
    ".png",
    ".jpg",
    ".gif",
};
```

```c
struct window_size {
    int width;
    int height;
};

static const struct window_size gs_default_size = {
    .width = 800,
    .height = 600
};
```

### 5.12. switch and case

Put `case` and `default` labels in the same column as `switch`. Do not squeeze a `case` body onto one line.

Incorrect:

```c
static int convert_grade(char grade)
{
    int points = 0;

    switch (grade) {
        case 'A':
            points = 4;
            break;
        case 'B':
            points = 3;
            break;
        default:
            points = 0;
            break;
    }

    return points;
}
```

```c
static int convert_grade(char grade)
{
    int points = 0;

    switch (grade) {
    case 'A': points = 4; break;
    case 'B': points = 3; break;
    default: points = 0; break;
    }

    return points;
}
```

Correct:

```c
static int convert_grade(char grade)
{
    int points = 0;

    switch (grade) {
    case 'A':
        points = 4;
        break;
    case 'B':
        points = 3;
        break;
    default:
        points = 0;
        break;
    }

    return points;
}
```

### 5.13. goto labels

Put a goto label in the same column as the braces of the enclosing block. A label in a function body goes in column 0.

Incorrect:

```c
struct session {
    FILE *log;
    char *buffer;
    int *scores;
};

static int open_session(struct session *const mut_session, const char *const log_path)
{
    mut_session->log = fopen(log_path, "a");
    if (mut_session->log == NULL) {
        return -1;
    }

    mut_session->buffer = malloc(4096);
    if (mut_session->buffer == NULL) {
        goto handle_buffer_alloc;
    }

    mut_session->scores = calloc(64, sizeof(*mut_session->scores));
    if (mut_session->scores == NULL) {
        goto handle_scores_alloc;
    }

    return 0;

    handle_scores_alloc:
    free(mut_session->buffer);
    mut_session->buffer = NULL;
    handle_buffer_alloc:
    fclose(mut_session->log);
    mut_session->log = NULL;
    return -1;
}
```

Correct:

```c
struct session {
    FILE *log;
    char *buffer;
    int *scores;
};

static int open_session(struct session *const mut_session, const char *const log_path)
{
    mut_session->log = fopen(log_path, "a");
    if (mut_session->log == NULL) {
        return -1;
    }

    mut_session->buffer = malloc(4096);
    if (mut_session->buffer == NULL) {
        goto handle_buffer_alloc;
    }

    mut_session->scores = calloc(64, sizeof(*mut_session->scores));
    if (mut_session->scores == NULL) {
        goto handle_scores_alloc;
    }

    return 0;

handle_scores_alloc:
    free(mut_session->buffer);
    mut_session->buffer = NULL;
handle_buffer_alloc:
    fclose(mut_session->log);
    mut_session->log = NULL;
    return -1;
}
```

```c
static size_t count_rows_without_zero(const int *const cells, size_t row_count, size_t column_count)
{
    size_t count = 0;

    for (size_t row = 0; row < row_count; ++row) {
        for (size_t column = 0; column < column_count; ++column) {
            if (cells[row * column_count + column] == 0) {
                goto next_row;
            }
        }

        ++count;
    next_row:
        continue;
    }

    return count;
}
```

### 5.14. Spaces

- Put a space after keywords such as `if`, `for`, `while`, `switch`, and `return`.
- Do not put a space between a function name and `(`, or between `sizeof` and `(`.
- Put spaces on both sides of binary operators and the conditional operator.
- Do not put spaces between unary operators, `.`, `->`, or increment and decrement operators and their operands.
- Put a space after a comma, but not before it.
- Do not put spaces just inside parentheses or after a cast.

Incorrect:

```c
struct point {
    int x;
    int y;
};

static int sum_distances(const struct point *const points, size_t count, bool is_closed)
{
    int total = 0;
    const struct point *last = & points[count - 1];

    for(size_t i = 1; i<count; ++ i) {
        total+=abs (points[i] . x-points[i - 1] . x)+abs( points[i].y - points[i - 1].y );
    }

    if(! is_closed) {
        total -= abs(last -> x - points[0].x);
    }

    while(total > INT_MAX / 2) {
        total /= 2;
    }

    switch(count) {
    case 0:
        -- total;
        break;
    default:
        break;
    }

    int unit = sizeof (*points)*count>4096?2:1;

    return(int) (total / unit);
}
```

Correct:

```c
struct point {
    int x;
    int y;
};

static int sum_distances(const struct point *const points, size_t count, bool is_closed)
{
    int total = 0;
    const struct point *last = &points[count - 1];

    for (size_t i = 1; i < count; ++i) {
        total += abs(points[i].x - points[i - 1].x) + abs(points[i].y - points[i - 1].y);
    }

    if (!is_closed) {
        total -= abs(last->x - points[0].x);
    }

    while (total > INT_MAX / 2) {
        total /= 2;
    }

    switch (count) {
    case 0:
        --total;
        break;
    default:
        break;
    }

    int unit = (sizeof(*points) * count > 4096) ? 2 : 1;

    return (int)(total / unit);
}
```

### 5.15. Pointer notation

Attach `*` to the name. The same applies to functions that return pointers. In Windows-only projects, however, follow the Windows convention and attach `*` to the type.

Incorrect:

```c
struct menu;

const char* find_label(const struct menu* const menu, int id);
char* copy_label(const char* const label);
```

Correct:

```c
struct menu;

const char *find_label(const struct menu *const menu, int id);
char *copy_label(const char *const label);
```

### 5.16. One statement per line where possible

Unless there is a specific reason, write only one statement per line.

Incorrect:

```c
struct size {
    int width;
    int height;
};

static void reset_size(struct size *const mut_size)
{
    mut_size->width = 0; mut_size->height = 0;
}
```

Correct:

```c
struct size {
    int width;
    int height;
};

static void reset_size(struct size *const mut_size)
{
    mut_size->width = 0;
    mut_size->height = 0;
}
```

### 5.17. Blank lines

Insert blank lines as appropriate between chunks where the task or context changes.

Correct:

```c
static long count_long_lines(const char *const path, size_t limit)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return -1;
    }

    long count = 0;
    size_t length = 0;

    for (int c = fgetc(file); c != EOF; c = fgetc(file)) {
        if (c != '\n') {
            ++length;
            continue;
        }

        if (length > limit) {
            ++count;
        }
        length = 0;
    }

    fclose(file);
    file = NULL;
    return count;
}
```

### 5.18. Alignment

Do not align consecutive assignments, declarations, macros, or trailing comments in columns, because editing one line would then require realigning the neighboring lines.

However, align the line-ending `\` of a multi-line macro in one column. Write the `\` one space after the last character of the longest line, and put the `\` of every other line in that column. When each `\` sits right next to the code at a different position on every line, the code is hard to read, and it is hard to see at a glance that the block is a macro. When the `\` characters line up in one column, the extent of the macro is visible at once, and a missing `\` stands out immediately.

Incorrect:

```c
#define MAX_ACCOUNTS  64
#define NAME_LEN      32

struct account {
    char   name[NAME_LEN]; /* login name */
    long   balance;        /* in cents */
    double interest_rate;  /* per year */
};

static void open_account(struct account *const mut_account)
{
    mut_account->balance       = 0;
    mut_account->interest_rate = 0.015;
}
```

```c
#define RESET_RANGE(range) \
    do { \
        (range)->first = 0; \
        (range)->last = -1; \
    } while (0)
```

Correct:

```c
#define MAX_ACCOUNTS 64
#define NAME_LEN 32

struct account {
    char name[NAME_LEN]; /* login name */
    long balance; /* in cents */
    double interest_rate; /* per year */
};

static void open_account(struct account *const mut_account)
{
    mut_account->balance = 0;
    mut_account->interest_rate = 0.015;
}
```

```c
#define RESET_RANGE(range)  \
    do {                    \
        (range)->first = 0; \
        (range)->last = -1; \
    } while (0)
```

### 5.19. Preprocessing directives

Put the `#` of a preprocessing directive at the very start of the line. The same applies to directives inside functions. For a directive inside a conditional compilation block, put one space after `#` for each level of block depth. The include guard does not count toward the depth. With `#` always at the start of the line, directives stand apart from code, and the spaces after `#` show the nesting depth.

Incorrect:

```c
static char get_path_separator(void)
{
    #if defined(_WIN32)
    return '\\';
    #else
    return '/';
    #endif
}
```

```c
#if defined(__linux__)
    #if defined(__GLIBC__)
        #define LIBC_NAME "glibc"
    #else
        #define LIBC_NAME "other libc"
    #endif
#else
    #define LIBC_NAME "unknown"
#endif
```

```c
#if defined(__linux__)
#if defined(__GLIBC__)
#define LIBC_NAME "glibc"
#else
#define LIBC_NAME "other libc"
#endif
#else
#define LIBC_NAME "unknown"
#endif
```

Correct:

```c
static char get_path_separator(void)
{
#if defined(_WIN32)
    return '\\';
#else
    return '/';
#endif
}
```

```c
#if defined(__linux__)
# if defined(__GLIBC__)
#  define LIBC_NAME "glibc"
# else
#  define LIBC_NAME "other libc"
# endif
#else
# define LIBC_NAME "unknown"
#endif
```

Correct (`platform/paths.h`):

```c
#ifndef PLATFORM_PATHS_H
#define PLATFORM_PATHS_H

#if defined(_WIN32)
# define PATH_SEPARATOR '\\'
#else
# define PATH_SEPARATOR '/'
#endif

#endif /* PLATFORM_PATHS_H */
```

### 5.20. GCC/Clang inline assembly

In inline assembly, write one instruction per string. End every string except the last with `\n\t`.

`\n\t` is the separator that the Extended Asm section of the GCC manual recommends. `\n` breaks the line between instructions, and `\t` indents each instruction to the instruction field in the assembly that the compiler emits. The compiler itself breaks the line after the last instruction of an asm statement. So appending `\n\t` to the last string adds an extra line that holds only a tab.

Incorrect:

```c
static inline uint32_t read_max_cpuid_leaf(void)
{
    uint32_t leaf;

    __asm__ volatile("xorl %%eax, %%eax; cpuid"
                     : "=a"(leaf)
                     :
                     : "ebx", "ecx", "edx");
    return leaf;
}
```

Correct:

```c
static inline uint32_t read_max_cpuid_leaf(void)
{
    uint32_t leaf;

    __asm__ volatile("xorl %%eax, %%eax\n\t"
                     "cpuid"
                     : "=a"(leaf)
                     :
                     : "ebx", "ecx", "edx");
    return leaf;
}
```

## 6. Naming

### 6.1. Function and variable names

Name functions, variables, parameters, struct/union/enum tags, and goto labels in lowercase snake_case. In cross-platform projects, Windows-specific code follows this rule too.

Incorrect:

```c
struct GroceryList {
    int itemCount;
    long totalCents;
};

union PriceValue {
    long Cents;
    double Ratio;
};

enum CartState {
    CART_STATE_OPEN,
    CART_STATE_PAID,
};

static int AddItem(struct GroceryList *const pList, long PriceCents)
{
    if (PriceCents < 0) {
        goto InvalidPrice;
    }

    ++pList->itemCount;
    pList->totalCents += PriceCents;
    return 0;

InvalidPrice:
    return -1;
}
```

Correct:

```c
struct grocery_list {
    int item_count;
    long total_cents;
};

union price_value {
    long cents;
    double ratio;
};

enum cart_state {
    CART_STATE_OPEN,
    CART_STATE_PAID,
};

static int add_item(struct grocery_list *const mut_list, long price_cents)
{
    if (price_cents < 0) {
        goto invalid_price;
    }

    ++mut_list->item_count;
    mut_list->total_cents += price_cents;
    return 0;

invalid_price:
    return -1;
}
```

### 6.2. Macro and enumeration constant names

Name macros and enumeration constants in uppercase snake_case. Prefixing enumeration constants with the enumeration name is not mandatory.

Incorrect:

```c
#define maxRetries 3

enum shape {
    Circle,
    Square,
};
```

Correct:

```c
#define MAX_RETRIES 3

enum shape {
    SHAPE_CIRCLE,
    SHAPE_SQUARE,
};
```

Correct (without the enumeration name as a prefix):

```c
enum direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
};
```

### 6.3. Function-like macros

Name function-like macros in uppercase too. As an exception, lowercase is allowed for conventional names that wrap compiler builtins, such as `likely` and `unlikely`.

Incorrect:

```c
#define to_string(x) #x
```

Correct:

```c
#define TO_STRING(x) #x
```

Correct (conventional names that wrap compiler builtins):

```c
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
```

### 6.4. Names of const objects

The variable rules apply to const objects too. Use uppercase names only for macros and enumeration constants.

Incorrect:

```c
static const int DAYS_IN_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
```

Correct:

```c
static const int gs_days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
```

### 6.5. Name length and content

- Make names with external linkage and names at file scope descriptive.
- A local variable with a narrow scope may take a short name whose conventional use is well established (`i`, `n`, `tmp`, `iter`, and so on).
- Do not attach type-indicating prefixes or suffixes to names (Hungarian notation). A name expresses the meaning of the value.
- Use only widely used abbreviations.
- Do not use single-letter names that are easily confused with digits, such as `l` and `O`, or names that differ only in case.

Incorrect:

```c
struct reading {
    int celsius;
};

static int avgtmp(const struct reading *const pReadings, size_t nCount)
{
    long l = 0;
    long L = 0;

    for (size_t i = 0; i < nCount; ++i) {
        l += pReadings[i].celsius;
        ++L;
    }

    return (int)(l / L);
}
```

Correct:

```c
struct reading {
    int celsius;
};

static int compute_average_celsius(const struct reading *const readings, size_t count)
{
    long sum = 0;

    for (size_t i = 0; i < count; ++i) {
        sum += readings[i].celsius;
    }

    return (int)(sum / (long)count);
}
```

```c
static void fill_bytes(unsigned char *const mut_buffer, size_t size, unsigned char value)
{
    for (size_t i = 0; i < size; ++i) {
        mut_buffer[i] = value;
    }
}
```

### 6.6. First word of function names

Start function names with a verb. This applies to functions that return a value too. If there is a module prefix, the word after the prefix is the verb.

Incorrect:

```c
struct rect {
    double width;
    double height;
};

double perimeter(const struct rect *const rect);
unsigned int checksum(const unsigned char *const bytes, size_t count);
int cart_item_add(const char *const name);
```

Correct:

```c
struct rect {
    double width;
    double height;
};

double compute_perimeter(const struct rect *const rect);
unsigned int calculate_checksum(const unsigned char *const bytes, size_t count);
int cart_add_item(const char *const name);
```

### 6.7. Names for truth values

Name functions and function-like macros that return a truth value, and variables, parameters, and struct members that hold one, in one of the following two forms.

- Start with the verb be or a verb that forms questions. After `is_`, put a noun or adjective as the complement; after `has_`, a noun or past participle; after `can_` and `should_`, a verb in its base form (`is_empty`, `is_directory`, `has_children`, `has_found`, `can_retry`, `should_flush`).
- Start with a third-person verb that describes a state (`exists`, `contains_key`, `matches_pattern`).

Incorrect:

```c
#define EVEN(n) (((n) % 2) == 0)

struct download {
    bool finished;
    int retry;
};

bool palindrome(const char *const text);
bool check_key(const char *const key);
bool find_name(const char *const *const names, size_t count, const char *const name);
```

```c
static size_t count_code_lines(const char *const *const lines, size_t count)
{
    size_t code_lines = 0;
    int inside_comment = 0;

    for (size_t i = 0; i < count; ++i) {
        if (strncmp(lines[i], "/*", 2) == 0) {
            inside_comment = 1;
        }

        if (inside_comment == 0) {
            ++code_lines;
        }

        if (strstr(lines[i], "*/") != NULL) {
            inside_comment = 0;
        }
    }

    return code_lines;
}
```

```c
static void report_negative(const int *const values, size_t count)
{
    bool found = false;

    for (size_t i = 0; i < count; ++i) {
        if (values[i] < 0) {
            found = true;
            break;
        }
    }

    puts((found) ? "negative value found" : "all values are non-negative");
}
```

Correct:

```c
#define IS_EVEN(n) (((n) % 2) == 0)

struct download {
    bool is_finished;
    int can_retry;
};

bool is_palindrome(const char *const text);
bool is_valid_key(const char *const key);
bool contains_name(const char *const *const names, size_t count, const char *const name);
```

```c
static size_t count_code_lines(const char *const *const lines, size_t count)
{
    size_t code_lines = 0;
    int is_inside_comment = 0;

    for (size_t i = 0; i < count; ++i) {
        if (strncmp(lines[i], "/*", 2) == 0) {
            is_inside_comment = 1;
        }

        if (is_inside_comment == 0) {
            ++code_lines;
        }

        if (strstr(lines[i], "*/") != NULL) {
            is_inside_comment = 0;
        }
    }

    return code_lines;
}
```

```c
static void report_negative(const int *const values, size_t count)
{
    bool has_found = false;

    for (size_t i = 0; i < count; ++i) {
        if (values[i] < 0) {
            has_found = true;
            break;
        }
    }

    puts((has_found) ? "negative value found" : "all values are non-negative");
}
```

### 6.8. Global and static variables

The prefix of a variable name depends on whether the variable has file scope and whether it is `static`. `g` stands for file scope, and `s` for `static`.

- A file scope variable without `static` starts with `g_`.
- A file scope variable with `static` starts with `gs_`.
- A `static` variable inside a function starts with `s_`.
- A local variable without `static` takes no prefix.

Variables with `g_`, `gs_`, or `s_` all have static storage duration, so they live until the program ends and are initialized only once, before program startup. They differ in scope and linkage. A `g_` variable is visible from other files too, while a `gs_` variable is visible only within its file. An `s_` variable is visible only within its function, but its value persists between calls. The prefix reveals this lifetime and scope wherever the variable is used.

Incorrect:

```c
int font_size = 12;
static unsigned int g_retry_limit = 3;

static unsigned int issue_ticket(void)
{
    static unsigned int last_ticket;

    ++last_ticket;
    unsigned int s_ticket = last_ticket;

    return s_ticket;
}
```

Correct:

```c
int g_font_size = 12;
static unsigned int gs_retry_limit = 3;

static unsigned int issue_ticket(void)
{
    static unsigned int s_last_ticket;

    ++s_last_ticket;
    unsigned int ticket = s_last_ticket;

    return ticket;
}
```

### 6.9. typedef

Avoid typedefs for structs, unions, and enums as far as possible. Use the tag as it is, as in `struct point`. In particular, unless there is a specific reason, declare a variable of an enum type with the `enum` tag instead of hiding the enum behind a typedef. A typedef suits opaque types, which hide their internals, and function pointer types.

Discouraged:

```c
typedef struct point {
    int x;
    int y;
} point;

typedef enum {
    FAN_SPEED_LOW,
    FAN_SPEED_HIGH,
} fan_speed;

static fan_speed gs_fan_speed = FAN_SPEED_HIGH;
```

Correct:

```c
struct point {
    int x;
    int y;
};

enum fan_speed {
    FAN_SPEED_LOW,
    FAN_SPEED_HIGH,
};

static enum fan_speed gs_fan_speed = FAN_SPEED_HIGH;
```

```c
typedef struct audio_device audio_device;
typedef void (*sample_callback)(const float *const samples, size_t count);

audio_device *open_audio_device(const char *const name, sample_callback on_samples);
```

### 6.10. Reserved identifiers

The C standard reserves some names for the implementation. The implementation is the C standard's term for the compiler and the standard library together. Standard headers and compilers use such names for internal macros and functions. Declaring or defining a reserved identifier in a program is undefined behavior (C99 §7.1.3 ¶2).

Do not use the following names.

- Names that start with `_` and an uppercase letter (`_STORE_CART_H`) and names that start with `__` (`__retry_count`): They are reserved for any use (C99 §7.1.3 ¶1).
- Names that start with `_` at file scope (`_buffer_size`): They are reserved for use as identifiers with file scope (C99 §7.1.3 ¶1).
- Names that end in `_t` (`item_id_t`): POSIX allows the implementation to add such names to any header (POSIX.1-2024 XSH 2.2.2). glibc also reserves such names for type names (glibc manual: Reserved Names).

Feature test macros that a standard tells the user to define, such as `_POSIX_C_SOURCE`, are an exception.

Incorrect (`store/cart.h`):

```c
#ifndef _STORE_CART_H
#define _STORE_CART_H

typedef unsigned long item_id_t;

extern int __retry_count;

#endif /* _STORE_CART_H */
```

Incorrect (`store/cart.c`):

```c
#include "store/cart.h"

static int _buffer_size;
```

Correct (`store/cart.h`):

```c
#ifndef STORE_CART_H
#define STORE_CART_H

typedef unsigned long item_id;

extern int g_retry_count;

#endif /* STORE_CART_H */
```

Correct (`store/cart.c`):

```c
#include "store/cart.h"

static int gs_buffer_size;
```

### 6.11. Win32 API symbols and types

When developing cross-platform code, write the names of the Win32 API and its symbols (`CreateFileW`, `HANDLE`, `SOCKET`) in their original spelling. Do not wrap Win32 API names in other names, and do not imitate the Win32 API notation in names of your own.

Incorrect:

```c
#include <windows.h>

typedef HANDLE log_handle;
#define close_log CloseHandle

static log_handle OpenLogFileW(LPCWSTR pszPath)
{
    return CreateFileW(pszPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
}

static void CloseLogFile(log_handle hFile)
{
    close_log(hFile);
}
```

Correct (Windows):

```c
#include <windows.h>

static HANDLE open_log_file(const wchar_t *const path)
{
    return CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
}

static void close_log_file(HANDLE file)
{
    CloseHandle(file);
}
```

## 7. Declarations and types

### 7.1. One declaration per line

Declare only one variable per line. However, in the init clause of a `for` statement, one declaration may hold several loop variables that are handled together.

Incorrect:

```c
static void print_space_positions(const char *const text)
{
    const char *first_space = strchr(text, ' '), *last_space = strrchr(text, ' ');
    size_t first_index = 0, last_index = 0;

    if (first_space != NULL) {
        first_index = (size_t)(first_space - text);
        last_index = (size_t)(last_space - text);
    }

    printf("spaces at %zu and %zu\n", first_index, last_index);
}
```

Correct:

```c
static void print_space_positions(const char *const text)
{
    const char *first_space = strchr(text, ' ');
    const char *last_space = strrchr(text, ' ');
    size_t first_index = 0;
    size_t last_index = 0;

    if (first_space != NULL) {
        first_index = (size_t)(first_space - text);
        last_index = (size_t)(last_space - text);
    }

    printf("spaces at %zu and %zu\n", first_index, last_index);
}
```

### 7.2. Declaration placement

Declare a variable where it is first used, in the narrowest scope. Declare loop variables in the `for` init clause.

Incorrect:

```c
struct order {
    long amount_cents;
    int is_paid;
};

static long sum_unpaid(const struct order *const orders, size_t count)
{
    size_t i;
    long total;
    const struct order *order;

    total = 0;
    for (i = 0; i < count; ++i) {
        order = &orders[i];
        if (order->is_paid == 0) {
            total += order->amount_cents;
        }
    }

    return total;
}
```

Correct:

```c
struct order {
    long amount_cents;
    int is_paid;
};

static long sum_unpaid(const struct order *const orders, size_t count)
{
    long total = 0;

    for (size_t i = 0; i < count; ++i) {
        const struct order *order = &orders[i];

        if (order->is_paid == 0) {
            total += order->amount_cents;
        }
    }

    return total;
}
```

However, projects that choose C89 are an exception. C89 does not allow declarations after statements or inside a `for` init clause. So declare a variable at the start of the narrowest block that uses it, and declare a loop variable at the start of the block that encloses the loop.

Correct in a C89 project:

```c
struct order {
    long amount_cents;
    int is_paid;
};

static long sum_unpaid(const struct order *const orders, size_t count)
{
    long total = 0;
    size_t i;

    for (i = 0; i < count; ++i) {
        const struct order *order = &orders[i];

        if (order->is_paid == 0) {
            total += order->amount_cents;
        }
    }

    return total;
}
```

### 7.3. Scope and shadowing

Keep a variable used by only one function inside that function.

Declare functions and file scope variables `static` by default. Leave out `static` only when there is a specific reason, such as use from another file. A name declared `static` is not visible to other files. So linking does not break even if another file has the same name, and the compiler warns about unused functions.

Do not declare a name in an inner scope that is already declared in an outer scope. `-Wshadow` warns about such shadowing.

Incorrect:

```c
int total_errors;
char error_text[128];

int count_errors(const int *const codes, size_t count)
{
    int total_errors = 0;

    for (size_t i = 0; i < count; ++i) {
        if (codes[i] != 0) {
            ++total_errors;
        }
    }

    snprintf(error_text, sizeof(error_text), "%d errors", total_errors);
    puts(error_text);
    return total_errors;
}
```

Correct:

```c
static int gs_total_errors;

static int count_errors(const int *const codes, size_t count)
{
    char error_text[128];
    int errors = 0;

    for (size_t i = 0; i < count; ++i) {
        if (codes[i] != 0) {
            ++errors;
        }
    }

    gs_total_errors += errors;
    snprintf(error_text, sizeof(error_text), "%d errors", errors);
    puts(error_text);
    return errors;
}
```

### 7.4. Pointers to objects whose lifetime has ended

If the object a pointer points to goes out of scope and its lifetime ends while the pointer variable remains, assign `NULL` to the pointer variable. Assign it even if the pointer will not be used for access again. Then, if the pointer is reused by mistake, the code handles `NULL` instead of the vanished object, so a `NULL` check catches it or, in most cases, it fails on the spot.

Incorrect:

```c
struct word_scanner {
    const char *cursor;
    size_t word_count;
};

void scan_words(struct word_scanner *const mut_scanner);

static int scan_next_line(struct word_scanner *const mut_scanner, FILE *const mut_input)
{
    char line[128];

    if (fgets(line, sizeof(line), mut_input) == NULL) {
        return -1;
    }

    mut_scanner->cursor = line;
    scan_words(mut_scanner);
    return 0;
}
```

```c
static void print_greeting(const char *const name, bool is_formal)
{
    const char *greeting = "hi";

    if (is_formal) {
        char formal[64];

        snprintf(formal, sizeof(formal), "good evening, %s", name);
        greeting = formal;
        puts(greeting);
    }

    printf("greeted %s\n", name);
}
```

Correct:

```c
struct word_scanner {
    const char *cursor;
    size_t word_count;
};

void scan_words(struct word_scanner *const mut_scanner);

static int scan_next_line(struct word_scanner *const mut_scanner, FILE *const mut_input)
{
    char line[128];

    if (fgets(line, sizeof(line), mut_input) == NULL) {
        return -1;
    }

    mut_scanner->cursor = line;
    scan_words(mut_scanner);
    mut_scanner->cursor = NULL;
    return 0;
}
```

```c
static void print_greeting(const char *const name, bool is_formal)
{
    const char *greeting = "hi";

    if (is_formal) {
        char formal[64];

        snprintf(formal, sizeof(formal), "good evening, %s", name);
        greeting = formal;
        puts(greeting);
        greeting = NULL;
    }

    printf("greeted %s\n", name);
}
```

### 7.5. Integer types

- Use `size_t` for sizes, counts, and indices.
- When the size of a type must be exact, use the exact-width integer types from `<stdint.h>`. However, code where return types and parameters are mostly basic types, as with the POSIX APIs on Linux, may follow the API's types instead of using `<stdint.h>`.
- Handle byte data as `unsigned char` or `uint8_t`.
- Cast explicitly when an operation mixes signed and unsigned values.

Incorrect:

```c
static unsigned int sum_bytes(const char *const data, int length)
{
    unsigned int sum = 0;

    for (int i = 0; i < length; ++i) {
        sum += data[i];
    }

    return sum;
}

static int is_inside(int offset, size_t length)
{
    return offset < length;
}
```

Correct:

```c
static uint32_t sum_bytes(const unsigned char *const data, size_t length)
{
    uint32_t sum = 0;

    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }

    return sum;
}

static int is_inside(int offset, size_t length)
{
    return offset >= 0 && (size_t)offset < length;
}
```

Correct (following the types of a POSIX API):

```c
static int read_exactly(int fd, unsigned char *const mut_buffer, size_t size)
{
    size_t done = 0;

    while (done < size) {
        ssize_t count = read(fd, mut_buffer + done, size - done);

        if (count <= 0) {
            return -1;
        }

        done += (size_t)count;
    }

    return 0;
}
```

### 7.6. Truth values

Truth values may be handled with `bool`, `true`, and `false` from `<stdbool.h>`, or with an integer type such as `int` and the values `1` and `0`. However, prefer `int` in code where return types and parameters are mostly basic types, as with the POSIX APIs on Linux. Other code may use `bool`.

When truth values are handled as integers, true is `1` and false is `0`. Every value other than `0` is interpreted as true, but prefer `1` in code that sets a true state directly.

Incorrect:

```c
#define FLAG_DIRTY 0x4U

static int is_dirty(unsigned int flags)
{
    return flags & FLAG_DIRTY;
}
```

Correct:

```c
#define FLAG_DIRTY 0x4U

static int is_dirty(unsigned int flags)
{
    return (flags & FLAG_DIRTY) != 0;
}
```

```c
#include <stdbool.h>

#define FLAG_DIRTY 0x4U

static bool gs_is_running = true;

static bool is_dirty(unsigned int flags)
{
    return (flags & FLAG_DIRTY) != 0;
}
```

```c
#define FLAG_DIRTY 0x4U

static int gs_is_running = 1;

static int is_dirty(unsigned int flags)
{
    return (flags & FLAG_DIRTY) != 0;
}
```

Projects that choose C89 have no `<stdbool.h>`, so they use an integer type. Do not define `bool`, `true`, and `false` yourself in that case, because these names became keywords in C23 and would collide.

Incorrect in a C89 project:

```c
typedef int bool;
#define true 1
#define false 0

static bool gs_is_running = true;
```

Correct in a C89 project:

```c
static int gs_is_running = 1;
```

### 7.7. Constants

- Define a group of related integer constants as an enum.
- Define a standalone constant with `#define`.
- Define a constant whose type matters as a `static const` object.

Incorrect:

```c
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

#define MILLISECONDS_PER_DAY 86400000

static long long convert_days_to_ms(int days)
{
    return days * MILLISECONDS_PER_DAY;
}
```

`MILLISECONDS_PER_DAY` is an `int`, so `days * MILLISECONDS_PER_DAY` is also computed as `int`. Therefore, when `days` is 25 or more, the result exceeds the range of `int` before it is converted to `long long`.

Correct:

```c
enum color {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
};

#define MAX_RETRIES 3

static const long long gs_milliseconds_per_day = 86400000;

static long long convert_days_to_ms(int days)
{
    return days * gs_milliseconds_per_day;
}
```

### 7.8. const

- `const` is not required on ordinary variables, including variables of basic types, or on value parameters.
- A pointer to a string literal is a `const char *`.
- Do not add `const` to a return type returned by value.

Incorrect:

```c
static char *gs_default_unit = "cm";

const double compute_ratio(double part, double whole);
```

Correct:

```c
static const char *gs_default_unit = "cm";

double compute_ratio(double part, double whole);
```

```c
static double convert_to_inches(double centimeters)
{
    double inches = centimeters / 2.54;

    return inches;
}
```

### 7.9. register and volatile

`register` may be used. Use `volatile` only in the following cases.

- Variables shared with a signal handler (`volatile sig_atomic_t`)
- Memory-mapped I/O
- Local variables read after `setjmp`
- Benchmark sinks that must not be optimized away
- Inline assembly that must execute even if its result is unused, or must execute again on every call (`__asm__ volatile`)

For synchronization between threads, use a mutex or atomic operations, not `volatile`.

Incorrect:

```c
static int gs_result;
static volatile int gs_has_result;

static void publish_result(int value)
{
    gs_result = value;
    gs_has_result = 1;
}

static int read_result(int *const mut_value)
{
    if (gs_has_result == 0) {
        return -1;
    }

    *mut_value = gs_result;
    return 0;
}
```

Correct:

```c
static pthread_mutex_t gs_result_lock = PTHREAD_MUTEX_INITIALIZER;
static int gs_result;
static int gs_has_result;

static void publish_result(int value)
{
    pthread_mutex_lock(&gs_result_lock);
    gs_result = value;
    gs_has_result = 1;
    pthread_mutex_unlock(&gs_result_lock);
}

static int read_result(int *const mut_value)
{
    int ret = -1;

    pthread_mutex_lock(&gs_result_lock);
    if (gs_has_result != 0) {
        *mut_value = gs_result;
        ret = 0;
    }
    pthread_mutex_unlock(&gs_result_lock);
    return ret;
}
```

Correct (`register`):

```c
static unsigned int count_set_bits(unsigned int value)
{
    register unsigned int count = 0;

    while (value != 0) {
        value &= value - 1;
        ++count;
    }

    return count;
}
```

Correct (a variable shared with a signal handler):

```c
static volatile sig_atomic_t gs_should_stop;

static void handle_sigterm(int signo)
{
    gs_should_stop = 1;
}
```

Correct (memory-mapped I/O):

```c
#define TIMER_COUNT (*(volatile const uint32_t *)0x40001024UL)

static uint32_t read_timer(void)
{
    return TIMER_COUNT;
}
```

Correct (a local variable read after `setjmp`):

```c
static jmp_buf gs_retry_point;

int parse_or_jump(const char *const text);

static int parse_with_retry(const char *const text)
{
    volatile int attempt_count = 0;

    if (setjmp(gs_retry_point) != 0) {
        if (attempt_count >= 3) {
            return -1;
        }
    }

    ++attempt_count;
    return parse_or_jump(text);
}
```

Correct (a benchmark sink):

```c
static volatile unsigned long gs_sink;

unsigned long hash_word(const char *const word);

static void run_hash_benchmark(const char *const *const words, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        gs_sink = hash_word(words[i]);
    }
}
```

Correct (inline assembly):

```c
static inline int read_hardware_random(uint64_t *const mut_value)
{
    unsigned char is_ok;

    __asm__ volatile("rdrand %0\n\t"
                     "setc %1"
                     : "=r"(*mut_value), "=qm"(is_ok));
    return (is_ok != 0) ? 0 : -1;
}
```

### 7.10. VLA

Do not use VLAs (variable length arrays). When a size is determined at run time, use dynamic allocation.

Incorrect:

```c
static int print_histogram(const unsigned char *const values, size_t count, size_t bucket_count)
{
    unsigned int buckets[bucket_count];

    memset(buckets, 0, sizeof(buckets));
    for (size_t i = 0; i < count; ++i) {
        ++buckets[values[i] % bucket_count];
    }

    for (size_t i = 0; i < bucket_count; ++i) {
        printf("%zu: %u\n", i, buckets[i]);
    }

    return 0;
}
```

Correct:

```c
static int print_histogram(const unsigned char *const values, size_t count, size_t bucket_count)
{
    unsigned int *buckets = calloc(bucket_count, sizeof(*buckets));
    if (buckets == NULL) {
        return -1;
    }

    for (size_t i = 0; i < count; ++i) {
        ++buckets[values[i] % bucket_count];
    }

    for (size_t i = 0; i < bucket_count; ++i) {
        printf("%zu: %u\n", i, buckets[i]);
    }

    free(buckets);
    buckets = NULL;
    return 0;
}
```

### 7.11. Initialization

- Initialize a variable in its declaration when its initial value is meaningful.
- To zero-fill an array in its declaration, initialize it with `{ 0 }`. The omitted remaining elements are initialized to zero, like objects with static storage duration (C99 §6.7.8 ¶21).
- Prefer designated initializers for structures.

Incorrect:

```c
static double compute_average(const double *const values, size_t count)
{
    double sum;

    sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        sum += values[i];
    }

    return sum / (double)count;
}
```

Correct:

```c
static double compute_average(const double *const values, size_t count)
{
    double sum = 0.0;

    for (size_t i = 0; i < count; ++i) {
        sum += values[i];
    }

    return sum / (double)count;
}
```

```c
static void print_letter_counts(const char *const text)
{
    unsigned int counts[26] = { 0 };

    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            ++counts[text[i] - 'a'];
        }
    }

    for (int letter = 0; letter < 26; ++letter) {
        printf("%c: %u\n", 'a' + letter, counts[letter]);
    }
}
```

```c
struct window_config {
    int width;
    int height;
    const char *title;
};

static void print_default_window(void)
{
    struct window_config config = {
        .width = 800,
        .height = 600,
        .title = "untitled"
    };

    printf("%s: %dx%d\n", config.title, config.width, config.height);
}
```

However, projects that choose C89 are an exception. C89 has no designated initializers, so declare a structure zero-filled with `{ 0 }` and then assign the required members.

Correct in a C89 project:

```c
struct window_config {
    int width;
    int height;
    const char *title;
};

static void print_default_window(void)
{
    struct window_config config = { 0 };

    config.width = 800;
    config.height = 600;
    config.title = "untitled";
    printf("%s: %dx%d\n", config.title, config.width, config.height);
}
```

Initialize file scope variables whose values are determined at run time early in the program where possible, that is, at the beginning of `main` or in an initialization function. This covers structures filled at run time, dynamically allocated memory, and values obtained from a library or the OS. The initializer of an object with static storage duration must be a constant expression (C99 §6.7.8 ¶4), so such values cannot be initialized at definition. Initialize constants and const tables whose values are fixed at compile time in their definitions.

Correct:

```c
#define SETTINGS_PATH "editor.conf"

struct settings;

struct settings *load_settings(const char *const path);
int run_editor(const struct settings *const settings);

static const unsigned int gs_autosave_intervals_s[] = { 30, 60, 300 };
static struct settings *gs_settings;

int main(void)
{
    gs_settings = load_settings(SETTINGS_PATH);
    if (gs_settings == NULL) {
        return EXIT_FAILURE;
    }

    return run_editor(gs_settings);
}
```

## 8. Expressions

### 8.1. Literal notation

- In hexadecimal literals, write the `0x` prefix in lowercase and the hexadecimal digits in uppercase.
- In binary literals, write the `0b` prefix in lowercase. Binary literals became standard in C23. In projects that choose C17 or earlier, they are a GNU extension, so add `__extension__`.
- Write all suffixes in uppercase (`U`, `L`, `UL`, `ULL`, `F`). A lowercase `l` is easily confused with the digit `1`, so write the other suffixes in uppercase too, for consistency.
- Write a decimal point in floating-point literals.

Incorrect:

```c
static const uint32_t gs_color_mask = 0Xff00ff00u;
static const unsigned int gs_mode_bits = 0B0110u;
static const long gs_max_offset = 65536l;
static const unsigned long gs_high_bit = 0x80000000ul;
static const unsigned long long gs_file_limit = 4000000000ull;
static const float gs_half = .5f;
static const double gs_timeout_s = 2;
```

Correct:

```c
static const uint32_t gs_color_mask = 0xFF00FF00U;
static const unsigned int gs_mode_bits = __extension__ 0b0110U;
static const long gs_max_offset = 65536L;
static const unsigned long gs_high_bit = 0x80000000UL;
static const unsigned long long gs_file_limit = 4000000000ULL;
static const float gs_half = 0.5F;
static const double gs_timeout_s = 2.0;
```

### 8.2. Increment and decrement operators

Use the prefix form (`++i`) of increment and decrement operators by default. Use the postfix form only when the previous value is required. Unless there is a specific reason, do not use increment or decrement operators inside array indices, function arguments, or operands of other operators. Move them into separate statements. Do not modify the same object more than once in one expression.

Incorrect:

```c
static size_t copy_positive(int *const mut_dst, const int *const src, size_t count)
{
    size_t copied = 0;

    for (size_t i = 0; i < count; i++) {
        if (src[i] > 0) {
            mut_dst[copied++] = src[i];
        }
    }

    return copied;
}
```

```c
static int read_pair_sum(const int *const values, size_t *const mut_index)
{
    size_t i = *mut_index;
    int sum = values[i++] + values[i++];

    *mut_index = i;
    return sum;
}
```

Correct:

```c
static size_t copy_positive(int *const mut_dst, const int *const src, size_t count)
{
    size_t copied = 0;

    for (size_t i = 0; i < count; ++i) {
        if (src[i] > 0) {
            mut_dst[copied] = src[i];
            ++copied;
        }
    }

    return copied;
}
```

```c
static int read_pair_sum(const int *const values, size_t *const mut_index)
{
    size_t i = *mut_index;
    int sum = values[i] + values[i + 1];

    *mut_index = i + 2;
    return sum;
}
```

Correct (when the previous value is required):

```c
static void print_reversed(const int *const values, size_t count)
{
    for (size_t i = count; i-- > 0;) {
        printf("%d\n", values[i]);
    }
}
```

### 8.3. sizeof

- Derive the size passed to functions such as allocators, `memset`, and `memcpy` from the target variable (`sizeof(*p)`, `sizeof(var)`).
- Get the number of elements of an array with the `ARRAY_SIZE()` macro. Use this macro only on objects of array type. Dynamically allocated memory and array parameters are pointers, so pass their element counts separately.

Incorrect:

```c
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct order {
    char customer[32];
    long amount_cents;
};

static const char *const gs_status_names[] = {
    "pending",
    "paid",
    "shipped",
};

static struct order *create_order(const char *const customer)
{
    struct order *order = malloc(sizeof(struct order));
    if (order == NULL) {
        return NULL;
    }

    memset(order, 0, sizeof(struct order));
    snprintf(order->customer, 32, "%s", customer);
    return order;
}

static void print_status_names(void)
{
    for (size_t i = 0; i < sizeof(gs_status_names) / sizeof(gs_status_names[0]); ++i) {
        puts(gs_status_names[i]);
    }
}

static void print_names(const char *const *const names)
{
    for (size_t i = 0; i < ARRAY_SIZE(names); ++i) {
        puts(names[i]);
    }
}
```

Correct:

```c
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct order {
    char customer[32];
    long amount_cents;
};

static const char *const gs_status_names[] = {
    "pending",
    "paid",
    "shipped",
};

static struct order *create_order(const char *const customer)
{
    struct order *order = malloc(sizeof(*order));
    if (order == NULL) {
        return NULL;
    }

    memset(order, 0, sizeof(*order));
    snprintf(order->customer, sizeof(order->customer), "%s", customer);
    return order;
}

static void print_status_names(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(gs_status_names); ++i) {
        puts(gs_status_names[i]);
    }
}

static void print_names(const char *const *const names, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        puts(names[i]);
    }
}
```

### 8.4. Casts

- Do not cast when converting `void *` to another object pointer type. The same applies to the return value of `malloc`.
- Use an explicit cast for conversions that may truncate a value and for operations that mix signedness.

Incorrect:

```c
static int32_t *create_samples(size_t count)
{
    int32_t *samples = (int32_t *)malloc(count * sizeof(*samples));

    return samples;
}

static int compute_percent(size_t done, size_t total)
{
    int percent = done * 100 / total;

    return percent;
}

static long compute_balance_change(unsigned long deposit_cents, long withdrawal_cents)
{
    return deposit_cents - withdrawal_cents;
}
```

Correct:

```c
static int32_t *create_samples(size_t count)
{
    int32_t *samples = malloc(count * sizeof(*samples));

    return samples;
}

static int compute_percent(size_t done, size_t total)
{
    int percent = (int)(done * 100 / total);

    return percent;
}

static long compute_balance_change(unsigned long deposit_cents, long withdrawal_cents)
{
    return (long)deposit_cents - withdrawal_cents;
}
```

### 8.5. Parentheses for precedence

- Parenthesize bitwise operators and shifts when they are mixed with other operators.
- Parenthesize when `&&` and `||` are mixed.
- Parentheses that the grammar does not require may be used for intuitive understanding, grouping, and readability. Avoid aggressively dropping parentheses just because the grammar does not require them.

The position of parentheses changes the meaning of an expression. A cast applies after postfix operators such as `[]`, `->`, and function calls, but before binary operators such as shifts. In the example below, `pixels` is a `const uint8_t *` that holds the red, green, blue, and alpha values of each pixel in sequence, one byte each. The following expressions all differ.

- `(uint32_t)pixels[2]` reads `pixels[2]` first. The subscript `2` means two bytes, so it converts only the blue value of the first pixel to `uint32_t`.
- `((const uint32_t *)pixels)[2]` converts the pointer first. The subscript `2` means two `uint32_t` values, that is, 8 bytes, so it reads all 4 bytes of the third pixel at once. Reading a byte buffer this way violates the strict aliasing rule. If the address is not suitably aligned for `uint32_t`, it also violates the alignment requirement.
- `(uint32_t)pixels[3] << 24` converts the alpha value to `uint32_t` and then shifts it. In contrast, `(uint32_t)(pixels[3] << 24)` first shifts the value promoted to `int`. So if the alpha value is `0x80` or more, the result exceeds the range of `int`, which is undefined behavior.

Incorrect:

```c
#define MODE_WRITE 0x2U

static int should_save(unsigned int mode, bool is_open, bool is_forced)
{
    return mode & MODE_WRITE != 0 && is_open || is_forced;
}

static unsigned int make_low_mask(unsigned int bits)
{
    return 1U << bits - 1;
}
```

```c
static uint32_t read_first_blue(const uint8_t *const pixels)
{
    return ((const uint32_t *)pixels)[2];
}

static uint32_t place_first_alpha(const uint8_t *const pixels)
{
    return (uint32_t)(pixels[3] << 24);
}
```

Discouraged:

```c
static long compute_offset(long header_size, long row, long row_stride, long column, long cell_size)
{
    return header_size + row * row_stride + column * cell_size;
}
```

Correct:

```c
#define MODE_WRITE 0x2U

static int should_save(unsigned int mode, bool is_open, bool is_forced)
{
    return (mode & MODE_WRITE) != 0 && (is_open || is_forced);
}

static unsigned int make_low_mask(unsigned int bits)
{
    return (1U << bits) - 1;
}
```

```c
static uint32_t read_first_blue(const uint8_t *const pixels)
{
    return (uint32_t)pixels[2];
}

static uint32_t place_first_alpha(const uint8_t *const pixels)
{
    return (uint32_t)pixels[3] << 24;
}
```

```c
static long compute_offset(long header_size, long row, long row_stride, long column, long cell_size)
{
    return header_size + (row * row_stride) + (column * cell_size);
}
```

### 8.6. Comparisons

- Do not write comparisons with the constant on the left (`NULL == p`). Compiler warnings catch accidental assignments.
- Do not compare floating-point values with `==`, and do not use them as loop variables.

Incorrect:

```c
static const double gs_tolerance = 1e-9;

void plot_point(double x);

static int check_result(const char *const name, double measured, double expected)
{
    if (NULL == name) {
        return -1;
    }

    if (measured == expected) {
        return 1;
    }

    return 0;
}

static void plot_unit_range(void)
{
    for (double x = 0.0; x != 1.0; x += 0.1) {
        plot_point(x);
    }
}
```

`0.1` cannot be represented exactly in binary, so adding it ten times does not give exactly `1.0`. Therefore, the loop in `plot_unit_range()` never ends.

Correct:

```c
static const double gs_tolerance = 1e-9;

void plot_point(double x);

static int check_result(const char *const name, double measured, double expected)
{
    if (name == NULL) {
        return -1;
    }

    if (fabs(measured - expected) < gs_tolerance) {
        return 1;
    }

    return 0;
}

static void plot_unit_range(void)
{
    for (int step = 0; step < 10; ++step) {
        plot_point(step * 0.1);
    }
}
```

### 8.7. Null pointers and truth tests

- Write a null pointer as `NULL`, not `0`. Only `NULL` shows that the value is a pointer.
- Compare pointers explicitly with `NULL`, and integers with `0`.
- Use only `bool` values directly as conditions.

Incorrect:

```c
static int print_title(const char *const title_nullable, int line_count, bool is_draft)
{
    const char *label = 0;

    if (!title_nullable) {
        return -1;
    }

    if (line_count && is_draft == true) {
        label = "draft";
    } else {
        label = "final";
    }

    printf("%s: %s\n", title_nullable, label);
    return 0;
}
```

Correct:

```c
static int print_title(const char *const title_nullable, int line_count, bool is_draft)
{
    const char *label = NULL;

    if (title_nullable == NULL) {
        return -1;
    }

    if (line_count != 0 && is_draft) {
        label = "draft";
    } else {
        label = "final";
    }

    printf("%s: %s\n", title_nullable, label);
    return 0;
}
```

### 8.8. Conditional operator

Use the conditional operator only to select one value. Wrap the condition in parentheses even though the grammar does not require them. Wrap even a condition that is a single comparison. This makes the condition read as one unit.

Avoid nesting conditional operators. If they are nested anyway, wrap the entire inner conditional expression in parentheses. If the nested expression does not fit on one line, pull the inner conditional operator out into an extra variable that holds its result.

Discouraged:

```c
static const char *describe_count(size_t count)
{
    return (count == 0) ? "no files" : ((count == 1) ? "one file" : "several files");
}
```

Incorrect:

```c
static const char *pick_noun(size_t count)
{
    return count == 1 ? "file" : "files";
}
```

```c
static const char *describe_count(size_t count)
{
    return count == 0 ? "no files" : count == 1 ? "one file" : "several files";
}
```

```c
void print_details(void);
void print_overview(void);

static void print_summary(bool is_verbose)
{
    (is_verbose) ? print_details() : print_overview();
}
```

```c
static const char *describe_copy(size_t file_count)
{
    const char *summary = (file_count == 0)
                              ? "no files were copied"
                              : ((file_count == 1) ? "one file was copied" : "several files were copied");

    return summary;
}
```

Correct:

```c
static const char *pick_noun(size_t count)
{
    return (count == 1) ? "file" : "files";
}
```

```c
static const char *describe_count(size_t count)
{
    if (count == 0) {
        return "no files";
    }
    if (count == 1) {
        return "one file";
    }
    return "several files";
}
```

```c
void print_details(void);
void print_overview(void);

static void print_summary(bool is_verbose)
{
    if (is_verbose) {
        print_details();
    } else {
        print_overview();
    }
}
```

```c
static const char *describe_copy(size_t file_count)
{
    const char *copied = (file_count == 1) ? "one file was copied" : "several files were copied";
    const char *summary = (file_count == 0) ? "no files were copied" : copied;

    return summary;
}
```

### 8.9. Assignment

- Do not assign inside a condition. Write a loop that reads and compares a value on every iteration as a `for` statement, and put the read in the init clause and the increment expression. Use assignment inside a condition only when this rewrite is impossible.
- Do not chain assignments.

Incorrect:

```c
static long count_bytes(const char *const path)
{
    FILE *file;
    long total = 0;
    int c;

    if ((file = fopen(path, "rb")) == NULL) {
        return -1;
    }

    while ((c = fgetc(file)) != EOF) {
        ++total;
    }

    fclose(file);
    file = NULL;
    return total;
}
```

```c
struct range {
    int first;
    int last;
};

static void clear_range(struct range *const mut_range)
{
    mut_range->first = mut_range->last = -1;
}
```

Correct:

```c
static long count_bytes(const char *const path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }

    long total = 0;

    for (int c = fgetc(file); c != EOF; c = fgetc(file)) {
        ++total;
    }

    fclose(file);
    file = NULL;
    return total;
}
```

```c
struct range {
    int first;
    int last;
};

static void clear_range(struct range *const mut_range)
{
    mut_range->first = -1;
    mut_range->last = -1;
}
```

### 8.10. Comma operator

Do not use the comma operator. However, in a `for` statement that handles two or more variables together, it may be used in the init clause and the increment expression. If such a `for` statement does not fit on one line, break it after a `;`, and start the next line in the column right after the opening parenthesis.

Incorrect:

```c
static void add_sample(long *const mut_total, int *const mut_count, int value)
{
    *mut_total += value, ++*mut_count;
}
```

```c
struct cursor {
    int row;
    int column;
};

static void reset_cursor(struct cursor *const mut_cursor)
{
    mut_cursor->row = 0, mut_cursor->column = 0;
}
```

Correct:

```c
static void add_sample(long *const mut_total, int *const mut_count, int value)
{
    *mut_total += value;
    ++*mut_count;
}
```

```c
struct cursor {
    int row;
    int column;
};

static void reset_cursor(struct cursor *const mut_cursor)
{
    mut_cursor->row = 0;
    mut_cursor->column = 0;
}
```

```c
static void reverse_text(char *const mut_text, size_t length)
{
    if (length == 0) {
        return;
    }

    for (size_t left = 0, right = length - 1; left < right; ++left, --right) {
        char saved = mut_text[left];

        mut_text[left] = mut_text[right];
        mut_text[right] = saved;
    }
}
```

```c
static void copy_to_free_slots(int *const mut_slots, size_t capacity, const int *const sources,
                               size_t source_count, size_t first_free_slot)
{
    for (size_t source = 0, slot = first_free_slot; source < source_count && slot < capacity;
         ++source, ++slot) {
        mut_slots[slot] = sources[source];
    }
}
```

## 9. Statements

### 9.1. switch

- Do not put `default` in a `switch` that handles every value of an enumeration. If a new enumeration constant is left out, `-Wswitch` reports it. With a `default`, this warning is turned off.
- Put `default` in every other `switch`.
- End every `case` with one of `break`, `return`, `goto`, or `continue`.
- Mark an intended fallthrough with a `/* fallthrough */` comment. GCC's `-Wimplicit-fallthrough` recognizes this comment.

Incorrect:

```c
enum shape {
    SHAPE_TRIANGLE,
    SHAPE_SQUARE,
    SHAPE_PENTAGON,
};

static int count_corners(enum shape shape)
{
    switch (shape) {
    case SHAPE_TRIANGLE:
        return 3;
    case SHAPE_SQUARE:
        return 4;
    case SHAPE_PENTAGON:
        return 5;
    default:
        return 0;
    }
}
```

```c
#define RIGHT_VIEW 0x1U
#define RIGHT_EDIT 0x2U
#define RIGHT_MANAGE 0x4U

static unsigned int grant_rights(char role)
{
    unsigned int rights = 0;

    switch (role) {
    case 'a':
        rights |= RIGHT_MANAGE;
    case 'e':
        rights |= RIGHT_EDIT;
    case 'v':
        rights |= RIGHT_VIEW;
        break;
    }

    return rights;
}
```

Correct:

```c
enum shape {
    SHAPE_TRIANGLE,
    SHAPE_SQUARE,
    SHAPE_PENTAGON,
};

static int count_corners(enum shape shape)
{
    switch (shape) {
    case SHAPE_TRIANGLE:
        return 3;
    case SHAPE_SQUARE:
        return 4;
    case SHAPE_PENTAGON:
        return 5;
    }

    return 0;
}
```

```c
#define RIGHT_VIEW 0x1U
#define RIGHT_EDIT 0x2U
#define RIGHT_MANAGE 0x4U

static unsigned int grant_rights(char role)
{
    unsigned int rights = 0;

    switch (role) {
    case 'a':
        rights |= RIGHT_MANAGE;
        /* fallthrough */
    case 'e':
        rights |= RIGHT_EDIT;
        /* fallthrough */
    case 'v':
        rights |= RIGHT_VIEW;
        break;
    default:
        break;
    }

    return rights;
}
```

### 9.2. Empty loop bodies

Avoid loops with empty bodies where possible. Move assignments, increments, and similar operations, as well as function calls, out of the condition and into the body or into the init clause and increment expression of a `for` statement. However, if the work must be in the condition, leave it there.

Incorrect:

```c
static size_t measure_text(const char *const text)
{
    const char *cursor = text;

    while (*cursor++ != '\0') {
    }

    return (size_t)(cursor - text - 1);
}
```

```c
static void echo_line(FILE *const mut_input, FILE *const mut_output)
{
    int c;

    while ((c = fgetc(mut_input)) != EOF && fputc(c, mut_output) != '\n') {
    }
}
```

Correct:

```c
static size_t measure_text(const char *const text)
{
    size_t length = 0;

    while (text[length] != '\0') {
        ++length;
    }

    return length;
}
```

```c
static void echo_line(FILE *const mut_input, FILE *const mut_output)
{
    for (int c = fgetc(mut_input); c != EOF; c = fgetc(mut_input)) {
        fputc(c, mut_output);
        if (c == '\n') {
            break;
        }
    }
}
```

Correct (when the work must stay in the condition):

```c
#define ADC_STATUS (*(volatile const uint32_t *)0x40012000UL)
#define ADC_CONVERSION_DONE 0x2U

static void wait_for_conversion(void)
{
    while ((ADC_STATUS & ADC_CONVERSION_DONE) == 0) {
    }
}
```

### 9.3. goto

Use goto in the following cases.

- When leaving control statements or loops nested two or more levels deep in one step
- When a staged initialization stops partway. If initialization runs from stage 1 to stage N in order and stops at some stage, undo the initializations already done, from that stage back to stage 1. If there are resources to clean up, put labels for the cleanup code at the end of the function and jump to them with `goto`. Place the labels in the reverse order of resource acquisition.

A goto that jumps backward may also be used. However, prefer to write the logic so that such a structure does not arise in the first place.

Discouraged:

```c
int read_sensor(int *const mut_value);
void wait_briefly(void);

static int read_sensor_with_retry(int *const mut_value)
{
    int attempts = 0;

retry:
    if (read_sensor(mut_value) != 0) {
        ++attempts;
        if (attempts < 3) {
            wait_briefly();
            goto retry;
        }
        return -1;
    }

    return 0;
}
```

Correct:

```c
struct grid {
    pthread_mutex_t lock;
    int cells[8][8];
};

static int find_cell(struct grid *const mut_grid, int target, int *const mut_row,
                     int *const mut_column)
{
    int result = -1;

    pthread_mutex_lock(&mut_grid->lock);
    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            if (mut_grid->cells[row][column] == target) {
                *mut_row = row;
                *mut_column = column;
                result = 0;
                goto out_unlock;
            }
        }
    }

out_unlock:
    pthread_mutex_unlock(&mut_grid->lock);
    return result;
}
```

```c
#define BMP_HEADER_SIZE 54

struct image {
    FILE *file;
    unsigned char *header;
    unsigned char *pixels;
};

static int open_image(struct image *const mut_image, const char *const path, size_t pixel_bytes)
{
    mut_image->file = fopen(path, "rb");
    if (mut_image->file == NULL) {
        return -1;
    }

    mut_image->header = malloc(BMP_HEADER_SIZE);
    if (mut_image->header == NULL) {
        goto handle_header_alloc;
    }

    mut_image->pixels = malloc(pixel_bytes);
    if (mut_image->pixels == NULL) {
        goto handle_pixels_alloc;
    }

    return 0;

handle_pixels_alloc:
    free(mut_image->header);
    mut_image->header = NULL;
handle_header_alloc:
    fclose(mut_image->file);
    mut_image->file = NULL;
    return -1;
}
```

```c
int read_sensor(int *const mut_value);
void wait_briefly(void);

static int read_sensor_with_retry(int *const mut_value)
{
    for (int attempts = 0; attempts < 3; ++attempts) {
        if (read_sensor(mut_value) == 0) {
            return 0;
        }
        wait_briefly();
    }

    return -1;
}
```

## 10. Functions

### 10.1. Parameter lists

Declare a function without parameters with `(void)`, not with empty parentheses.

Name every parameter in function declarations. Parameter names in the declaration and the definition must match.

Incorrect:

```c
static int gs_volume;

int read_volume();
int set_volume(int);

int read_volume()
{
    return gs_volume;
}

int set_volume(int new_volume)
{
    if (new_volume < 0 || new_volume > 100) {
        return -1;
    }

    gs_volume = new_volume;
    return 0;
}
```

Correct:

```c
static int gs_volume;

int read_volume(void);
int set_volume(int volume);

int read_volume(void)
{
    return gs_volume;
}

int set_volume(int volume)
{
    if (volume < 0 || volume > 100) {
        return -1;
    }

    gs_volume = volume;
    return 0;
}
```

### 10.2. Parameter order

Put output parameters that receive results first. This is the same order as in `memcpy`.

Incorrect:

```c
struct date {
    int year;
    int month;
    int day;
};

static int format_date(const struct date *const date, char *const mut_buffer, size_t buffer_size)
{
    return snprintf(mut_buffer, buffer_size, "%04d-%02d-%02d", date->year, date->month, date->day);
}
```

Correct:

```c
struct date {
    int year;
    int month;
    int day;
};

static int format_date(char *const mut_buffer, size_t buffer_size, const struct date *const date)
{
    return snprintf(mut_buffer, buffer_size, "%04d-%02d-%02d", date->year, date->month, date->day);
}
```

### 10.3. Pointer parameters

Pointer parameters follow these rules.

- Do not modify a pointer parameter itself inside the function. Therefore, declare every pointer parameter `*const`. If an address other than the one passed in is needed, obtain it by means other than the parameter, such as subscripting or a local pointer variable.
- If the function does not modify the target, make the target `const` too (`const T *const name`).
- If the function modifies the target, do not make the target `const`, and start the name with `mut_` (`T *const mut_name`).
- End the name of a parameter that may receive `NULL` as an argument with `_nullable` (`const T *const name_nullable`, `T *const mut_name_nullable`). Do not add it to parameters that must not receive `NULL`.
- When the target is promised not to overlap the target of any other pointer parameter, `restrict` may be added (`const T *const restrict name`). Projects that choose C89 have no `restrict`.
- Write the parameters of `main` exactly in the form the standard specifies.

Use the same form in the declaration and the definition of a function. Then the declaration alone shows whether the function modifies the targets passed as arguments and whether it accepts `NULL`.

Incorrect:

```c
static size_t count_digits(const char *text)
{
    size_t count = 0;

    for (; *text != '\0'; ++text) {
        if (isdigit((unsigned char)*text) != 0) {
            ++count;
        }
    }

    return count;
}
```

```c
static void convert_to_upper(char *const text)
{
    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] = (char)(text[i] - 'a' + 'A');
        }
    }
}

size_t count_vowels(char *const text);
```

```c
static void print_label(const char *const label)
{
    if (label == NULL) {
        puts("(none)");
        return;
    }

    puts(label);
}
```

Correct:

```c
size_t count_nonzero(const int *const values, size_t count);
void clear_values(int *const mut_values, size_t count);
void clear_values_if_present(int *const mut_values_nullable, size_t count);
void copy_pixels(uint8_t *const restrict mut_dst, const uint8_t *const restrict src, size_t count);
```

```c
static size_t count_digits(const char *const text)
{
    size_t count = 0;

    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (isdigit((unsigned char)text[i]) != 0) {
            ++count;
        }
    }

    return count;
}
```

```c
static size_t count_digits(const char *const text)
{
    size_t count = 0;

    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        if (isdigit((unsigned char)*cursor) != 0) {
            ++count;
        }
    }

    return count;
}
```

```c
static void convert_to_upper(char *const mut_text)
{
    for (size_t i = 0; mut_text[i] != '\0'; ++i) {
        if (mut_text[i] >= 'a' && mut_text[i] <= 'z') {
            mut_text[i] = (char)(mut_text[i] - 'a' + 'A');
        }
    }
}

size_t count_vowels(const char *const text);
```

```c
static void print_label(const char *const label_nullable)
{
    if (label_nullable == NULL) {
        puts("(none)");
        return;
    }

    puts(label_nullable);
}
```

### 10.4. Return value conventions

- For a function that does a task and reports only whether it succeeded, prefer returning `0` on success and a negative value on failure. However, this convention may vary with the design of the higher-level API that calls the function, so if the higher-level API defines a convention, follow it.
- A function that cannot fail returns `void`.
- A function that returns a truth value returns `int` or `bool`.
- A function that computes and returns a value signals failure with an out-of-range value (`NULL`, `-1`).

Returning `int` lets the value distinguish the reasons for false. For example, if all negative values map to false, each negative value can represent a different reason for false. Also, many values in Linux APIs are passed as `int`. When following that convention, `int` is often the natural choice.

Incorrect:

```c
struct profile;
struct statistics;

bool load_profile(const char *const path);
int reset_statistics(struct statistics *const mut_statistics);
bool is_profile_loaded(void);
int is_cache_warm(void);
struct profile *find_profile(const char *const name);
long parse_count(const char *const text);
```

Correct:

```c
struct profile;
struct statistics;

int load_profile(const char *const path);
void reset_statistics(struct statistics *const mut_statistics);
bool is_profile_loaded(void);
int is_cache_warm(void);
struct profile *find_profile(const char *const name);
long parse_count(const char *const text);
```

Correct (following a convention set by a higher-level API):

```c
struct task {
    char name[32];
    int priority;
};

static int compare_priority(const void *const left, const void *const right)
{
    const struct task *first = left;
    const struct task *second = right;

    if (first->priority < second->priority) {
        return -1;
    }
    if (first->priority > second->priority) {
        return 1;
    }
    return 0;
}

static void sort_tasks(struct task *const mut_tasks, size_t count)
{
    qsort(mut_tasks, count, sizeof(*mut_tasks), compare_priority);
}
```

### 10.5. Function size

There is no upper limit on function length or on the number of local variables. One function may take on several tasks.

### 10.6. inline

Consider `inline` for code fragments with a short execution time. The candidates are fragments that are used repeatedly in several places but are awkward to name as standalone functions, and that fall short as macros in syntax, code navigation and reading, and IDE code analysis. Do not apply it to a function with a long execution time, even if it has few lines. The compiler makes the final decision on inlining.

Use `static inline` as the default form, but `static` is not mandatory. Under C99 §6.7.4 ¶6, the definition of a function with `inline` but without `static` is an inline definition and does not provide an external definition. Therefore, for such a function, put an `extern inline` declaration in one `.c` file to create the external definition. Otherwise, linking fails for calls that the compiler did not inline. GCC and Clang report an `undefined reference` error with `-std=c99 -O0`. Conversely, in GNU89 mode every file gets an external definition, which causes a `multiple definition` error.

Discouraged (`util/listing.h`):

```c
#ifndef UTIL_LISTING_H
#define UTIL_LISTING_H

#include <stddef.h>
#include <stdio.h>

static inline void print_table(const int *const values, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        printf("%zu: %d\n", i, values[i]);
    }
}

#endif /* UTIL_LISTING_H */
```

Correct (`util/listing.h`):

```c
#ifndef UTIL_LISTING_H
#define UTIL_LISTING_H

#include <stddef.h>

void print_table(const int *const values, size_t count);

#endif /* UTIL_LISTING_H */
```

Correct (`util/color.h`):

```c
#ifndef UTIL_COLOR_H
#define UTIL_COLOR_H

#include <stdint.h>

static inline uint8_t blend_channel(uint8_t back, uint8_t front, uint8_t alpha)
{
    return (uint8_t)((front * alpha + back * (255 - alpha)) / 255);
}

#endif /* UTIL_COLOR_H */
```

Correct (`util/clamp.h`):

```c
#ifndef UTIL_CLAMP_H
#define UTIL_CLAMP_H

inline int clamp_percent(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 100) {
        return 100;
    }
    return value;
}

#endif /* UTIL_CLAMP_H */
```

Correct (`util/clamp.c`):

```c
#include "util/clamp.h"

extern inline int clamp_percent(int value);
```

However, projects that choose C89 are an exception. C89 has no `inline`, so do not define functions in headers. Define functions in `.c` files, and put only declarations in headers.

Correct in a C89 project (`util/color.h`):

```c
#ifndef UTIL_COLOR_H
#define UTIL_COLOR_H

unsigned char blend_channel(unsigned char back, unsigned char front, unsigned char alpha);

#endif /* UTIL_COLOR_H */
```

### 10.7. Function placement

- Place a static function called within the file above the functions that call it. Use forward declarations only in special cases such as mutual recursion.
- `main` is the last function in the file.
- Write a declaration in the order: storage-class specifier, `inline`, return type, name.

Incorrect:

```c
#include <stdio.h>

static void print_banner(const char *const title);

int main(void)
{
    print_banner("inventory");
    return 0;
}

void static print_banner(const char *const title)
{
    printf("== %s ==\n", title);
}
```

Correct:

```c
#include <stdio.h>

static void print_banner(const char *const title)
{
    printf("== %s ==\n", title);
}

int main(void)
{
    print_banner("inventory");
    return 0;
}
```

Correct (functions that call each other):

```c
static double parse_sum(const char **const mut_cursor);

static double parse_factor(const char **const mut_cursor)
{
    if (**mut_cursor == '(') {
        ++*mut_cursor;
        double value = parse_sum(mut_cursor);
        ++*mut_cursor;
        return value;
    }

    char *end = NULL;
    double value = strtod(*mut_cursor, &end);

    *mut_cursor = end;
    return value;
}

static double parse_sum(const char **const mut_cursor)
{
    double sum = parse_factor(mut_cursor);

    while (**mut_cursor == '+') {
        ++*mut_cursor;
        sum += parse_factor(mut_cursor);
    }

    return sum;
}
```

### 10.8. main

- Write `main` as `int main(void)` or `int main(int argc, char *argv[])`.
- End with `return 0;` on success and `return EXIT_FAILURE;` on failure.

Incorrect:

```c
void main()
{
    puts("ready");
    exit(0);
}
```

Correct:

```c
int main(void)
{
    puts("ready");
    return 0;
}
```

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <city>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("forecast for %s\n", argv[1]);
    return 0;
}
```

## 11. Preprocessor and macros

### 11.1. Conditional compilation

- The build system decides which files to compile. Put `#if` in source code only where the implementation actually differs.
- Branch on features rather than on operating system names. Identify the architecture with macros that the compiler defines.
- Use `#error` only to mean "this cannot be correct in this environment". Handle an unknown platform with a default value such as `unknown`.
- After the `#endif` of a conditional block longer than 20 lines, write the condition in a comment.

Incorrect (`src/sleep_win32.c`):

```c
#if defined(_WIN32)

# include <windows.h>

static void sleep_ms(unsigned int milliseconds)
{
    Sleep(milliseconds);
}

#endif
```

```c
#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>

static int open_for_reading(const char *const path)
{
#if defined(__linux__)
    return open(path, O_RDONLY | O_CLOEXEC);
#else
    return open(path, O_RDONLY);
#endif
}
```

```c
#if defined(__x86_64__)
# define TARGET_ARCH_NAME "x86_64"
#else
# error "unsupported architecture"
#endif
```

```c
int clamp_percent(int value);
size_t count_words(const char *const text);

#if defined(ENABLE_SELF_TEST)
static int run_self_test(void)
{
    int failures = 0;

    if (clamp_percent(-5) != 0) {
        ++failures;
    }
    if (clamp_percent(50) != 50) {
        ++failures;
    }
    if (clamp_percent(150) != 100) {
        ++failures;
    }
    if (count_words("one two  three") != 3) {
        ++failures;
    }
    if (count_words("") != 0) {
        ++failures;
    }

    printf("self test: %d failures\n", failures);
    return (failures == 0) ? 0 : -1;
}
#endif
```

Correct (`CMakeLists.txt`):

```cmake
if(WIN32)
    target_sources(app PRIVATE src/sleep_win32.c)
else()
    target_sources(app PRIVATE src/sleep_posix.c)
endif()
```

```c
#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>

static int open_for_reading(const char *const path)
{
#if defined(O_CLOEXEC)
    return open(path, O_RDONLY | O_CLOEXEC);
#else
    return open(path, O_RDONLY);
#endif
}
```

```c
#if defined(__x86_64__) || defined(_M_X64)
# define TARGET_ARCH_NAME "x86_64"
#elif defined(__aarch64__) || defined(_M_ARM64)
# define TARGET_ARCH_NAME "aarch64"
#else
# define TARGET_ARCH_NAME "unknown"
#endif
```

```c
#include <limits.h>

#if CHAR_BIT != 8
# error "this code reads files as 8-bit bytes"
#endif
```

```c
int clamp_percent(int value);
size_t count_words(const char *const text);

#if defined(ENABLE_SELF_TEST)
static int run_self_test(void)
{
    int failures = 0;

    if (clamp_percent(-5) != 0) {
        ++failures;
    }
    if (clamp_percent(50) != 50) {
        ++failures;
    }
    if (clamp_percent(150) != 100) {
        ++failures;
    }
    if (count_words("one two  three") != 3) {
        ++failures;
    }
    if (count_words("") != 0) {
        ++failures;
    }

    printf("self test: %d failures\n", failures);
    return (failures == 0) ? 0 : -1;
}
#endif /* defined(ENABLE_SELF_TEST) */
```

### 11.2. Alternatives to macros

For code that behaves like a function, consider a `static inline` function rather than a macro. Use a macro when the code must be type-independent or needs the preprocessor, as with `#`, `##`, and `__FILE__`.

Incorrect:

```c
#define PERCENT_OF(part, whole) ((part) * 100 / (whole))
```

Correct:

```c
static inline int compute_percent_of(int part, int whole)
{
    return part * 100 / whole;
}
```

```c
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define PRINT_INT(expr) printf("%s = %d\n", #expr, (expr))
#define GLUE(prefix, name) prefix##name
#define TRACE(message) fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, (message))
```

### 11.3. Parentheses

If a macro body is an expression, wrap the entire body in parentheses. Wrap every use of an argument in parentheses. Do not use parentheses for a body that is a single literal.

Incorrect:

```c
#define MAX_SEATS (120)
#define SEATS_PER_ROW MAX_SEATS / 10
#define AVERAGE(a, b) a + b / 2
```

Correct:

```c
#define MAX_SEATS 120
#define SEATS_PER_ROW (MAX_SEATS / 10)
#define AVERAGE(a, b) (((a) + (b)) / 2)
```

### 11.4. Multi-statement macros

Wrap a macro that expands to several statements in `do { ... } while (0)`. Only then does it behave as a single statement in an `if` body too.

Incorrect:

```c
static int gs_failure_count;

#define REPORT_FAILURE(what)                \
    fprintf(stderr, "%s failed\n", (what)); \
    ++gs_failure_count
```

Correct:

```c
static int gs_failure_count;

#define REPORT_FAILURE(what)                    \
    do {                                        \
        fprintf(stderr, "%s failed\n", (what)); \
        ++gs_failure_count;                     \
    } while (0)
```

### 11.5. Macros to avoid

Do not write the following macros.

- Macros that change control flow, for example by returning from the calling function
- Macros that assume a local variable with a specific name exists
- Macros used on the left-hand side of an assignment
- Macros that change the syntax

Incorrect:

```c
int save_file(const char *const path);

#define RETURN_IF_FAILED(expr) \
    do {                       \
        if ((expr) < 0) {      \
            return -1;         \
        }                      \
    } while (0)

static int save_both(const char *const first_path, const char *const second_path)
{
    RETURN_IF_FAILED(save_file(first_path));
    RETURN_IF_FAILED(save_file(second_path));
    return 0;
}
```

```c
#define PRINT_PROGRESS() printf("%d/%d done\n", done, total)

static void process_jobs(int total)
{
    for (int done = 1; done <= total; ++done) {
        PRINT_PROGRESS();
    }
}
```

```c
struct scoreboard {
    int scores[16];
    size_t count;
};

#define LAST_SCORE(board) ((board)->scores[(board)->count - 1])

static void reset_last_score(struct scoreboard *const mut_board)
{
    if (mut_board->count > 0) {
        LAST_SCORE(mut_board) = 0;
    }
}
```

```c
#define BEGIN {
#define END }

static int count_negatives(const int *const values, size_t count)
BEGIN
    int negatives = 0;

    for (size_t i = 0; i < count; ++i) BEGIN
        if (values[i] < 0) BEGIN
            ++negatives;
        END
    END

    return negatives;
END
```

Correct:

```c
int save_file(const char *const path);

static int save_both(const char *const first_path, const char *const second_path)
{
    if (save_file(first_path) < 0) {
        return -1;
    }
    if (save_file(second_path) < 0) {
        return -1;
    }
    return 0;
}
```

```c
static void print_progress(int done, int total)
{
    printf("%d/%d done\n", done, total);
}

static void process_jobs(int total)
{
    for (int done = 1; done <= total; ++done) {
        print_progress(done, total);
    }
}
```

```c
struct scoreboard {
    int scores[16];
    size_t count;
};

static void reset_last_score(struct scoreboard *const mut_board)
{
    if (mut_board->count > 0) {
        mut_board->scores[mut_board->count - 1] = 0;
    }
}
```

```c
static int count_negatives(const int *const values, size_t count)
{
    int negatives = 0;

    for (size_t i = 0; i < count; ++i) {
        if (values[i] < 0) {
            ++negatives;
        }
    }

    return negatives;
}
```

### 11.6. Local names in macros

If a macro must declare a local variable, append one underscore (`_`) to its name, as in `a_`. This keeps it from colliding with names on the calling side. Do not prefix the name with two underscores (`__`), as in `__a`, because names that start with two underscores are reserved identifiers.

Incorrect:

```c
#define MIN_OF(a, b)             \
    __extension__({              \
        __typeof__(a) __a = (a); \
        __typeof__(b) __b = (b); \
        (__a < __b) ? __a : __b; \
    })
```

Correct:

```c
#define MIN_OF(a, b)            \
    __extension__({             \
        __typeof__(a) a_ = (a); \
        __typeof__(b) b_ = (b); \
        (a_ < b_) ? a_ : b_;    \
    })
```

## 12. Comments

### 12.1. Amount and content of comments

Write comments only when necessary. First fix names and structure so that the code explains itself. In comments, state what is done and why, and leave how it is done to the code.

Where reading the code alone is not enough to understand it, write comments on the following.

- Background knowledge needed to understand the code
- The reason the operation must be done
- The reason for an implementation that looks pointless at first glance

Write long comments when the code serves as a public API, library, or interface, and when it is educational code. Then describe the internal behavior, arguments, return values, per-case scenarios, and so on in detail.

Incorrect:

```c
/* 86400 */
#define SECONDS_PER_DAY 86400
```

```c
static int publish_file(const char *const temp_path, const char *const final_path)
{
    /* rename the temporary file */
    if (rename(temp_path, final_path) != 0) {
        return -1;
    }

    return 0;
}
```

```c
static int is_valid_index(int index, unsigned int count)
{
    /* cast index */
    return (unsigned int)index < count;
}
```

Correct:

```c
/* POSIX time counts every day as exactly 86400 seconds, so leap seconds are not counted */
#define SECONDS_PER_DAY 86400
```

```c
static int publish_file(const char *const temp_path, const char *const final_path)
{
    /* readers see either the old file or the complete new one, never a half-written file */
    if (rename(temp_path, final_path) != 0) {
        return -1;
    }

    return 0;
}
```

```c
static int is_valid_index(int index, unsigned int count)
{
    /* a negative index wraps to a large unsigned value, so one comparison rejects it too */
    return (unsigned int)index < count;
}
```

Correct (educational code):

```c
/*
 * Binary search keeps the answer inside [low, high). Each step compares the
 * middle element with the key and drops the half that cannot contain it, so a
 * sorted array of n elements needs about log2(n) steps instead of n.
 */
static long find_sorted(const int *const values, size_t count, int key)
{
    size_t low = 0;
    size_t high = count;

    while (low < high) {
        size_t middle = low + (high - low) / 2;

        if (values[middle] < key) {
            low = middle + 1;
        } else if (values[middle] > key) {
            high = middle;
        } else {
            return (long)middle;
        }
    }

    return -1;
}
```

### 12.2. Comment format

- Use only `/* */`. Do not use `//`.
- Write a multi-line comment as a `/*` line, lines that start with ` * `, and a `*/` line.
- Write a trailing comment one space after the code. Do not align trailing comments in columns.

Incorrect:

```c
struct date;

// returns the number of days from start to end
long count_days_between(const struct date *const start, const struct date *const end);
```

```c
struct date;

/* Returns the number of days from start to end. Both dates use the
   proleptic Gregorian calendar, and end may come before start. */
long count_days_between(const struct date *const start, const struct date *const end);
```

```c
static int gs_retry_limit = 3;   /* attempts before giving up */
static int gs_delay_ms = 250;    /* wait between attempts */
```

Correct:

```c
struct date;

/* returns the number of days from start to end */
long count_days_between(const struct date *const start, const struct date *const end);
```

```c
struct date;

/*
 * Returns the number of days from start to end. Both dates use the
 * proleptic Gregorian calendar, and end may come before start.
 */
long count_days_between(const struct date *const start, const struct date *const end);
```

```c
static int gs_retry_limit = 3; /* attempts before giving up */
static int gs_delay_ms = 250; /* wait between attempts */
```

### 12.3. Function comments and documentation comments

Add function comments to functions whose use is not clear from the name and signature alone, and to functions that serve as a public API, library, or interface. If needed, a documentation comment format such as Doxygen or kernel-doc may be used.

Incorrect:

```c
struct rect;

/**
 * @brief Gets the width.
 * @return The width.
 */
static int get_width(const struct rect *const rect);
```

Correct:

```c
struct rect;

static int get_width(const struct rect *const rect);
```

```c
/* returns -1 when year is outside 1..9999 */
static int count_leap_days(int year);
```

```c
/*
 * Parses a duration such as "90s", "15m" or "2h30m" and stores it in seconds.
 *
 * text must not be NULL. The units are h, m and s, each used at most once and
 * in that order. Whitespace is not accepted.
 *
 * Returns 0 on success. Returns -1 with errno set to EINVAL when text is not a
 * valid duration, or to ERANGE when the result does not fit in a long.
 */
int parse_duration(const char *const text, long *const mut_seconds);
```

```c
/**
 * @brief Parses a duration such as "90s", "15m" or "2h30m".
 * @param text Duration text; must not be NULL.
 * @param mut_seconds Receives the duration in seconds.
 * @return 0 on success, or -1 with errno set to EINVAL or ERANGE.
 */
int parse_duration(const char *const text, long *const mut_seconds);
```

## 13. Error handling and resources

### 13.1. Checking return values

Check the return value of a call whose failure changes the outcome. When a call's result is discarded, make the call as it is, without `(void)`. GCC issues the `warn_unused_result` warning even with a `(void)` cast. So `(void)` is not even a portable means of suppression.

Incorrect:

```c
static int save_notes(const char *const path, const char *const text)
{
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return -1;
    }

    fputs(text, file);
    fclose(file);
    file = NULL;
    return 0;
}

static long measure_file(const char *const path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }

    long size = -1;

    if (fseek(file, 0, SEEK_END) == 0) {
        size = ftell(file);
    }

    (void)fclose(file);
    file = NULL;
    return size;
}
```

Correct:

```c
static int save_notes(const char *const path, const char *const text)
{
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return -1;
    }

    int write_result = fputs(text, file);
    int close_result = fclose(file);

    file = NULL;
    if (write_result == EOF || close_result != 0) {
        return -1;
    }

    return 0;
}

static long measure_file(const char *const path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }

    long size = -1;

    if (fseek(file, 0, SEEK_END) == 0) {
        size = ftell(file);
    }

    fclose(file);
    file = NULL;
    return size;
}
```

### 13.2. Unused diagnostics

Do not hide unused diagnostics in source code. Do not use `(void)x;`, the unused attribute, diagnostic pragmas, or edits that delete names to avoid a diagnostic. Whether to ignore these diagnostics or turn them off with compiler options (`-Wno-unused-parameter` and so on) is a matter of discretion.

Incorrect:

```c
static int print_entry(const char *const path, void *const mut_context)
{
    (void)mut_context;
    puts(path);
    return 0;
}

static int count_entry(const char *const path, __attribute__((unused)) void *const mut_context)
{
    return (path[0] == '.') ? 0 : 1;
}
```

```c
#pragma GCC diagnostic ignored "-Wunused-parameter"

static int print_entry(const char *const path, void *const mut_context)
{
    puts(path);
    return 0;
}
```

Incorrect (C23 and later):

```c
static int print_entry(const char *const path, void *const)
{
    puts(path);
    return 0;
}
```

Correct:

```c
static int print_entry(const char *const path, void *const mut_context)
{
    puts(path);
    return 0;
}

static int count_entry(const char *const path, void *const mut_context)
{
    return (path[0] == '.') ? 0 : 1;
}
```

### 13.3. errno

Read `errno` immediately after the failed call. If another library call comes in between, save it to a local variable first.

Incorrect:

```c
static int replace_file(const char *const temp_path, const char *const final_path)
{
    if (rename(temp_path, final_path) != 0) {
        remove(temp_path);
        perror("rename");
        return -1;
    }

    return 0;
}
```

Correct:

```c
static int replace_file(const char *const temp_path, const char *const final_path)
{
    if (rename(temp_path, final_path) != 0) {
        perror("rename");
        remove(temp_path);
        return -1;
    }

    return 0;
}
```

```c
static int replace_file(const char *const temp_path, const char *const final_path)
{
    if (rename(temp_path, final_path) != 0) {
        int saved_errno = errno;

        remove(temp_path);
        /* remove() may overwrite errno, so restore the error of rename() for the caller */
        errno = saved_errno;
        return -1;
    }

    return 0;
}
```

### 13.4. Resource cleanup

- If there are no resources to clean up, return immediately at the point of failure.
- If there are resources to clean up, put cleanup labels at the end of the function and jump to them with `goto`. Place the labels in the reverse order of resource acquisition.
- Give labels descriptive names (`handle_buffer_alloc`, `out_close_file`). Do not number them, as in `err1` and `err2`.

Incorrect:

```c
#define LINE_LEN 256

struct palette;

int parse_palette(struct palette *const mut_palette, const char *const line);

static int load_palette(struct palette *const mut_palette, const char *const path)
{
    FILE *file = fopen(path, "r");
    char *line = malloc(LINE_LEN);

    if (file == NULL || line == NULL) {
        goto err1;
    }

    if (fgets(line, LINE_LEN, file) == NULL) {
        goto err1;
    }

    return parse_palette(mut_palette, line);

err1:
    free(line);
    line = NULL;
    fclose(file);
    file = NULL;
    return -1;
}
```

Correct:

```c
#define LINE_LEN 256

struct palette;

int parse_palette(struct palette *const mut_palette, const char *const line);

static int load_palette(struct palette *const mut_palette, const char *const path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return -1;
    }

    int result = -1;

    char *line = malloc(LINE_LEN);
    if (line == NULL) {
        goto out_close_file;
    }

    if (fgets(line, LINE_LEN, file) != NULL && parse_palette(mut_palette, line) == 0) {
        result = 0;
    }

    free(line);
    line = NULL;
out_close_file:
    fclose(file);
    file = NULL;
    return result;
}
```

### 13.5. Dynamic memory

- Do not iterate with the allocated pointer itself. Use a separate pointer for iteration.
- After `free`, assign `NULL` to the pointer. Unless there is a specific reason, assign it even if the pointer is not used again. The same applies after calling another function that releases the target, such as `fclose`.

Incorrect:

```c
struct history {
    int *entries;
    size_t count;
};

static int fill_history(struct history *const mut_history, size_t count, int initial)
{
    mut_history->entries = malloc(count * sizeof(*mut_history->entries));
    if (mut_history->entries == NULL) {
        return -1;
    }

    for (size_t i = 0; i < count; ++i) {
        *mut_history->entries = initial;
        ++mut_history->entries;
    }

    mut_history->count = count;
    return 0;
}

static void clear_history(struct history *const mut_history)
{
    free(mut_history->entries);
    mut_history->count = 0;
}
```

```c
static long sum_squares(const int *const values, size_t count)
{
    long *squares = malloc(count * sizeof(*squares));
    if (squares == NULL) {
        return -1;
    }

    long sum = 0;

    for (size_t i = 0; i < count; ++i) {
        squares[i] = (long)values[i] * values[i];
        sum += squares[i];
    }

    free(squares);
    return sum;
}
```

Correct:

```c
struct history {
    int *entries;
    size_t count;
};

static int fill_history(struct history *const mut_history, size_t count, int initial)
{
    mut_history->entries = malloc(count * sizeof(*mut_history->entries));
    if (mut_history->entries == NULL) {
        return -1;
    }

    int *end = mut_history->entries + count;

    for (int *entry = mut_history->entries; entry < end; ++entry) {
        *entry = initial;
    }

    mut_history->count = count;
    return 0;
}

static void clear_history(struct history *const mut_history)
{
    free(mut_history->entries);
    mut_history->entries = NULL;
    mut_history->count = 0;
}
```

```c
static long sum_squares(const int *const values, size_t count)
{
    long *squares = malloc(count * sizeof(*squares));
    if (squares == NULL) {
        return -1;
    }

    long sum = 0;

    for (size_t i = 0; i < count; ++i) {
        squares[i] = (long)values[i] * values[i];
        sum += squares[i];
    }

    free(squares);
    squares = NULL;
    return sum;
}
```

### 13.6. assert

Use `assert` only to check internal invariants. An internal invariant is a condition that never breaks if the program is correct. Do not check failures of external input, files, or system calls with `assert`. Release builds turn off `assert` with `-DNDEBUG`.

Incorrect:

```c
static FILE *open_settings(const char *const path)
{
    FILE *file = fopen(path, "r");

    assert(file != NULL);
    return file;
}
```

Correct:

```c
static FILE *open_settings(const char *const path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "cannot open %s\n", path);
        return NULL;
    }

    return file;
}
```

```c
#define STACK_CAPACITY 32

struct stack {
    int items[STACK_CAPACITY];
    size_t top;
};

static void push_item(struct stack *const mut_stack, int item)
{
    assert(mut_stack->top < STACK_CAPACITY);
    mut_stack->items[mut_stack->top] = item;
    ++mut_stack->top;
}
```

### 13.7. Static assertions

C99 has no `_Static_assert`. Only projects that choose C11 or later use `_Static_assert`. C99 projects use a run-time `assert` or a check in the build system instead.

Correct in a C11 project:

```c
struct record_header {
    uint16_t kind;
    uint16_t flags;
    uint32_t length;
};

_Static_assert(sizeof(struct record_header) == 8, "record_header must stay 8 bytes");
```

Correct in a C99 project:

```c
struct record_header {
    uint16_t kind;
    uint16_t flags;
    uint32_t length;
};

static void check_record_layout(void)
{
    assert(sizeof(struct record_header) == 8);
}
```

### 13.8. Termination

- Terminate on an unrecoverable internal error with `return` or `exit()`.
- If the program cannot continue because of user input or the environment, write a message and terminate with `EXIT_FAILURE`. In `main`, end with `return EXIT_FAILURE;`, and in a function other than `main`, end with `exit(EXIT_FAILURE);`. This is because returning from the initial call to `main` is equivalent to calling `exit` with the returned value, whereas the return value of any other function only goes back to its caller (C99 §5.1.2.2.3 ¶1).

Incorrect:

```c
enum job_state {
    JOB_STATE_IDLE,
    JOB_STATE_RUNNING,
    JOB_STATE_DONE,
};

static enum job_state gs_job_state = JOB_STATE_IDLE;

static void finish_job(void)
{
    if (gs_job_state != JOB_STATE_RUNNING) {
        abort();
    }

    gs_job_state = JOB_STATE_DONE;
}

static long parse_dimension(const char *const text)
{
    char *end = NULL;
    long value = strtol(text, &end, 10);

    if (*end != '\0' || value <= 0) {
        abort();
    }

    return value;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        abort();
    }

    long width = parse_dimension(argv[1]);
    long height = parse_dimension(argv[2]);

    gs_job_state = JOB_STATE_RUNNING;
    printf("area: %ld\n", width * height);
    finish_job();
    return 0;
}
```

Correct:

```c
enum job_state {
    JOB_STATE_IDLE,
    JOB_STATE_RUNNING,
    JOB_STATE_DONE,
};

static enum job_state gs_job_state = JOB_STATE_IDLE;

static void finish_job(void)
{
    if (gs_job_state != JOB_STATE_RUNNING) {
        exit(EXIT_FAILURE);
    }

    gs_job_state = JOB_STATE_DONE;
}

static long parse_dimension(const char *const text)
{
    char *end = NULL;
    long value = strtol(text, &end, 10);

    if (*end != '\0' || value <= 0) {
        fprintf(stderr, "invalid dimension: %s\n", text);
        exit(EXIT_FAILURE);
    }

    return value;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <width> <height>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long width = parse_dimension(argv[1]);
    long height = parse_dimension(argv[2]);

    gs_job_state = JOB_STATE_RUNNING;
    printf("area: %ld\n", width * height);
    finish_job();
    return 0;
}
```

## 14. Tools

### 14.1. clang-format

The `.clang-format` at the project root can be used to check and clean up formatting. Check changed files before committing.

If the default behavior of clang-format conflicts with the rules of this document, do not turn off clang-format as a whole. Change only the settings that control that behavior. The root `.clang-format` has the following settings.

- `BasedOnStyle: LLVM` and `IndentWidth: 4`: Start from the LLVM style and change indentation to four spaces (section 5.1).
- `ColumnLimit: 0`: Lines are broken by hand (sections 5.2 and 5.3). clang-format neither joins nor re-breaks lines broken by hand. It only adjusts indentation and alignment.
- `BreakBeforeBraces: Custom` and `AfterFunction: true` in `BraceWrapping`: Put only the opening brace of a function body on a new line (sections 5.6 and 5.7).
- `InsertBraces: true`: Wrap the bodies of control statements and loops in braces (section 5.8).
- `AllowShortFunctionsOnASingleLine: None`: Do not merge the body of a short function onto one line (section 5.6).
- `AllowShortEnumsOnASingleLine: false`: Do not merge the elements of an enum onto one line (section 5.11).
- `Cpp11BracedListStyle: false`: Put spaces just inside the braces of initializer lists (section 5.11).
- `Kind: Never` in `AlignTrailingComments`: Do not align trailing comments in columns (section 5.18).
- `AlignEscapedNewlines: Left`: Align the line-ending `\` of multi-line macros in one column (section 5.18).
- `IndentPPDirectives: AfterHash` and `PPIndentWidth: 1`: Indent directives inside conditional compilation blocks with one space per level after `#` (section 5.19).
- `IncludeBlocks: Regroup` and `IncludeCategories`: Group includes by category and sort them (section 4.8). The main header is classified as category 0 and comes first. A header with a dependency gets the same `Priority` as the other `<...>` headers and a larger `SortPriority`, which moves it to the end of the same group. In this case, set the `Priority` of the `"..."` group larger than that `SortPriority`.
- `InsertNewlineAtEOF: true`: Add a newline at the end of a file if it is missing (section 4.4).

With these settings, clang-format enforces the final newline (section 4.4), whitespace characters in C sources (section 4.5), include order (section 4.8), indentation (section 5.1), alignment of lines broken inside parentheses (section 5.3), brace placement (sections 5.6 to 5.10), spaces inside initializer list braces (section 5.11), `switch` and goto labels (sections 5.12 and 5.13), spaces and pointer notation (sections 5.14 and 5.15), one statement per line (section 5.16), alignment (section 5.18), preprocessing directives (section 5.19), and column alignment of trailing comments (section 12.2). Apply the remaining rules by hand. In particular, check the following by hand, because clang-format does not fix them or changes them against the rules.

- Lines over 100 columns are not broken (section 5.2). A line broken before a binary operator is joined into one line, so the joined line can exceed 100 columns.
- Lines broken after an assignment operator or after a return type (sections 5.3 and 5.5) and split message strings (section 5.4) are left as they are.
- The trailing comma of a list that will grow is not added, and a designated initializer written on one line is not split per member (section 5.11).
- Braces are inserted around a body without braces, but a following statement that is only indented to match is not put into the body (section 5.8).
- Conditional operators nested without parentheses are broken across lines and aligned (section 8.8). If the inner conditional operator is wrapped in parentheses, the expression stays on one line.
- Blank lines (section 5.17) and inline assembly strings (section 5.20) are not fixed.

In a directory of dependency code, put a `.clang-format` that contains only `DisableFormat: true` and a `.editorconfig` that matches that code (section 1.2). Put `root = true` in this `.editorconfig`. Otherwise it inherits settings such as trailing whitespace removal from the `.editorconfig` at the project root.

A project split into independent trees for each platform (section 1.1) puts a `.clang-format` with the style of that platform in each tree, side by side. clang-format uses the `.clang-format` in the nearest parent directory of a file, so the settings of one tree do not mix into another. `.editorconfig` files are merged with those of parent directories, so put `root = true` in the `.editorconfig` of each tree.

If dependency code shares a directory with project code, list the paths of those files in `.clang-format-ignore` at the project root, and add a separate section to the root `.editorconfig` that selects only those files. clang-format skips the files listed in `.clang-format-ignore`.

Correct:

```sh
clang-format --dry-run --Werror path/to/file.c
clang-format -i path/to/file.c
```

Correct (`.clang-format` at the project root):

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 0
BreakBeforeBraces: Custom
BraceWrapping:
    AfterFunction: true
InsertBraces: true
AllowShortFunctionsOnASingleLine: None
AllowShortEnumsOnASingleLine: false
Cpp11BracedListStyle: false
AlignTrailingComments:
    Kind: Never
AlignEscapedNewlines: Left
IndentPPDirectives: AfterHash
PPIndentWidth: 1
IncludeBlocks: Regroup
IncludeCategories:
    - Regex: '^<readline/'
      Priority: 1
      SortPriority: 2
    - Regex: '^<'
      Priority: 1
      SortPriority: 1
    - Regex: '^"'
      Priority: 3
InsertNewlineAtEOF: true
```

Correct (`.clang-format` in a directory of dependency code):

```yaml
DisableFormat: true
```

Correct (`.editorconfig` in a directory of dependency code indented with two spaces):

```ini
root = true

[*.{c,h}]
indent_style = space
indent_size = 2
```

Correct (a project split into independent trees for each platform):

```text
netprobe/
    linux/
        .clang-format
        .editorconfig
        netprobe.c
    windows/
        .clang-format
        .editorconfig
        netprobe.c
```

Correct (`.clang-format-ignore` at the project root):

```text
src/ini.c
src/ini.h
```

Correct (a section appended to the `.editorconfig` at the project root):

```ini
[src/ini.{c,h}]
indent_style = space
indent_size = 2
trim_trailing_whitespace = false
```

### 14.2. clang-tidy

The `.clang-tidy` at the project root checks the naming rules (chapter 6) with `readability-identifier-naming`. `bugprone-reserved-identifier` checks reserved identifiers (section 6.10), and `readability-inconsistent-declaration-parameter-name` checks parameter names in declarations and definitions (section 10.1). The `mut_` prefix of pointer parameters (section 10.3) cannot be checked, because `readability-identifier-naming` classifies all pointer parameters declared `*const` as the same kind, regardless of whether the target is `const`.

Correct:

```sh
clang-tidy path/to/file.c -- -std=c99
```

### 14.3. GCC/Clang compiler warnings

- With GCC/Clang, build with `-Wall -Wextra -pedantic -Wshadow`.
- Fix all warnings other than unused diagnostics (section 13.2).
- Do not use `-Werror`.

To review the unused diagnostics on their own, run a separate check that keeps only those diagnostics. GCC does not issue `-Wunused-function` when checking with `-fsyntax-only`, so compile all the way to object code and then pick out only the diagnostics marked `-Wunused`. For Clang, turn off all warnings with `-Wno-everything` and then turn only the unused diagnostics back on. The compiler cannot find functions with external linkage that no other file uses, so check the whole project with the cppcheck `unusedFunction` check.

Correct:

```sh
gcc -std=c99 -Wall -Wextra -pedantic -Wshadow -c foo.c
```

Correct (reviewing the unused diagnostics on their own):

```sh
gcc -std=c99 -Wall -Wextra -Wunused-macros -c -o /dev/null foo.c 2>&1 | grep -- '-Wunused'
clang -std=c99 -fsyntax-only -Wno-everything -Wunused -Wunused-parameter -Wunused-macros foo.c
cppcheck --enable=unusedFunction --quiet src/
```

### 14.4. Editor settings

`.editorconfig` tells editors the encoding, line ending, indentation, final newline, and trailing whitespace rules for C sources and headers. `.gitattributes` pins the line endings in the project. Keep editor settings in these two files only, and do not put editor modelines in source files.

Correct:

```ini
root = true

[*.{c,h}]
charset = utf-8
end_of_line = lf
indent_style = space
indent_size = 4
insert_final_newline = true
trim_trailing_whitespace = true
```

## 15. References

- C99 Foreword: N1256 [PDF page 9](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=9), [PDF page 10](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=10) (sections 7.2, 7.6, 7.11, 10.3, and 10.6)
- C99: [ISO/IEC 9899:1999](https://www.iso.org/standard/29237.html). WG14 public draft N1256 [§5.1.1.2](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=22), [§4](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=19), [Annex J.2](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=504) (section 4.4)
- C99 §6.3.2.3: N1256 [PDF page 59](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=59) (sections 8.5 and 8.7)
- C99 §6.5: N1256 [PDF page 79](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=79), [PDF page 80](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=80) (sections 8.2 and 8.5)
- C99 §6.5.7: N1256 [PDF page 97](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=97) (section 8.5)
- C99 §6.7.3.1: N1256 [PDF page 122](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=122) (section 10.3)
- C99 §6.7.4: N1256 [PDF page 124](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=124) (section 10.6)
- C99 §6.7.8: N1256 [PDF page 137](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=137), [PDF page 139](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=139) (section 7.11)
- C99 §7.1.3: N1256 [PDF page 178](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=178) (section 6.10)
- C99 §5.1.2.2.3: N1256 [PDF page 25](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf#page=25) (section 13.8)
- C11 Foreword: N1570 [PDF page 14](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf#page=14) (sections 3.6 and 13.7)
- C11: WG14 public draft N1570 [§5.1.1.2](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf#page=29) (section 4.4)
- C11 §7.24.5.8: N1570 [PDF page 388](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf#page=388) (section 3.5)
- C23: WG14 public draft N3220 [§5.1.1.2](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3220.pdf#page=24) (section 4.4)
- C23 §6.4.1: N3220 [PDF page 66](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3220.pdf#page=66) (section 2.2)
- C23 §6.4.4.1: N3220 [PDF page 70](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3220.pdf#page=70) (section 8.1)
- POSIX.1-2024 XBD [4.19](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap04.html#tag_04_19) (section 12.1)
- POSIX.1-2024 XSH [2.2.1.1](https://pubs.opengroup.org/onlinepubs/9799919799/functions/V2_chap02.html#tag_16_02_01_01), [2.2.1.2](https://pubs.opengroup.org/onlinepubs/9799919799/functions/V2_chap02.html#tag_16_02_01_02) (section 4.7)
- POSIX.1-2024 XSH [2.2.2](https://pubs.opengroup.org/onlinepubs/9799919799/functions/V2_chap02.html#tag_16_02_02) (section 6.10)
- POSIX.1-2024 XSH [strlcpy](https://pubs.opengroup.org/onlinepubs/9799919799/functions/strlcpy.html) (section 2.2)
- POSIX.1-2024 XSH [pthread_mutex_lock](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_lock.html) (section 3.2)
- glibc manual: [Feature Test Macros](https://sourceware.org/glibc/manual/latest/html_node/Feature-Test-Macros.html) (section 4.7)
- glibc manual: [Reserved Names](https://sourceware.org/glibc/manual/latest/html_node/Reserved-Names.html) (section 6.10)
- glibc source: [sysdeps/gnu/net/if.h](https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/gnu/net/if.h;hb=HEAD) (section 4.9)
- Linux man-pages [feature_test_macros(7)](https://man7.org/linux/man-pages/man7/feature_test_macros.7.html) (section 4.7)
- GCC manual: [C Dialect Options](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html) (section 3.1)
- GCC manual: [Built-in Functions for Memory Model Aware Atomic Operations](https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html) (section 3.2)
- GCC manual: [Integers implementation](https://gcc.gnu.org/onlinedocs/gcc/Integers-implementation.html) (section 3.4)
- GCC manual: [-Wunused-function](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-function) (section 4.10)
- GCC manual: [Extended Asm](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#AssemblerTemplate) (section 5.20)
- GCC manual: Extended Asm [Volatile](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Volatile-1) (section 7.9)
- GCC manual: [Binary Constants using the ‘0b’ Prefix](https://gcc.gnu.org/onlinedocs/gcc/Binary-constants.html) (section 8.1)
- GCC manual: [-Wimplicit-fallthrough](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wimplicit-fallthrough) (section 9.1)
- GCC Bugzilla [Bug 66425](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425) (section 13.1)
- GCC manual: [Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) (section 14.3)
- Clang manual: [Differences between various standard modes](https://clang.llvm.org/docs/UsersManual.html#differences-between-various-standard-modes) (section 3.1)
- Clang documentation: [Diagnostic flags: -Wunused](https://clang.llvm.org/docs/DiagnosticsReference.html#wunused) (section 14.3)
- Microsoft [/std (Specify Language Standard Version)](https://learn.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version) (section 3.1)
- CMake source: [Modules/Compiler/MSVC-C.cmake](https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/MSVC-C.cmake) (section 3.1)
- Microsoft [Slim Reader/Writer (SRW) Locks](https://learn.microsoft.com/en-us/windows/win32/sync/slim-reader-writer--srw--locks), [InterlockedIncrement](https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedincrement) (section 3.2)
- clang-format [IncludeCategories](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#includecategories), [IncludeIsMainRegex](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#includeismainregex) (section 14.1)
- clang-format [AlignEscapedNewlines](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#alignescapednewlines), [AlignTrailingComments](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#aligntrailingcomments), [AllowShortEnumsOnASingleLine](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#allowshortenumsonasingleline), [AllowShortFunctionsOnASingleLine](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#allowshortfunctionsonasingleline), [IndentPPDirectives](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#indentppdirectives), [PPIndentWidth](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#ppindentwidth) (section 14.1)
- clang-format [ColumnLimit](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#columnlimit), [DisableFormat](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#disableformat), [BasedOnStyle](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#basedonstyle), [BraceWrapping](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#bracewrapping), [InsertBraces](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#insertbraces) (section 14.1)
- clang-format [.clang-format-ignore](https://clang.llvm.org/docs/ClangFormat.html#clang-format-ignore) (section 14.1)
- clang-tidy [bugprone-reserved-identifier](https://clang.llvm.org/extra/clang-tidy/checks/bugprone/reserved-identifier.html), [readability-identifier-naming](https://clang.llvm.org/extra/clang-tidy/checks/readability/identifier-naming.html), [readability-inconsistent-declaration-parameter-name](https://clang.llvm.org/extra/clang-tidy/checks/readability/inconsistent-declaration-parameter-name.html) (section 14.2)
- Cppcheck [Manual](https://cppcheck.sourceforge.io/manual.pdf) (section 14.3)
- EditorConfig [Specification](https://spec.editorconfig.org/) (sections 14.1 and 14.4)
- [Linux kernel coding style](https://www.kernel.org/doc/html/latest/process/coding-style.html) (section 1.1, chapter 2)
- Linux kernel coding style: [Placing Braces and Spaces](https://www.kernel.org/doc/html/latest/process/coding-style.html#placing-braces-and-spaces) (sections 5.6 and 5.7)
- Linux kernel coding style: [Spaces](https://www.kernel.org/doc/html/latest/process/coding-style.html#spaces) (section 5.15)
- Linux kernel coding style: [Naming](https://www.kernel.org/doc/html/latest/process/coding-style.html#naming) (sections 6.1 and 6.5)
- Linux kernel coding style: [Typedefs](https://www.kernel.org/doc/html/latest/process/coding-style.html#typedefs) (section 6.9)
- Linux kernel coding style: [The inline disease](https://www.kernel.org/doc/html/latest/process/coding-style.html#the-inline-disease) (section 10.6)
- Linux kernel coding style: [Conditional Compilation](https://www.kernel.org/doc/html/latest/process/coding-style.html#conditional-compilation) (section 11.1)
- Linux kernel documentation: [Kernel-doc comments](https://www.kernel.org/doc/html/latest/doc-guide/kernel-doc.html) (section 12.3)
- MISRA C:2025: MISRA [MISRA C](https://misra.org.uk/misra-c/) (chapter 2)
- Doxygen manual: [Documenting the code](https://www.doxygen.nl/manual/docblocks.html) (section 12.3)
- GNU Readline manual: [Custom Functions](https://tiswww.cwru.edu/php/chet/readline/readline.html#Custom-Functions) (section 4.8)
- Git documentation: [gitattributes](https://git-scm.com/docs/gitattributes), [git-config](https://git-scm.com/docs/git-config) (section 4.2)
- GNU Coding Standards: [Naming Variables, Functions, and Files](https://www.gnu.org/prep/standards/html_node/Names.html) (sections 6.1 and 6.2)
- Microsoft [Naming Files, Paths, and Namespaces](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file) (section 4.1)
- Microsoft [Using the Windows Headers](https://learn.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers) (section 4.7)
- Microsoft [Module 1. Your First Windows Program](https://learn.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program) (section 5.7)
- Microsoft [Define consistent coding styles with EditorConfig](https://learn.microsoft.com/en-us/visualstudio/ide/create-portable-custom-editor-options) (sections 4.2 and 14.1)
