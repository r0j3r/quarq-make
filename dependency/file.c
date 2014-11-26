#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <skein.h>
#include "list.h"
#include "file.h"

struct list * hash_db;

int
file_doesnt_exist(char * file) {
    int sd;
    sd = open(file, O_RDONLY);
    if (sd < 0) {
        if (errno == ENOENT) return 1;
    } else {
        close(sd);
    }
    return 0;
}

int
file_changed(struct list ** db, char * f) {
    unsigned char * old_hash;
    unsigned char * new_hash;

    new_hash = compute_file_hash(f);
    old_hash = get_hash(*db, f);

    if (!old_hash) {
        create_hash(db, f, new_hash);
        return 1;
    } else if (hash_differ(old_hash, new_hash)) {
        unsigned char * uh;
        update_hash(*db, f, new_hash, &uh);
        free(uh);
        return 1;
    } else {
        free(new_hash);
    }
    return 0;
}

int
hash_differ(unsigned char * a, unsigned char * b) {
    return memcmp(a, b, 64);
}

struct list *
open_file_hash_db(char * filename, struct list ** hash_db) {
    int fd;
    char buff[4096];
    int ret;
    int parse_ret;

    fd = open(filename, O_RDONLY);
    if (!fd) { 
        return NULL;
    }
    parse_hash_db(fd, hash_db);
    close(fd);
    return NULL;
}

int
append_to_file(int fd, char * buff, int * o, int size, char * s, int l) {
    if ((*o + l) > size) {
        write(fd, buff, *o);
        o = 0;
    }
    memcpy(buff + *o, s, l);
    o += l;
    return 0;
}

int
close_file_hash_db(struct list * db, char * filename) {
    struct list * i;
    char buff[4096];
    char h_s[128];
    char n[1024];
    int fd;
    int o;
   
    sprintf(n, "%s.tmp", filename);
    fd = open(n, O_CREAT|O_EXCL|O_RDWR, 0600);
    i = db;
    while(i) {
        struct kv_db * f_h = i->data;
        append_to_file(fd, buff, &o, sizeof(buff), f_h->key, strlen(f_h->key));
        append_to_file(fd, buff, &o, sizeof(buff), ":", 1);
        hex_to_string(f_h->value, 64, h_s);
        append_to_file(fd, buff, &o, sizeof(buff), h_s, 128);
        append_to_file(fd, buff, &o, sizeof(buff), "\n", 1);
        i = i->next;
    }
    close(fd);
    rename(n, filename);
    return 0;
}

unsigned char *
get_hash(struct list * db, char * f) {
    struct list * i;
    struct kv_db * r;

    i = db;
    while(i) {
        r = i->data;
        if (!strcmp(f, r->key)) {
            return r->value;
        }
        i = i->next;
    }
    return NULL;
}

int
update_hash(struct list * db, char * f, unsigned char * h, 
    unsigned char ** old_hash)
{
    struct list * i;
    struct kv_db * r;

    i = db;
    while(i) {
        r = i->data;
        if (!strcmp(f, r->key)) {
            if (old_hash) {
                *old_hash = r->value;
            }
            r->value = h;
            break;
        }
        i = i->next;
    }
    return 0;

}

int
create_hash(struct list ** db, char *f, unsigned char * h) {
    struct kv_db * new_r;
    struct list * item;

    new_r = malloc(sizeof(*new_r));
    memset(new_r, 0, sizeof(*new_r));
    new_r->key = f;
    new_r->value = h;
    item = malloc(sizeof(*item));
    memset(item, 0, sizeof(*item));
    item->data = new_r;
    if (*db) {
        item->next = *db;
    }
    *db = item;
    return 0;
}

unsigned char *
compute_file_hash(char * f) {
    unsigned char * h;
    Skein_512_Ctxt_t c;
    int fd;
    unsigned char buff[4096];
    int ret;

    memset(&c, 0, sizeof(c));
    fd = open(f, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }

    h = malloc(64);
    Skein_512_Init(&c, 512);
    ret = read(fd, buff, sizeof(buff));
    while(ret > 0) {
        Skein_512_Update(&c, buff, ret);
        ret = read(fd, buff, sizeof(buff));
    }
    close(fd);
    Skein_512_Final(&c, h);
    return h;
}
