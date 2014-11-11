struct sym {
    char * name;
    struct rule * r;
};

struct sym * sym_tab;

struct sym *
init_sym_tab(unsigned long sz) {
    struct sym * tab = malloc(sizeof(struct sym *) * sz);
    memset(tab, 0, sizeof(struct sym *) * sz);
    return tab;
}
