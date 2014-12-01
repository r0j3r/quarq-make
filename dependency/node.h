
struct list
{
    struct list * n;
    unsigned char * name;
};

struct rule 
{
    struct rule * next;
    unsigned char ** sources;
    struct rule ** adj;
    unsigned char ** targets;
    int incidence;
    int adjacency;
    int command_len; 
    unsigned char ** commands;
    unsigned char visited;
    unsigned char marked; 
    unsigned char in_queue;
};
