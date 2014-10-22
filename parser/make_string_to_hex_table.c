#include <string.h>
#include <stdio.h>
int
main()
{
    unsigned char t[0xff];
    int i;

    memset(t, 0xff, sizeof(t));
    t['0'] = 0; 
    t['1'] = 1;
    t['2'] = 2;
    t['3'] = 3;
    t['4'] = 4;
    t['5'] = 5;
    t['6'] = 6;
    t['7'] = 7;
    t['8'] = 8;
    t['9'] = 9;
    t['a'] = 0xa;
    t['b'] = 0xb;
    t['c'] = 0xc;
    t['d'] = 0xd;
    t['e'] = 0xe;
    t['f'] = 0xf;
    t['A'] = 0xa;
    t['B'] = 0xb;
    t['C'] = 0xc;
    t['D'] = 0xd;
    t['E'] = 0xe;
    t['F'] = 0xf;

    printf("char string_to_hex_table[] = {");
    for(i = 0 ; i < 0xfe; i++)
    {
        printf("0x%02x, ", t[i]);
    }
    printf("0x%02x};\n", t[i]);
    
}
