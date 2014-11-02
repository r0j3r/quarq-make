
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
    char * commands;
    int visited;
};
