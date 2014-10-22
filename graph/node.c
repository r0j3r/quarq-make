#include <stdlib.h>
#include <string.h>
#include "node.h"

//add vertex
struct g_vertex *
create_vertex(void * data)
{
    struct g_vertex * new_v;

    if (!data) return NULL;
    new_v = malloc(sizeof(*new_v));
    memset(new_v, 0, sizeof(*new_v));
    new_v->data = data;
    return new_v;	
}

int
add_vertex(struct g_vertex * v, struct g_vertex ** v_list)
{
    if (!v) return -1;
    if (!v_list) return -1;
    if (*v_list)
        v->next = *v_list;
    *v_list = v;
    return 0;
}

//add edge

int
add_edge(struct g_vertex * from, struct g_vertex * to)
{
    struct g_edge * new_edge;

    if (!from) return -1;
    if (!to) return -1;
    new_edge = malloc(sizeof(*new_edge));
    memset(new_edge, 0, sizeof(*new_edge));
    new_edge->tail = to;
    new_edge->next = from->e;
    from->e = new_edge;
    return 0;
}

int
add_feedback_edge(struct g_vertex * from, struct g_vertex * to)
{
    struct g_edge * new_edge;

    if (!from) return -1;
    if (!to) return -1;
    new_edge = malloc(sizeof(*new_edge));
    memset(new_edge, 0, sizeof(*new_edge));
    new_edge->tail = to;
    new_edge->next = from->u;
    from->u = new_edge;
    return 0;
}

//remove edge

int
remove_edge(struct g_vertex * from, struct g_vertex * to)
{
    struct g_edge * e_list;
    struct g_edge * victim;
    struct g_edge * prev = NULL;

    e_list = from->e;
    //catch cases where the edge is at the start of the list or last remaining
    while(e_list && (e_list->tail == to))
    {
	victim = e_list;
        e_list = e_list->next;
	if (victim)
	{
	    free(victim);
	    victim = NULL;
	}
    }
    if (!e_list) from->e = NULL;
    else 
    {	
        from->e = e_list;
        prev = e_list;
        e_list = e_list->next;
        while(e_list)
        {
	    if (e_list->tail == to)
	        victim = e_list;
	    else
	        prev = e_list;
            e_list = e_list->next;
	    if (victim)
	    {
	        prev->next = e_list;
	        free(victim);
                victim = NULL;		
	    }
        }
    }
    return 0;
}

//remove vertex

int
remove_vertex(struct g_vertex * target, struct g_vertex ** v_list)
{
    struct g_vertex * v;
    struct g_vertex * prev = NULL;

    v = *v_list;
    while(v)
    {
        if (v == target)
        {
            break;
        }
        prev = v;
        v = v->next;
    }
    if (!v) return -1;
    if (target->e) return -1;
    if (prev)
        prev->next = target->next;
    else
        *v_list = target->next;
    free(target);
    return 0;	
}

int
if_edge_exists(struct g_vertex * from, struct g_vertex * to)
{
    struct g_edge * e_list;
    e_list = from->e;
    while(e_list)
    {
	if (e_list->tail == to)
	    return 1;
	e_list = e_list->next;
    }
    return 0;
}

struct g_vertex *
get_vertex(struct g_vertex * list, int (*match)(void *, void *), void * key)
{
    struct g_vertex * n;

    n = list;
    while(n)
    {
	if (match(n->data,key) == 0) return n;
        n = n->next;
    }
    return NULL;
}

struct g_vertex *
next_vertex(struct g_vertex ** list)
{
    struct g_vertex * n;
    struct g_vertex * next_v;

    n = *list;
    if (!n) return NULL;
    next_v = n;
    n = n->next;
    *list = n;
    return next_v;
}

struct g_edge *
get_dependents_list(struct g_vertex * v)
{
    return v->u;
}

struct iterator *
create_dep_iter(struct g_edge * e)
{
    struct iterator * i;
    i = malloc(sizeof(*i));
    memset(i, 0, sizeof(*i));
    i->c = e;
    return i;
}

int
destroy_iterator(struct iterator * i)
{
    free(i);
}

struct g_vertex *
next_dep(struct iterator * i)
{
    struct g_vertex * n; 
    n = i->c->tail;
    i->c = i->c->next;
    return n;
}

void *
get_data(struct g_vertex * v)
{
    return v->data;
}
