#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "queue.h"
#include <node.h>
#include "dep.h"
#include "command_queue.h"
#include "file.h"


struct dep_node *
create_dep_node()
{
    struct dep_node * new_node;
    new_node = malloc(sizeof(*new_node));
    if (!new_node) return NULL;
    memset(new_node, 0, sizeof(*new_node));
    return new_node;
};

int
add_source(struct dep_node * d, char * source)
{
    struct list * new_node;

    new_node = malloc(sizeof(*new_node));
    memset(new_node, 0, sizeof(*new_node));
    new_node->data = source;

    if (d->sources)
    {
        new_node->next = d->sources;        
    }
    d->sources = new_node;
    return 0;
}

int
add_target(struct dep_node * d, char * target)
{
    struct list * new_node;

    new_node = malloc(sizeof(*new_node));
    memset(new_node, 0, sizeof(*new_node));
    new_node->data = target;

    if (d->targets)
    {
        new_node->next = d->targets;        
    }
    d->targets = new_node;
    return 0;
}

struct command_vector *
create_cmdline()
{
    struct command_vector * new_vector;

    new_vector = malloc(sizeof(*new_vector));
    memset(new_vector, 0, sizeof(*new_vector));
    new_vector->size = sizeof(char *) * 8;
    new_vector->args = malloc(sizeof(char *) * 8);
    return new_vector;
}

int
add_arg(struct command_vector * v, char * arg)
{
    void *  tmp;
    if (((v->count + 1) * sizeof(char *)) > v->size)
    {
        tmp = realloc(v->args, v->size + v->size);
        if (tmp)
        {
            v->args = tmp;
            v->size += v->size;
        }
        else
        {
            exit(1);
        }
    }
    v->args[v->count] = arg;
    v->count++;
    return 0;
}

int
add_command(struct dep_node * d, struct command_vector * command)
{
    if (d->commands)
        d->commands_tail->next = command;
    else
        d->commands = command;
    d->commands_tail = command;
    return 0;
}

int
get_command(char * c, struct command_vector * d, struct command_vector ** v)
{
    struct command_vector * p; 
    int i;

    for(i = 0, p = d; p; i++)
    {
        if (strcmp(c, p->args[0]) == 0)
        {
            *v = p;
            return 0;
        } 
        p = p->next;
    }
    return -1;
}

int
get_arg(struct command_vector * v, char * a)
{
    
}

int 
add_rule(struct g_vertex ** v, struct dep_node * r)
{
    add_vertex(create_vertex(r), v);
    return 0;
}

int
dep_match(void * a, void * b)
{
    struct list * l;
    struct dep_node * n = a;
   
    l = n->targets; 
    while(l)
    {
        if (strcmp(l->data, b))
            return 1;
        l = l->next;
    }
    return 0;
}

int
eval_dependencies(struct g_vertex * v)
{
    struct g_vertex * t_v;
    struct g_vertex * tail;
    struct g_vertex * head;
    char * s;
    struct list * s_list;
    struct dep_node * d_n;

    t_v = v;
    head = next_vertex(&t_v);
    while(head)
    {
        d_n = head->data;
        s_list = d_n->sources;
        s = next_item(&s_list);
        while(s)
        {
            tail = get_vertex(v ,dep_match, s);
            if (tail)
            {
                add_edge(head, tail);
                add_feedback_edge(tail, head);
                tail->incidence++;
            }
            else
            {
                //create_rule from pattern rules
                return -1;
            }
            s = next_item(&s_list);
        }
        head = next_vertex(&t_v);
    }
    return 0;
}

int
validgraph(struct g_vertex * v)
{
    struct g_vertex * t_v;
    struct g_vertex * head;
    int gen = 1;

    t_v = v;
    head = next_vertex(&t_v);
    while(head)
    {
        if (head->incidence == 0)
        {
            if (dfs(head, gen++) == 0) return 0;
        }
        head = next_vertex(&t_v);
    }
    return 1;
}
    
int
dfs(struct g_vertex * v, int gen)
{
    struct g_edge * e;
    int ret;

    //detect cycles
    if (v->visited == gen)
        return 0;
    else 
        v->visited = gen;

    if (v->e)
    {
        e = v->e;
        while(e)
        {
            if (e->tail)
            {
                ret = dfs(e->tail, gen);
                if (ret == 0) return 0;
            }
            else
                return 0;
            e = e->next;
        }
    }
    
    return 1;
}

//dfs topological sort
//returns 0 no update
//returns -1 on error
//return 1 when update is needed

int
generate_ready_queue(struct g_vertex * v, struct queue * ready_q, struct list ** f_db)
{
    struct g_edge * e;
    int ret;
    struct dep_node * d_n;
  
    d_n = v->data;

    if (v->visited) 
    {
        return d_n->need_update;
    }
    else
    {
        v->visited = 1;

        if (v->e)
        {
            e = v->e;
            while(e)
            {
                if (e->tail)
                {
                    ret = generate_ready_queue(e->tail, ready_q, f_db);
                    if (ret < 0)
                        return ret;
                    if (ret == 1)
                    {
                        d_n->need_update = 1;
                        d_n->wait_count++;
                    }
                }
                else
                    return -1;
                e = e->next;
            }
            if (d_n->need_update)
                insert_in_ready_queue(ready_q, d_n);
            else if (command_changed(d_n))     
            {
                insert_in_ready_queue(ready_q, d_n);
                d_n->need_update = 1;
            }
        }
        else
        {
            if (d_n->commands && d_n->sources)
            {
                struct list * l;
                char * f;
                int changed = 0;

                l = d_n->sources;
                f = next_item(&l);
                while(f)
                {
                    if (file_doesnt_exist(f))
                    {
                        return -1;
                    }
            
                    if (file_changed(f_db, f))
                    {
                        changed++;
                    }
                    f = next_item(&l);
                }
                if (changed)
                    return 1;
            }
            else if (d_n->commands && !d_n->sources)
            {
                insert_in_ready_queue(ready_q, d_n);
                d_n->need_update = 1;
            }
            else
            {
                struct list * l;
                char * f;
                int changed = 0;

                l = d_n->targets;
                f = next_item(&l);
                while(f)
                {
                    if (file_doesnt_exist(f))
                    {
                        return -1;
                    }
            
                    if (file_changed(f_db, f))
                    {
                        changed++;
                    }
                    f = next_item(&l);
                }
                if (changed)
                    return 1;
            }
        }
    }    

    if (d_n->need_update)
        return 1;
    else
        return 0;
}

char *
next_item(struct list ** l)
{
    struct list * s;

    if (!l) return NULL;
    s = *l;
    if (!s) return NULL;
    *l = s->next;
    return s->data;
}

int
dec_wait_count(struct dep_node * n)
{
    n->wait_count--;
}

int
done_waiting(struct dep_node * n)
{
    if (n->wait_count <= 0) 
    {
        n->wait_count = 0;
        return 1;
    }
    return 0;
}

int
run_command(struct dep_node * n)
{
    c = get_command_list(n);
    i = create_iterator(c);         
    l = next_item(i);
    while(l)
    {
        l = next_item(i);
    }
    pid = fork();
    if pid > 0
    exec();
}
