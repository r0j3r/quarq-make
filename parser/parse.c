#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "file.h"
#include "parse.h"
#include "string_to_hex_table.h"
#include "hex_to_string_table.h"
#include <rule.h>

int
next_tok(enum tokenizer_state * state, unsigned char ** in, enum token * tok, 
         unsigned char * lex, int * l, int lim)
{
    while(in) {
        switch(*state) {
        case start:
            if (**in == ' ') {
                (*in)++;
                continue;
            } else if (**in == '\t') {
                (*in)++;
                continue;
            }
            else if (**in == '\n') {
                *tok = newline; 
                (*in)++;
                return 0;
            } else if (**in == ';') {
                *tok = semicolon; 
                (*in)++;
                return 0;
            } else if (**in == ':') {
                *tok = colon; 
                (*in)++;
                return 0;
            } else if (**in == '{') {
                *tok = left_brace; 
                (*in)++;
                return 0;
            } else if (**in == '}') {
                *tok = right_brace; 
                (*in)++;
                return 0;
            } else if (**in == 0) {
                return 1;
            } else {
                *state = alphanum;
                continue;
            }
            break;
        case alphanum:
            if (**in == 0) {
                lex[*l] = 0;
                return -1;
            } else if ((**in == ' ') ||
                     (**in == '\n') || 
                     (**in == ';') || 
                     (**in == '{') || 
                     (**in == '}') || 
                     (**in == ':') || 
                     (**in == '\t'))
           {
                lex[*l] = 0;
                *state = start;
                *tok = string;
                return 0;
            }
            lex[(*l)++] = **in;
            if ((*l) >= lim) return -1;
            break;
        default:
            return -1;
        }
        (*in)++;
    }
    return 0;
}

void
string_to_hex(unsigned char * s, unsigned int l, unsigned char * h) {
    unsigned int i, j;
    for(i = 0, j = 0; j < l; i++) {
        h[i] = string_to_hex_table[s[j++]] << 4;        
        h[i] |= string_to_hex_table[s[j++]];        
    }
}

void
hex_to_string(unsigned char * h, unsigned int l, unsigned char * s) {
    int j;
    int i;

    for(j = 0, i = 0; i < l; i++) {
        s[j++] = hex_to_string_table[h[i] >> 4];
        s[j++] = hex_to_string_table[h[i] & 0xf];
    }
    s[j] = 0;
}

int
next_token_from_file(int fd, enum tokenizer_state * state, unsigned char ** in,
                     unsigned char * buff, int buff_size, enum token * tok, 
                     unsigned char * lex, int * l, int lim)
{
    int ret;
    while(next_tok(state, in, tok, lex, l, lim) != 0) {
        ret = read(fd, buff, buff_size);
        if (ret < 0) {
            return -1;
        } 
        *in = buff;
    }
    return 0;
}

int
parse_hash_db(int fd, struct list ** db) {
    int ret;
    enum tokenizer_state state;
    unsigned char buff[4096];
    unsigned char * in;
    enum token tok; 
    unsigned char lex[1024];
    int l;
    unsigned char * f_n;
    unsigned char * h_s;

    while(1) {
        ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), &tok, 
                                   lex, &l, sizeof(lex));
        while((ret == 0) && (tok != string)) {
            ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), 
                                      &tok, lex, &l, sizeof(lex));
        }
        while(1) {
            if (tok == string) {
                f_n = (unsigned char *)strdup((char *)lex);
            } else {
                break;
            }
            ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), 
                                       &tok, lex, &l, sizeof(lex));
            if (tok == colon) {
                continue;
            } else {
                break;
            }
            ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), 
                                       &tok, lex, &l, sizeof(lex));
            if (tok == string) {
                h_s = malloc(l/2);
                string_to_hex(lex, l, h_s);
            } else {
                break;
            } 
            create_hash(db, f_n, h_s);
        
            while((ret == 0) && (tok != string)) {
                ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                           &tok, lex, &l, sizeof(lex));
            }
        }
    }
    return ret;
}

struct dyn_array {
    int sz;
    int n;
    unsigned char d[];
};

struct command_vec {
    int sz;
    int n;
    unsigned char * d[];
};

void
init_dyn_array(struct dyn_array ** a) {
    *a = malloc(sizeof(4 + 60));
    (*a)->sz = 60;
    (*a)->n = 0;
}

void
init_command_vec(struct command_vec ** v) {
    *v = malloc(sizeof(4 + 60));
    (*v)->sz = 60;
    (*v)->n = 0;
}

unsigned char *
insert_string(unsigned char * s, int l, struct dyn_array ** a) {
    while (((*a)->n + l + 1) > (*a)->sz) {
        int sz = (*a)->sz;
        struct dyn_array * t = realloc(*a, sz + sz);
        if (t) {
            *a = t;
            (*a)->sz += sz;
        }
    }
    unsigned char * ret = &((*a)->d[((*a)->n)]);
    memcpy(ret, s, l);
    (*a)->n += l;
    (*a)->d[(*a)->n++] = 0;
    return ret;
}

int
insert_vec(unsigned char * s, struct command_vec ** v)
{
    (*v)->d[(*v)->n++] = s;
    if ((*v)->sz <= ((*v)->n + 1) * sizeof(char *)) {
        int sz = (*v)->sz;
        struct command_vec * t = realloc(*v, sz + sz);
        if (t) {
            *v = t;
            (*v)->sz += sz;
        }
    }
    return (*v)->n;
}

int
parse_mkfile(int fd) {
    int ret = 0;
    enum tokenizer_state state;
    unsigned char buff[4096];
    unsigned char * in;
    enum token tok; 
    unsigned char lex[1024];
    int l;
    struct dyn_array *targets, *prereqs, *commands; 
    struct command_vec *targets_vec, *prereqs_vec, *commands_vec;

    init_dyn_array(&targets);
    init_dyn_array(&prereqs);
    init_dyn_array(&commands);
    init_command_vec(&targets_vec);
    init_command_vec(&prereqs_vec);

    while(1) {
        //get_targets
        while(tok != colon) {
           ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                   &tok, lex, &l, sizeof(lex));
           if (string == tok) {
               insert_vec(insert_string(lex, l, &targets), &targets_vec);
           }
        }
        //get sources
        while(tok != left_brace) {
           ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                   &tok, lex, &l, sizeof(lex));
           if (string == tok) {
               insert_vec(insert_string(lex, l, &prereqs), &prereqs_vec);
           }
        }
        //get commands
        while(tok != right_brace) {
           ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                   &tok, lex, &l, sizeof(lex));
           if (string == tok) {
               insert_vec(insert_string(lex, l, &prereqs), &commands_vec);
           }
        }
        struct rule * r = create_rule(targets_vec->d, prereqs_vec->d, 
            commands_vec->d);
        add_rule(r);
    }
    return ret;
}
