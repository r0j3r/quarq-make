struct kv_db {
    char * key;
    unsigned char * value;
};

extern struct list * hash_db;

int file_changed(struct list ** db, char * filename);

int hash_differ(unsigned char * a, unsigned char * b);

struct list * open_file_hash_db(char * , struct list ** );

int close_file_hash_db(struct list * db, char * filename);

unsigned char * get_hash(struct list * db, char * f);

int update_hash(struct list * db, char *f, unsigned char * h, unsigned char ** old_hash);

int create_hash(struct list ** db, unsigned char *f, unsigned char * h);

unsigned char * compute_file_hash(char *);

int file_doesnt_exist(char *);
