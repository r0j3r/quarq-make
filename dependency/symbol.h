struct sym {
    struct sym * n;
    char * name;
    struct rule * r;
};

struct sym * find_name(struct sym *, char *);
void add_name(struct sym *, char *, struct rule *);
struct sym * init_sym_tab(unsigned long);
