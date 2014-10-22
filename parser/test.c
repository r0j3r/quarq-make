#include <CUnit/Basic.h>
#include <stdlib.h>
#include "parse.h"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */

int
string_match(void * a, void * b)
{
    return strcmp(a, b);
}

struct g_vertex * v;
int init_suite1(void)
{
    v = NULL;
      return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
      return 0;
}

void test_tokenizer(void)
{
    char hash_db[] = {"file1:lsdfkjsdfjsdjfsdkfslkdfslkdjf\n"
                      "file2:lsdfkjsdfjsdjfsdkfslkdfslkdjf\n"
                      "file3:lsdfkjsdfjsdjfsdkfslkdfslkdjf\n"};

    enum token tok_tab[]  = {string, colon, string, newline, 
                             string, colon, string, newline, 
                             string, colon, string, newline};

    char * strings_tab[] = {"file1","lsdfkjsdfjsdjfsdkfslkdfslkdjf",
                            "file2","lsdfkjsdfjsdjfsdkfslkdfslkdjf",
                            "file3","lsdfkjsdfjsdjfsdkfslkdfslkdjf"};

    char test_source[] = {
        "target : source\n"
        "{\n"
        "    command1 arg1 arg2 arg3;\n"
        "    command2 arg1 arg2 arg3;\n"
        "}\n"
    };

    enum token source_tok_tab[] = {string,colon,string,newline,
                                    left_brace,newline,
                string,string,string,string,semicolon,newline,
                string,string,string,string,semicolon,newline,
                right_brace,newline};

    char * source_strings[] = {"target","source", 
                             "command1","arg1","arg2","arg3",
                             "command2","arg1","arg2","arg3"};

    char * i = hash_db; 
    enum token t;
    enum tokenizer_state s = start;
    char lx[1024];
    int l = 0;
    int t_i = 0;
    int strings_i = 0;
    int string_score = 0;
    int tok_score = 0;

    while(next_tok(&s, &i, &t, lx, &l, sizeof(lx)) == 0)
    {
        if (t == tok_tab[t_i++])
        {
            tok_score++;     
            if (t == string)
            {
                if (memcmp(lx, strings_tab[strings_i++], l) == 0)
                {
                    string_score++;
                }
            }
        }
        l = 0;
    }
    CU_ASSERT((string_score == 6) && (tok_score == 12));

    i = test_source; 
    l = 0;
    s = start;
    t_i = 0;
    strings_i = 0;
    string_score = 0;
    tok_score = 0;
    while(next_tok(&s, &i, &t, lx, &l, sizeof(lx)) == 0)
    {
        if (t == source_tok_tab[t_i++])
        {
            tok_score++;     
            if (t == string)
            {
                if (memcmp(lx, source_strings[strings_i++], l) == 0)
                {
                    string_score++;
                }
            }
        }
        l = 0;
    }
    CU_ASSERT((string_score == 10) && (tok_score == 20));

}

void
test_serialization()
{
    int i;
    unsigned char * s = "012345678901234567890123456789012345678901234567890123456789"
    "0123456789012345678901234567890123456789012345678901234567890123456789";
    unsigned char h[64];
    unsigned char hex[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89, 
                 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89,
                 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89,
                 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89,
                 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89,
                 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89,
                 0x01, 0x23, 0x45, 0x67};
    unsigned char hex_s[129];

    string_to_hex(s, 128, h);
    CU_ASSERT(memcmp(h, hex, 64) == 0);
    hex_to_string(h, 64, hex_s);
    CU_ASSERT(memcmp(hex_s, s, 128) == 0);
}

int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "test of tokenizer", test_tokenizer)) ||
       (NULL == CU_add_test(pSuite, "test of serialization", test_serialization)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}

