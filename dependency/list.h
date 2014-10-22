struct list
{
    void * data;
    struct list * next;
};

char *
next_item(struct list ** l);
