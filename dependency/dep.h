struct command_vector
{
    struct command_vector * next;
    int count;
    int size;
    char ** args;
};

struct dep_node
{
    char need_update;
    char wait_count;
    struct list * targets;
    struct list * sources;
    struct command_vector * commands;
    struct command_vector * commands_tail;
};

struct dep_node *
create_dep_node();

int
add_command(struct dep_node *, struct command_vector *);

int
add_target(struct dep_node * d, char * target);

int 
add_rule(struct g_vertex **, struct dep_node * r);

int
eval_dependencies(struct g_vertex *);

struct command_vector *
create_cmdline();

int
add_arg(struct command_vector * v, char * arg);

int
add_source(struct dep_node *, char *);

int
dfs(struct g_vertex *, int);

int
validgraph(struct g_vertex *);
