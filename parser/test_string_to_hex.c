#include "string_to_hex_table.h"

int
main()
{
    unsigned char n[10];
    unsigned char s[] = "0123456789abcdef";

    n[0] = string_to_hex_table[s[0]] << 4;
    n[0] |= string_to_hex_table[s[1]];
    printf("n = %x\n", n[0]);
    n[1] = string_to_hex_table[s[2]] << 4;
    n[1] |= string_to_hex_table[s[3]];
    printf("n = %x\n", n[1]);
    n[2] = string_to_hex_table[s[4]] << 4;
    n[2] |= string_to_hex_table[s[5]];
    printf("n = %x\n", n[2]);
    n[3] = string_to_hex_table[s[6]] << 4;
    n[3] |= string_to_hex_table[s[7]];
    printf("n = %x\n", n[3]);
    n[4] = string_to_hex_table[s[8]] << 4;
    n[4] |= string_to_hex_table[s[9]];
    printf("n = %x\n", n[4]);
    n[5] = string_to_hex_table[s[10]] << 4;
    n[5] |= string_to_hex_table[s[11]];
    printf("n = %x\n", n[5]);
    n[6] = string_to_hex_table[s[12]] << 4;
    n[6] |= string_to_hex_table[s[13]];
    printf("n = %x\n", n[6]);
    n[7] = string_to_hex_table[s[14]] << 4;
    n[7] |= string_to_hex_table[s[15]];
    printf("n = %x\n", n[7]);

}
