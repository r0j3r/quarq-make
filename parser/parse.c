#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "parse.h"
#include "string_to_hex_table.h"
#include "hex_to_string_table.h"

int
next_tok(enum tokenizer_state * state, char ** in, enum token * tok, 
         char * lex, int * l, int lim)
{
    while(in)
    {
        switch(*state)
        {
        case start:
            if (**in == ' ')
            {
                (*in)++;
                continue;
            }
            else if (**in == '\t')
            {
                (*in)++;
                continue;
            }
            else if (**in == '\n')
            {
                *tok = newline; 
                (*in)++;
                return 0;
            }
            else if (**in == ';')
            {
                *tok = semicolon; 
                (*in)++;
                return 0;
            }
            else if (**in == ':')
            {
                *tok = colon; 
                (*in)++;
                return 0;
            }
            else if (**in == '{')
            {
                *tok = left_brace; 
                (*in)++;
                return 0;
            }
            else if (**in == '}')
            {
                *tok = right_brace; 
                (*in)++;
                return 0;
            }
            else if (**in == 0)
            {
                return 1;
            }
            else
            {
                *state = alphanum;
                continue;
            }
            break;
        case alphanum:
            if (**in == 0)
            {
                lex[*l] = 0;
                return -1;
            }
            else if ((**in == ' ') ||
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
string_to_hex(unsigned char * s, unsigned int l, unsigned char * h)
{
    unsigned int i, j;
    for(i = 0, j = 0; j < l; i++)
    {
        h[i] = string_to_hex_table[s[j++]] << 4;        
        h[i] |= string_to_hex_table[s[j++]];        
    }
}

void
hex_to_string(unsigned char * h, unsigned int l, unsigned char * s)
{
    int j;
    int i;

    for(j = 0, i = 0; i < l; i++)
    {
        s[j++] = hex_to_string_table[h[i] >> 4];
        s[j++] = hex_to_string_table[h[i] & 0xf];
    }
    s[j] = 0;
}

int
next_token_from_file(int fd, enum tokenizer_state * state, char ** in, 
                     char * buff, int buff_size, enum token * tok, char * lex, 
                     int * l, int lim)
{
    int ret;
    while(next_tok(state, in, tok, lex, l, lim) != 0)
    {
        ret = read(fd, buff, buff_size);
        if (ret < 0)
            return -1;
        *in = buff;
    }
    return 0;
}

int
parse_hash_db(int fd, struct list ** db)
{
    int ret;
    enum tokenizer_state state;
    char buff[4096];
    char * in;
    enum token tok; 
    char lex[1024];
    int l;
    char * f_n;
    unsigned char * h_s;

    while(1)
    {
        ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), &tok, 
                                   lex, &l, sizeof(lex));
        while((ret == 0) && (tok != string))
        {
            ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), 
                                      &tok, lex, &l, sizeof(lex));
        }
        while(1)
        {
            if (tok == string)
            {
                f_n = strdup(lex);
            }
            else
                break;
            ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), 
                                       &tok, lex, &l, sizeof(lex));
            if (tok == colon)
            {
                continue;
            }
            else
                break;
            ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff), 
                                       &tok, lex, &l, sizeof(lex));
            if (tok == string)
            {
                h_s = malloc(l/2);
                string_to_hex(lex, l, h_s);
            }
            else
                break;

            create_hash(db, f_n, h_s);
        
            while((ret == 0) && (tok != string))
            {
                ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                           &tok, lex, &l, sizeof(lex));
            }
        }
    }
    return ret;
}

struct dyn_array {
    int sz;
    unsigned char d[];
};

int
parse_mkfile(int fd)
{
    int ret;
    enum tokenizer_state state;
    char buff[4096];
    char * in;
    enum token tok; 
    char lex[1024];
    int l;
    struct dyn_array *targets, *prereqs, *commands; 
    char ** targets_vec, ** prereqs_vec, ** commands_vec;

    init_dyn_array(&targets);
    init_dyn_array(&prereqs);
    init_dyn_array(&commands);
    while(1)
    {
        //get_targets
        while(tok != colon)
        {
           ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                   &tok, lex, &l, sizeof(lex));
           if (string == tok) {
               insert_vec(insert_string(lex, &targets), targets_vec);
           }
        }
        //get sources
        while(tok != left_brace)
        {
           ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                   &tok, lex, &l, sizeof(lex));
           if (string == tok) {
               insert_vec(insert_string(lex, &prereqs), prereqs_vec);
           }
        }
        //get commands
        while(tok != right_brace)
        {
           ret = next_token_from_file(fd, &state, &in, buff, sizeof(buff),
                                   &tok, lex, &l, sizeof(lex));
           if (string == tok) {
               insert_vec(insert_string(lex, commands), commands_vec);
           }
        }
        r = create_rule(targets_vec, sources_vec, commands_vec);
        add_rule(r);
    }
}
