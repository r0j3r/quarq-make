struct sym {
    struct sym * n;
    unsigned char * name;
    struct rule * r;
};

struct sym * find_name(struct sym *, unsigned char *);
void add_name(struct sym *, unsigned char *, struct rule *);
struct sym * init_sym_tab(unsigned long);
