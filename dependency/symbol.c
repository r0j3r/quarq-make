#include <stdlib.h>
#include <string.h>
#include "spooky-c.h"

unsigned long get_index(char * name);

struct sym {
    struct sym * n;
    char * name;
    struct rule * r;
};

struct sym * sym_tab;

struct sym *
init_sym_tab(unsigned long sz) {
    struct sym * tab = malloc(sizeof(struct sym *) * sz);
    memset(tab, 0, sizeof(struct sym *) * sz);
    for(int i = 0; i < sz; i++) {
        tab[i].n = &tab[i];
    }
    return tab;
}

struct sym *
find_name(struct sym * s, char * n) {
    long i = get_index(n);
    struct sym * n_s = s[i].n;
    n_s->name = n;
    while(strcmp(n_s->name, n)) {
        s = s->n;
    }
    if (s == &s[i]) {
        return 0;
    } else {
        return s;
    }
}

void
add_name(struct sym * s, char * name, struct rule * r)
{
    long i = get_index(name);
    
    struct sym * n_s = malloc(sizeof(*s));
    n_s->name = name;
    n_s->r = r;

    n_s->n = s[i].n;
    s[i].n = n_s;
}

unsigned long 
get_index(char * name) {
    return spooky_hash64(name, strlen(name), 0xdeadbeefdeadbeef) % (1 << 7); 
}
