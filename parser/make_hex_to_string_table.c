#include <string.h>
#include <stdio.h>
int
main()
{
    unsigned char t[0xff];
    int i;

    memset(t, 0xff, sizeof(t));
    t[0] = '0'; 
    t[1] = '1';
    t[2] = '2';
    t[3] = '3';
    t[4] = '4';
    t[5] = '5';
    t[6] = '6';
    t[7] = '7';
    t[8] = '8';
    t[9] = '9';
    t[0xa] = 'a';
    t[0xb] = 'b';
    t[0xc] = 'c';
    t[0xd] = 'd';
    t[0xe] = 'e';
    t[0xf] = 'f';

    printf("char hex_to_string_table[] = {");
    for(i = 0 ; i < 0xfe; i++)
    {
        printf("0x%02x, ", t[i]);
    }
    printf("0x%02x};\n", t[i]);
    
}
