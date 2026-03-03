#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED   "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

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

static inline void print_mem_info(void* ptr, size_t size)
{
    printf("Total Size: %zu bytes\n", size);
    printf("Memory Layout (base: %p)\n ", ptr);

    for (size_t i = 0; i < size; ++i) {
        uint8_t val = *((uint8_t*)ptr + i);
        if (val != 0x00) {
            printf(ANSI_COLOR_RED "0x%02X " ANSI_COLOR_RESET, val);
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

    printf("-------------------------------------------------------------------------------\n"
           "struct internal_padding {\n"
           "    uint8_t a;\n"
           "    uint32_t b;\n"
           "};\n");
    print_mem_info(&inter_pad_struct, sizeof(inter_pad_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct trailing_padding {\n"
           "    uint32_t a;\n"
           "    uint8_t b;\n"
           "};\n");
    print_mem_info(&trail_pad_struct, sizeof(trail_pad_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct well_optimized_order {\n"
           "    uint32_t a;\n"
           "    uint16_t b;\n"
           "    uint8_t c;\n"
           "};\n");
    print_mem_info(&well_order_struct, sizeof(well_order_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct bad_optimized_order {\n"
           "    uint8_t a;\n"
           "    uint32_t b;\n"
           "    uint16_t c;\n"
           "};\n");
    print_mem_info(&bad_order_struct, sizeof(bad_order_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct nested_type_member {\n"
           "    uint8_t a;\n"
           "    struct internal_padding b;\n"
           "};\n");
    print_mem_info(&nested_type_struct, sizeof(nested_type_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct nested_anonymous_member {\n"
           "    uint8_t a;\n"
           "    struct {\n"
           "        uint32_t b1;\n"
           "        uint8_t b2;\n"
           "    } b;\n"
           "};\n");
    print_mem_info(&nested_anon_struct, sizeof(nested_anon_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct array_member_logic {\n"
           "    uint8_t a;\n"
           "    uint32_t b[3];\n"
           "    uint8_t c[3];\n"
           "};\n");
    print_mem_info(&array_logic_struct, sizeof(array_logic_struct));

    printf("-------------------------------------------------------------------------------\n"
           "struct union_in_struct {\n"
           "    uint8_t a;\n"
           "    union {\n"
           "        uint32_t u1;\n"
           "        uint8_t u2[8];\n"
           "    } b;\n"
           "};\n");
    print_mem_info(&union_struct, sizeof(union_struct));
    printf("-------------------------------------------------------------------------------\n");

    return 0;
}