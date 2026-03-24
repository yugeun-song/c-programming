#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static const char* COLORS[] = {
    ANSI_COLOR_RED,
    ANSI_COLOR_GREEN,
    ANSI_COLOR_BLUE,
    ANSI_COLOR_MAGENTA,
    ANSI_COLOR_CYAN,
    ANSI_COLOR_YELLOW
};

struct member_info {
    size_t offset;
    size_t size;
};

struct internal_padding {
    uint8_t a;
    uint32_t b;
};

struct trailing_padding {
    uint32_t a;
    uint8_t b;
};

struct well_optimized_order {
    uint32_t a;
    uint16_t b;
    uint8_t c;
};

struct bad_optimized_order {
    uint8_t a;
    uint32_t b;
    uint16_t c;
};

struct nested_type_member {
    uint8_t a;
    struct internal_padding b;
};

struct nested_anonymous_member {
    uint8_t a;
    struct {
        uint32_t b1;
        uint8_t b2;
    } b;
};

struct array_member_logic {
    uint8_t a;
    uint32_t b[3];
    uint8_t c[3];
};

struct union_in_struct {
    uint8_t a;
    union {
        uint32_t u1;
        uint8_t u2[8];
    } b;
};

static inline void print_mem_info(void* ptr, size_t size, const struct member_info* members, size_t num_members)
{
    printf("Total Size: %zu bytes\n", size);
    printf("Memory Layout (base: %p)\n ", ptr);

    size_t num_colors = sizeof(COLORS) / sizeof(COLORS[0]);

    for (size_t i = 0; i < size; ++i) {
        uint8_t val = *((uint8_t*)ptr + i);
        int color_idx = -1;

        for (size_t j = 0; j < num_members; ++j) {
            if (i >= members[j].offset && i < members[j].offset + members[j].size) {
                color_idx = (int)(j % num_colors);
                break;
            }
        }

        if (color_idx != -1) {
            printf("%s0x%02X" ANSI_COLOR_RESET " ", COLORS[color_idx], val);
        }
        else {
            printf("0x%02X ", val);
        }
    }
    printf("\n");
}

int main(void)
{
    struct internal_padding inter_pad_struct = { 0, };
    memset(&inter_pad_struct, 0, sizeof(struct internal_padding));
    inter_pad_struct.a = 0xAA;
    inter_pad_struct.b = 0x11223344;

    struct trailing_padding trail_pad_struct = { 0, };
    memset(&trail_pad_struct, 0, sizeof(struct trailing_padding));
    trail_pad_struct.a = 0x11223344;
    trail_pad_struct.b = 0xAA;

    struct well_optimized_order well_order_struct = { 0, };
    memset(&well_order_struct, 0, sizeof(struct well_optimized_order));
    well_order_struct.a = 0x11223344;
    well_order_struct.b = 0xBBCC;
    well_order_struct.c = 0xAA;

    struct bad_optimized_order bad_order_struct = { 0, };
    memset(&bad_order_struct, 0, sizeof(struct bad_optimized_order));
    bad_order_struct.a = 0xAA;
    bad_order_struct.b = 0x11223344;
    bad_order_struct.c = 0xBBCC;

    struct nested_type_member nested_type_struct = { 0, };
    memset(&nested_type_struct, 0, sizeof(struct nested_type_member));
    nested_type_struct.a = 0xAA;
    nested_type_struct.b.a = 0xBB;
    nested_type_struct.b.b = 0x11223344;

    struct nested_anonymous_member nested_anon_struct = { 0, };
    memset(&nested_anon_struct, 0, sizeof(struct nested_anonymous_member));
    nested_anon_struct.a = 0xAA;
    nested_anon_struct.b.b1 = 0x11223344;
    nested_anon_struct.b.b2 = 0xBB;

    struct array_member_logic array_logic_struct = { 0, };
    memset(&array_logic_struct, 0, sizeof(struct array_member_logic));
    array_logic_struct.a = 0xAA;
    array_logic_struct.b[0] = 0x11223344;
    array_logic_struct.b[1] = 0x55667788;
    array_logic_struct.b[2] = 0x99AABBCC;
    array_logic_struct.c[0] = 0xDD;

    struct union_in_struct union_struct = { 0, };
    memset(&union_struct, 0, sizeof(struct union_in_struct));
    union_struct.a = 0xAA;
    union_struct.b.u1 = 0x11223344;

    struct member_info mi_inter[] = {
        { offsetof(struct internal_padding, a), sizeof(inter_pad_struct.a) },
        { offsetof(struct internal_padding, b), sizeof(inter_pad_struct.b) }
    };

    struct member_info mi_trail[] = {
        { offsetof(struct trailing_padding, a), sizeof(trail_pad_struct.a) },
        { offsetof(struct trailing_padding, b), sizeof(trail_pad_struct.b) }
    };

    struct member_info mi_well[] = {
        { offsetof(struct well_optimized_order, a), sizeof(well_order_struct.a) },
        { offsetof(struct well_optimized_order, b), sizeof(well_order_struct.b) },
        { offsetof(struct well_optimized_order, c), sizeof(well_order_struct.c) }
    };

    struct member_info mi_bad[] = {
        { offsetof(struct bad_optimized_order, a), sizeof(bad_order_struct.a) },
        { offsetof(struct bad_optimized_order, b), sizeof(bad_order_struct.b) },
        { offsetof(struct bad_optimized_order, c), sizeof(bad_order_struct.c) }
    };

    struct member_info mi_nested_type[] = {
        { offsetof(struct nested_type_member, a), sizeof(nested_type_struct.a) },
        { offsetof(struct nested_type_member, b) + offsetof(struct internal_padding, a), sizeof(nested_type_struct.b.a) },
        { offsetof(struct nested_type_member, b) + offsetof(struct internal_padding, b), sizeof(nested_type_struct.b.b) }
    };

    struct member_info mi_nested_anon[] = {
        { offsetof(struct nested_anonymous_member, a), sizeof(nested_anon_struct.a) },
        { offsetof(struct nested_anonymous_member, b.b1), sizeof(nested_anon_struct.b.b1) },
        { offsetof(struct nested_anonymous_member, b.b2), sizeof(nested_anon_struct.b.b2) }
    };

    struct member_info mi_array[] = {
        { offsetof(struct array_member_logic, a), sizeof(array_logic_struct.a) },
        { offsetof(struct array_member_logic, b), sizeof(array_logic_struct.b) },
        { offsetof(struct array_member_logic, c), sizeof(array_logic_struct.c) }
    };

    struct member_info mi_union[] = {
        { offsetof(struct union_in_struct, a), sizeof(union_struct.a) },
        { offsetof(struct union_in_struct, b.u1), sizeof(union_struct.b.u1) },
        { offsetof(struct union_in_struct, b.u2), sizeof(union_struct.b.u2) }
    };

    printf("-------------------------------------------------------------------------------\n"
           "struct internal_padding {\n"
           "    uint8_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    uint32_t " ANSI_COLOR_GREEN "b" ANSI_COLOR_RESET ";\n"
           "};\n");
    print_mem_info(&inter_pad_struct, sizeof(inter_pad_struct), mi_inter, sizeof(mi_inter)/sizeof(mi_inter[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct trailing_padding {\n"
           "    uint32_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    uint8_t " ANSI_COLOR_GREEN "b" ANSI_COLOR_RESET ";\n"
           "};\n");
    print_mem_info(&trail_pad_struct, sizeof(trail_pad_struct), mi_trail, sizeof(mi_trail)/sizeof(mi_trail[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct well_optimized_order {\n"
           "    uint32_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    uint16_t " ANSI_COLOR_GREEN "b" ANSI_COLOR_RESET ";\n"
           "    uint8_t " ANSI_COLOR_BLUE "c" ANSI_COLOR_RESET ";\n"
           "};\n");
    print_mem_info(&well_order_struct, sizeof(well_order_struct), mi_well, sizeof(mi_well)/sizeof(mi_well[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct bad_optimized_order {\n"
           "    uint8_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    uint32_t " ANSI_COLOR_GREEN "b" ANSI_COLOR_RESET ";\n"
           "    uint16_t " ANSI_COLOR_BLUE "c" ANSI_COLOR_RESET ";\n"
           "};\n");
    print_mem_info(&bad_order_struct, sizeof(bad_order_struct), mi_bad, sizeof(mi_bad)/sizeof(mi_bad[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct nested_type_member {\n"
           "    uint8_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    struct internal_padding " ANSI_COLOR_GREEN "b" ANSI_COLOR_RESET ";\n"
           "};\n");
    print_mem_info(&nested_type_struct, sizeof(nested_type_struct), mi_nested_type, sizeof(mi_nested_type)/sizeof(mi_nested_type[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct nested_anonymous_member {\n"
           "    uint8_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    struct {\n"
           "        uint32_t " ANSI_COLOR_GREEN "b1" ANSI_COLOR_RESET ";\n"
           "        uint8_t " ANSI_COLOR_BLUE "b2" ANSI_COLOR_RESET ";\n"
           "    } b;\n"
           "};\n");
    print_mem_info(&nested_anon_struct, sizeof(nested_anon_struct), mi_nested_anon, sizeof(mi_nested_anon)/sizeof(mi_nested_anon[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct array_member_logic {\n"
           "    uint8_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    uint32_t " ANSI_COLOR_GREEN "b" ANSI_COLOR_RESET "[3];\n"
           "    uint8_t " ANSI_COLOR_BLUE "c" ANSI_COLOR_RESET "[3];\n"
           "};\n");
    print_mem_info(&array_logic_struct, sizeof(array_logic_struct), mi_array, sizeof(mi_array)/sizeof(mi_array[0]));

    printf("-------------------------------------------------------------------------------\n"
           "struct union_in_struct {\n"
           "    uint8_t " ANSI_COLOR_RED "a" ANSI_COLOR_RESET ";\n"
           "    union {\n"
           "        uint32_t " ANSI_COLOR_GREEN "u1" ANSI_COLOR_RESET ";\n"
           "        uint8_t " ANSI_COLOR_BLUE "u2" ANSI_COLOR_RESET "[8];\n"
           "    } b;\n"
           "};\n");
    print_mem_info(&union_struct, sizeof(union_struct), mi_union, sizeof(mi_union)/sizeof(mi_union[0]));
    printf("-------------------------------------------------------------------------------\n");

    return 0;
}