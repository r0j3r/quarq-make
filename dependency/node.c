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
                    printf("%p %s \n-> %p %s\n", r, r->commands, t, t->commands);
                    t->incidence++;
                    r->adjacency++; 
                    adj[a++] = t;
                }
            }
            adj[i] = 0;
            r->adj = adj;
        }
        r = r->next;
        printf("next %p\n", r);
    }
    return 0;       
}

void
update_deps(void) {
    struct rule * r = rules.next;
    
    if (&rules == r) {
        printf("rules is empty!\n");
    }

    while(r != &rules)
    {
        printf("commands %s\nincidence %d, adjacency %d\n", r->commands, r->incidence, 
            r->adjacency);
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
    update_deps();
}

int
main(void) {
    test();
    return 0;
}
