#include <hex_to_string_table.h>

int
main()
{
    char hex_num[] = {0 , 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
    char hex_s[32];
    int i;
    int j;

    for(i = 0, j = 0; i < 16; i++)
    {
         hex_s[j++] = hex_to_string_table[hex_num[i] >> 4]; 
         hex_s[j++] = hex_to_string_table[hex_num[i] & 0xf]; 
    }
    printf("\n");
    for(j = 0; j < 32; j++)
    {
        printf("%c", hex_s[j]);
    }
    printf("\n");
}
