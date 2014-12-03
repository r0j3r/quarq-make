#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "spooky-c.h"

unsigned long get_index(unsigned char * name);

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
find_name(struct sym * s, unsigned char * n) {
    long i = get_index(n);
    struct sym * n_s = s[i].n;
    n_s->name = n;
    while(strcmp((char *)n_s->name, (char *)n)) {
        n_s = n_s->n;
    }
    if (n_s == &s[i]) {
        return 0;
    } else {
        return n_s;
    }
}

void
add_name(struct sym * s, unsigned char * name, struct rule * r)
{
    long i = get_index(name);
    
    struct sym * n_s = malloc(sizeof(*s));
    n_s->name = name;
    n_s->r = r;

    n_s->n = s[i].n;
    s[i].n = n_s;
}

unsigned long 
get_index(unsigned char * name) {
    return spooky_hash64(name, strlen((char *)name), 0xdeadbeefdeadbeef) % (1 << 7); 
}
