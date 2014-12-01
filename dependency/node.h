
struct list
{
    struct list * n;
    unsigned char * name;
};

struct rule 
{
    struct rule * next;
    char ** sources;
    struct rule ** adj;
    char ** targets;
    int incidence;
    int adjacency;
    int command_len; 
    char ** commands;
    unsigned char visited;
    unsigned char marked; 
    unsigned char in_queue;
};
