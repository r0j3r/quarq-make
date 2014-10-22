#include <CUnit/Basic.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include <node.h>
#include "dep.h"
#include "file.h"
#include <skein.h>

struct g_vertex * v;

int init_suite1(void)
{
    v = NULL;
      return 0;
}

int clean_suite1(void)
{
      return 0;
}

void
test_file_routines(void)
{
    struct list * test_db = NULL;
    struct list * test_db1 = NULL;
    unsigned char * hash_rec;
    int fd;
    unsigned char hash[64];
    unsigned char hash1[64];
    unsigned char * f_hash;
    char * test_string = "this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing";
    char * test_string1 = "another this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing this is a test for skein hashing";
    Skein_512_Ctxt_t c;

    memset(&c, 0, sizeof(c));
    CU_ASSERT(file_doesnt_exist("testfile1"));
    fd = open("testfile1", O_CREAT|O_RDWR, 0600);
    CU_ASSERT(fd > 0);
    close(fd);
    CU_ASSERT(file_doesnt_exist("testfile1") == 0);
    unlink("testfile1");

    Skein_512_Init(&c, 512);
    Skein_512_Update(&c, (unsigned char *)test_string, strlen(test_string));
    Skein_512_Final(&c, hash);

    Skein_512_Init(&c, 512);
    Skein_512_Update(&c, (unsigned char *)test_string1, strlen(test_string1));
    Skein_512_Final(&c, hash1);

    fd = open("testfile2", O_CREAT|O_RDWR, 0600);
    CU_ASSERT(fd > 0);
    write(fd, test_string, strlen(test_string));
    close(fd);
    f_hash = compute_file_hash("testfile2");
    unlink("testfile2");

    CU_ASSERT(memcmp(f_hash, hash, 64) == 0);
    CU_ASSERT(memcmp(f_hash, hash1, 64) != 0);
    CU_ASSERT(hash_differ(f_hash, hash) == 0);
    CU_ASSERT(hash_differ(f_hash, hash1) != 0);
        
    create_hash(&test_db, "testfile2", hash);
    create_hash(&test_db, "testfile3", hash1);

    hash_rec = get_hash(test_db, "testfile2");
    CU_ASSERT(hash_rec != NULL);
    if (hash_rec)
    {
        CU_ASSERT(memcmp(hash, hash_rec, 64) == 0);
    }
    hash_rec = get_hash(test_db, "testfile3");
    CU_ASSERT(hash_rec != NULL);
    if (hash_rec)
    {
        CU_ASSERT(memcmp(hash1, hash_rec, 64) == 0);
    }

    fd = open("testfile2", O_CREAT|O_RDWR, 0600);
    CU_ASSERT(fd > 0);
    write(fd, test_string, strlen(test_string));
    close(fd);

    CU_ASSERT(file_changed(&test_db1, "testfile2") == 1);    
    CU_ASSERT(file_changed(&test_db1, "testfile2") == 0);    

    fd = open("testfile2", O_RDWR, 0600);
    CU_ASSERT(fd > 0);
    write(fd, test_string1, strlen(test_string1));
    close(fd);

    CU_ASSERT(file_changed(&test_db1, "testfile2") == 1);    
    unlink("testfile2");
}

void
test_rule(void)
{
    struct dep_node * new_node;
    struct list * s;
    char * l;
    struct command_vector * cmdline;
    char * command_list[] = {"command", "command2", "command3"};
    int ret;

    //items in these lists are pushed in reverse order
    char * source_list[] = {"source3", "source2", "source1" };
    char * target_list[] = {"target3", "target2", "target1" };

    int i;
    int c;

    new_node = create_dep_node();
    add_source(new_node, "source1");
    add_source(new_node, "source2");
    add_source(new_node, "source3");
    add_target(new_node, "target1");
    add_target(new_node, "target2");
    add_target(new_node, "target3");
    cmdline = create_cmdline();
    add_arg(cmdline, "command");
    add_arg(cmdline, "arg1");
    add_arg(cmdline, "arg2");
    add_arg(cmdline, "arg3");
    add_arg(cmdline, "arg4");
    add_arg(cmdline, "arg5");
    add_arg(cmdline, "arg6");
    add_arg(cmdline, "arg7");
    add_arg(cmdline, "arg8");
    add_arg(cmdline, "arg9");
    CU_ASSERT(cmdline->args != NULL);
    CU_ASSERT(cmdline->count == 10);
    CU_ASSERT(cmdline->size == 128);
    CU_ASSERT(strcmp(cmdline->args[0], "command") == 0);
    CU_ASSERT(strcmp(cmdline->args[1], "arg1") == 0);
    CU_ASSERT(strcmp(cmdline->args[2], "arg2") == 0);
    CU_ASSERT(strcmp(cmdline->args[3], "arg3") == 0);
    CU_ASSERT(strcmp(cmdline->args[4], "arg4") == 0);
    CU_ASSERT(strcmp(cmdline->args[5], "arg5") == 0);
    CU_ASSERT(strcmp(cmdline->args[6], "arg6") == 0);
    CU_ASSERT(strcmp(cmdline->args[7], "arg7") == 0);
    CU_ASSERT(strcmp(cmdline->args[8], "arg8") == 0);
    CU_ASSERT(strcmp(cmdline->args[9], "arg9") == 0);

    add_command(new_node, cmdline);

    cmdline = create_cmdline();
    add_arg(cmdline, "command2");
    add_arg(cmdline, "arg1");
    add_command(new_node, cmdline);

    cmdline = create_cmdline();
    add_arg(cmdline, "command3");
    add_arg(cmdline, "arg1");
    add_command(new_node, cmdline);

    add_rule(&v, new_node);

    s = new_node->sources; 
    CU_ASSERT(s != NULL);
    l = next_item(&s);
    CU_ASSERT(l != NULL);
    for(i = 0, c = 0; l && (i < 3); i++) 
    {
        if (strcmp(l, source_list[i]) == 0) c++;
        l = next_item(&s);
    }
    CU_ASSERT(c == 3);
    s = new_node->targets; 
    CU_ASSERT(s != NULL);
    for(i = 0, c = 0; i < 3; i++) 
    {
        if (strcmp(s->data, target_list[i]) == 0) c++;
        s = s->next;
    }
    CU_ASSERT(c == 3);

    cmdline = new_node->commands;
    for(i = 0, c = 0; i < 3; i++) 
    {
        if (strcmp(cmdline->args[0], command_list[i]) == 0) c++;
        cmdline = cmdline->next;
    }
    CU_ASSERT(c == 3);

    ret = eval_dependencies(v);
    CU_ASSERT(ret != 0);
    
    new_node = create_dep_node();
    add_target(new_node, "source1");
    add_rule(&v, new_node);

    new_node = create_dep_node();
    add_target(new_node, "source2");
    add_rule(&v, new_node);

    new_node = create_dep_node();
    add_target(new_node, "source3");
    add_rule(&v, new_node);

    ret = eval_dependencies(v);
    CU_ASSERT(ret == 0);

    ret = validgraph(v);
    CU_ASSERT(ret == 1);
}

int main()
{
   CU_pSuite pSuite = NULL;

   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite, "test of rule", test_rule)) ||
   (NULL == CU_add_test(pSuite, "test of file routines", test_file_routines)))
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

