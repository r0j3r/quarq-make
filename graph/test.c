#include <CUnit/Basic.h>
#include <stdlib.h>
#include "node.h"

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

void test_add_vertex(void)
{
    int ret; 
    struct g_vertex * target;

    //create vertex with label
    ret = add_vertex(create_vertex("all"), &v);
    CU_ASSERT(ret == 0);
    //create vertex with NULL label
    ret = add_vertex(create_vertex(NULL), &v);
    CU_ASSERT(ret);
    ret = add_vertex(NULL, &v);
    CU_ASSERT(ret);
    ret = add_vertex(create_vertex("first"), NULL);
    CU_ASSERT(ret);
    ret = add_vertex(create_vertex(NULL), NULL);
    CU_ASSERT(ret);
    ret = add_vertex(create_vertex("first"), &v);
    CU_ASSERT(ret == 0);
    target = get_vertex(v, string_match, "first");
    CU_ASSERT(target != NULL);
    target = get_vertex(v, string_match, "all");
    CU_ASSERT(target != NULL);
    target = get_vertex(v, string_match, "not there");
    CU_ASSERT(target == NULL);
}

void test_add_edge(void)
{
    struct g_vertex * head = NULL;
    struct g_vertex * tail = NULL;
    struct g_vertex * tail01 = NULL;
    struct g_vertex * tail002 = NULL;
    struct g_vertex * verts;
    struct g_edge * e;
    struct g_edge * target_e;
    int ret;


    head = get_vertex(v, string_match, "all");
    tail = get_vertex(v, string_match, "first");

    ret = add_edge(NULL, NULL);
    CU_ASSERT(ret != 0);
    ret = add_edge(NULL, tail);
    CU_ASSERT(ret != 0);
    ret = add_edge(head, NULL);
    CU_ASSERT(ret != 0);

    ret = add_edge(head, tail);
    CU_ASSERT(ret == 0);
    ret = add_edge(head, tail);
    CU_ASSERT(ret == 0);

    CU_ASSERT(if_edge_exists(head, tail));

    ret = remove_edge(head, tail);
    CU_ASSERT(ret == 0);

    CU_ASSERT(if_edge_exists(head, tail) == 0);

    ret = add_vertex(create_vertex("second"), &v);
    tail01 = get_vertex(v, string_match, "second");
    ret = add_vertex(create_vertex("third"), &v);
    tail002 = get_vertex(v, string_match, "third");

    ret = add_edge(head, tail01);
    ret = add_edge(head, tail);
    ret = add_edge(head, tail002);

    CU_ASSERT(if_edge_exists(head, tail));
    CU_ASSERT(if_edge_exists(head, tail01));

    ret = remove_edge(head, tail002);
    CU_ASSERT(if_edge_exists(head, tail));
    CU_ASSERT(if_edge_exists(head, tail01));
    CU_ASSERT(if_edge_exists(head, tail002) == 0);

    ret = remove_edge(head, tail01);
    CU_ASSERT(if_edge_exists(head, tail));
    CU_ASSERT(if_edge_exists(head, tail01) == 0);
    CU_ASSERT(if_edge_exists(head, tail002) == 0);
    ret = remove_edge(head, tail);

    CU_ASSERT(if_edge_exists(head, tail) == 0);
    CU_ASSERT(if_edge_exists(head, tail01) == 0);
    CU_ASSERT(if_edge_exists(head, tail002) == 0);

    add_edge(head, tail01);
    CU_ASSERT(if_edge_exists(head, tail01));
    add_edge(tail01, tail002);
    CU_ASSERT(if_edge_exists(tail01, tail002));

    ret = remove_vertex(tail01, &v);
    CU_ASSERT(ret);
    tail01 = get_vertex(v, string_match, "second");
    CU_ASSERT(tail01 != NULL);

    verts = v;
    while(v)
    {
        remove_edge(v, tail01);
        v = v->next;
    }    
    v = verts;
    e = tail01->e;
    while(e)
    {
        target_e = e;
        e = e->next;
        free(target_e);
    }
    tail01->e = NULL;

    ret = remove_vertex(tail01, &v);
    CU_ASSERT(ret == 0);
    tail01 = get_vertex(v, string_match, "second");
    CU_ASSERT(tail01 == NULL);
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
   if ((NULL == CU_add_test(pSuite, "test of add vertex", test_add_vertex)) ||
       (NULL == CU_add_test(pSuite, "test of add edge", test_add_edge)))
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

