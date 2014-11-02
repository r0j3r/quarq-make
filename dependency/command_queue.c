#include <stdlib.h>
#include <string.h>
#include <node.h>
#include "list.h"
#include "queue.h"
#include "dep.h"

struct job
{
    struct node * d;
    unsigned char out_of_date;
};

struct job_queue 
{
    struct job_queue * n;
    short t;
    short h;
    short max;
    struct job q[];
};

int nprocs = 0;

struct job_queue ready = {&ready, 0, 0};
struct job_queue * tail = &ready;

struct job_queue *
init_job_queue(void) {
    struct job_queue * q = malloc(4096);
    memset(q, 0, 4096);
    q->max = (4096 - sizeof(struct job_queue))/sizeof(struct job);
    return q; 
}

void
init_ready_queue(void) {
    struct job_queue * n = init_job_queue();
    n->n = tail->n;
    tail->n = n;
    tail = n;
}

int
enq(void * d) {
    tail->q[tail->t].d = d; 
    tail->q[tail->t].out_of_date = 1;
    tail->t++;
    if (tail->t >= tail->max) {
        struct job_queue *n = init_job_queue();
        n->n = tail->n;
        tail->n = n;
        tail = n; 
        return 0; 
    }
    return 0;
}
    
void *
deq(void)
{
    struct job_queue * head = ready.n;
    void * ret = &(head->q[head->h]);
    head->h++;
    if (head->h >= head->max) {
        ready.n = head->n;
        free(head);
    }
    return ret;
}

struct job * 
next_job() {
    
}

int
execute_command_queue()
{
    
}

struct run_item
{
    int pid;
    void * dep;
};

void *
create_run_item(void * i, int pid)
{
    struct run_item * r;

    r = malloc(sizeof(*r));
    memset(r, 0, sizeof(*r));
    r->dep = i;
    r->pid = pid;
    return r;
}

int
start_job(void * run_q, void * i)
{
    int pid; 
    void * d;

    d = get_data(i);
    pid = run_commands(d);
    if (pid > 0)
    {
        void * r_i;
        r_i = create_run_item(i, pid);
        insert_in_q(run_q, r_i);
    }
    else 
        return -1;
}

int
run_commands(void * d)
{
    int pid;
    
    pid = fork();
    if (pid)
    {
        void * i;
        void * c;

        i = create_iterator(get_commands(d));
        c = next_item(i);
        while(c)
        {
            pid = fork();
            if (pid)
            {
                wait();
                c = next_item(i);
            }
            else
                exec();
        }
    }
    else
    {
        wait();
    }
}

int
clean_up_job(int pid)
{
    void * d;
    d = remove_from_run_q(pid);
    update_dependents(d);
}

int
update_dependents(void * d)
{
    void * i;
    void * v;
    i = create_dep_iter(get_dependents_list(d));
    v = next_dep(i);
    while(v)
    {
        void * data;
        data = get_data(v);
        dec_wait_count(data);
        if (done_waiting(data))
        {
            insert_in_q(ready, v);
        }
        v = next_dep(i);
    }
    destroy_iterator(i);
}
