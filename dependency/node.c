#define __USE_LARGEFILE64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "node.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

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

struct rule * find_rule(char * n);

void 
add_rule(struct rule * n_r) {
    int i;
    for(i = 0; 0 == find_rule(n_r->targets[i]); i++);
    if (n_r->targets[i]) return;
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

void
canondir(char * p) {
    char * d = strdup(p);
    char * c;
    char n[1024];
   
    while(d) {
        c = strsep(&d, "/");
        if (c) {
            sprintf(n + strlen(n),"/%s", c);
            mkdir(n, 0755);
        } 
    }
}

char *
get_realpath(char * f) {

    char * p = realpath(f, 0);

    if (p) {
        return p;
    } else {
        if (ENOENT == errno) {
            char * b;
            char * d = strdup(f);
            char * l = strrchr(d, '/');
            if (l) {
                *l = 0;
                b = l + 1;
            } else {
                b = f;
            }
            if (0 == strlen(d)) {
                p = realpath(".", 0);
            } else {
                canondir(d);
                p = realpath(d, 0);
            }
            free(d);
            if (p) {
                char * t = realloc(p, strlen(p) + strlen(b) + 1); 
                if (t) {
                    sprintf(t + strlen(t), "/%s", b); 
                    free(p);
                    return t;  
                } else {
                    free(p);
                    return 0;
                }  
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
}

int
eval_deps(void) {
    struct rule * r = rules.next;
    while(r != &rules) {
        int m = 8; 
        int a = 0; 
        printf("eval %p\n", r); 
        struct rule ** adj = malloc(sizeof(struct rule *) * m);
        if (r->sources) { 
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
        }
        for (int i = 0; r->targets[i]; i++) {
            char * p = get_realpath(r->targets[i]);
            if (p) {
                if (find_name(p)) {
                    printf("ambiguous target: %s %s\n", r->targets[i], p);
                    return -1;
                } else {
                    add_name(p, r);
                }
            }
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
    struct timespec previous;
    struct timeval event;
    struct timespec current;
    int command_len;
    char * commands[];
};

struct job {
    struct rule * r;
    struct job * next; 
};

struct job * job_queue;

void
init_job_queue() {
    job_queue = malloc(sizeof(*job_queue));
    memset(job_queue, 0, sizeof(*job_queue));
    job_queue->next = job_queue;
}

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

void
run_job_queue(void){
    //start worker threads

    struct job * d = job_queue->next, * r;
    r = d->next;
    while(r != d) {
        r = r->next;
    }

    //wait for threads to exit
}

struct file_state *
get_state(char * r) {
    struct file_state * ret = 0;
    char * p = realpath(r, 0);
    if (p) {
        char * state_file_path  = malloc(strlen(p) + 7);
        sprintf(state_file_path, "%s%s", "r", ".state");
        int fd = open(state_file_path, O_RDONLY);
        if (fd != -1) {
            struct stat64 st_buff;
            fstat64(fd, &st_buff);
            ret = malloc(st_buff.st_size);
            int err;
            int bytes_read = 0;
            while ((err = read(fd, ret + bytes_read, st_buff.st_size 
                - bytes_read))) {
                if (-1 != err) {
                     bytes_read += err;
                }
            }
            close(fd); 
        } 
    } else {
        ret = malloc(sizeof(*ret));
        memset(ret, 0, sizeof(*ret));
    }
    return ret;
}

int
file_out_of_date(struct file_state * st, char * r) {
    char * p = realpath(r, 0);
    if (p) {
        struct stat64 st_buff; 
        stat64(p, &st_buff);
        st->current.tv_sec = st_buff.st_mtime;
        st->current.tv_nsec = st_buff.st_mtim.tv_nsec;
    }
    if (st && (st->previous.tv_sec == 0) && (st->previous.tv_nsec == 0)) {
        return 1;
    } else if (st && (st->previous.tv_sec != st->current.tv_sec) 
        && (st->previous.tv_nsec != st->current.tv_nsec)) {
        return 1;
    } 
    return 0;
}

//find out of date prereqs 
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

    init_job_queue();

    while(r != &rules) {
        if (0 == r->incidence) {
            struct job * rollback = job_queue;
            if (0 == update(r)) {
                rollback_queue(rollback);
            } 
        }
        r = r->next;
    }

    run_job_queue();
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
