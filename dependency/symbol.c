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
        tab[i]->n = &tab[i];
    }
    return tab;
}

struct sym *
find_name(struct sym * s, char * n) {
    long i = get_hash(n);
    struct sym * s = sym_tab[i]->n;
    s->name = n;
    while(strcmp(s->name, n)) {
        s = s->n;
    }
    if (s == sym_tab[i]) {
        return 0;
    } else {
        return s;
    }
}

void
add_name(char * name, strutc rule * r)
{
    long i = get_hash(name);
    
    struct sym * s = malloc(sizeof(*s));
    s->name = name;
    s->r = r;

    s->n = sym_tab[i].n;
    sym_tab[i].n = s;
}
