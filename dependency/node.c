#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "node.h"

struct rule rules = {&rules, 0, 0, 0};
struct rule * tail = &rules;

struct rule *
create_rule(char ** sources, char ** targets, char * commands) {
    struct rule *n = malloc(sizeof(*n));
    memset(n, 0, sizeof(*n));
    n->sources = sources;
    printf("creating rule: %p target %p\n", n, targets);
    n->targets = targets;
    n->commands = commands;
    return n;    
}

void 
add_rule(struct rule * n_r) {
    n_r->next = tail->next;
    tail->next = n_r;
    tail = n_r;
}

struct rule *
find_rule(char * n) {
    struct rule *r = rules.next;
    int m = 1;
    char * s[2];
    s[0] = n;
    s[1] = 0;
    rules.targets = s;
    printf("rules %p looking for %s\n", &rules, n);
    while(m) {
        printf("checking %p targets %p\n", r, r->targets);
        for(int i = 0; r->targets && r->targets[i] && m; i++) {
            printf("possible target %d %s\n", i, r->targets[i]);
            if (r->targets[i] && !strcmp(r->targets[i], n)) {
                    printf("found rule: %p target %s\n", r, r->targets[i]); 
                    m = 0;
            }
        }
        if (m) 
            r = r->next;
    }
    if (r == &rules) {
        return 0;
    }
    printf("found rule %p, name %s command %s\n", r, n, r->commands);
    return r;
}

int
eval_deps(void) {
    struct rule * r = rules.next;
    while(r != &rules) {
        int m = 8; 
        int a = 0; 
        printf("eval %p\n", r); 
        struct rule ** adj = malloc(sizeof(struct rule *) * m);
        for(int i = 0; r->sources && r->sources[i]; i++) {
            struct rule * t = find_rule(r->sources[i]);
            if (t) {
                if (i >= m) {
                    struct rule ** tmp = realloc(adj, (m + m) * sizeof(struct rule *));
                    if (tmp) {
                        adj = tmp;
                        m += m; 
                        adj[a] = 0;
                    } else {
                        free(adj);
                        return -1;
                    }               
                }
                int j; 
                for(j = 0; (j < a) & (adj[j] != t); j++);
                if (j == a) {
                    printf("adj %p, %p %s \n-> %p %s\n", adj, r, r->commands, t, t->commands);
                    t->incidence++;
                    r->adjacency++; 
                    adj[a++] = t;
                }
            }
            r->adj = adj;
        }
        r = r->next;
        printf("next %p\n", r);
    }
    return 0;       
}

int
find_cycles(struct rule * r) {
    int ret;

    //detect cycles
    if (r->marked) {
        return -1;
    } 

    if (0 == r->visited) {
        r->marked = 1;
        if (r->adj) {
            printf("checking adj %p adjacency %d \n", r->adj, r->adjacency); 
            for(int i = 0; i < r->adjacency; i++) {
                ret = find_cycles(r->adj[i]);
                if (-1 == ret) return -1;
            }
        }
        r->marked = 0;
        r->visited = 1;
    }
    
    return 0;
}

int
check_rules() {
    struct rule * r = rules.next;

    while(r != &rules) {
        if (r->visited == 0) {
            if (-1 == find_cycles(r)) { 
                return -1; 
            }
        }
        r = r->next;
    } 
    return 0;
}

struct file_state {
    struct timeval previous;
    struct timeval event;
    struct timeval current;
    int command_len;
    char * commands[];
};

struct job {
    struct rule * r;
    struct job * next; 
};

struct job * job_queue;

void
enqueue_job(struct rule * r) {
    if (r->in_queue) return; 
    struct job * j = malloc(sizeof(*j));
    j->r = r;
    j->next = job_queue->next;
    job_queue->next = j;
    job_queue = j;
    r->in_queue = 1;
}

void 
rollback_queue(struct job * start) {
    struct job * j = start->next, * s;
    start->next = job_queue->next;
    while(j != job_queue) {
        s = j; 
        j = j->next;
        s->r->in_queue = 0;  
        free(s);  
    }
    job_queue = start;
}

struct file_state *
get_state(char * r) {
    struct file_state * ret = malloc(sizeof(*ret));
    memset(ret, 0, sizeof(*ret));
    return ret;
}

int
file_out_of_date(struct file_state * st, char * r) {
    if (st && (st->previous.tv_sec == 0) && (st->previous.tv_usec == 0)) {
        return 1;
    } else if (st && (st->previous.tv_sec != st->current.tv_sec) 
        && (st->previous.tv_usec != st->current.tv_usec)) {
        return 1;
    } 
    return 0;
}

int
update(struct rule * r) {
    int ret = 0, targets_need_update = 0;
    struct file_state * f_st = get_state(r->targets[0]);

    if (r->adj) {
        for(int i = 0; i < r->adjacency; i++) {
            if (r->adj[i]->adj) {
                ret = update(r->adj[i]);
                if (-1 == ret) { 
                    return -1;
                } else if (1 == ret) {
                    targets_need_update++;
                }
            } else {
                switch (file_out_of_date(f_st, r->sources[i])) {
                case 0:
                    break;
                case 1:
                    targets_need_update++;
                    break;
                case -1:
                    return -1;
                
                }
            }
        }
    }

    if (r->commands) {
        if (f_st->command_len != r->command_len) {
            targets_need_update++;
        } else {      
            if (memcmp(f_st->commands, r->commands, f_st->command_len)) {
                targets_need_update++;
            }
        }
    }

    if (targets_need_update) {
        if (r->commands) {
            printf("command %s\n", r->commands);
            enqueue_job(r);
        }
        return 1;
    }

    return 0;
}

void
update_deps(void) {
    struct rule * r = rules.next;
    
    if (&rules == r) {
        printf("rules is empty!\n");
    }

    while(r != &rules) {
        if (0 == r->incidence) {
            struct job * rollback = job_queue;
            if (0 == update(r)) {
                rollback_queue(rollback);
            } 
        }
        r = r->next;
    }
}

void
test(void) {
    char * node_h_target[] = {"node.h", 0};
    char * node_c_target[] = {"node.c", 0};
    char * node_source[] = {"node.c", "node.h", 0};
    char * node_target[] = {"node.o", 0};
    char * node_commands = {"gcc -Wall -c -g -O2 -flto -march=native node.c -o node.o"};
    char * node_exec_source[] = {"node.o", 0};
    char * node_exec_target[] = {"node", 0}; 
    char * node_exec_commands = {"gcc -g -O2 -flto -fuse-linker-plugin -march=native node.o \
-o node"};

    struct rule * new_rule = create_rule(node_source, node_target, node_commands);
    add_rule(new_rule);
    new_rule = create_rule(node_exec_source, node_exec_target, node_exec_commands);
    add_rule(new_rule);
    new_rule = create_rule(0, node_h_target, 0);
    add_rule(new_rule);
    new_rule = create_rule(0, node_c_target, 0);
    add_rule(new_rule);

    eval_deps();
    if (0 == check_rules()) {
        update_deps();
    } else {
        printf("we found cycles in the dependency graph\n");
    }
}

int
main(void) {
    test();
    return 0;
}
