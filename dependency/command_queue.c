#include <stdlib.h>
#include <string.h>
#include <node.h>
#include "list.h"
#include "queue.h"
#include "dep.h"

int run_queue_len = 0;
int max_job_num = 0;

struct queue ready = {NULL, NULL};
struct queue run = {NULL, NULL};

int
insert_in_queue(struct queue * q, void * d)
{
    struct list * new_item;

    new_item = malloc(sizeof(*new_item));
    memset(new_item, 0, sizeof(*new_item));
    new_item->data = d;
    if (q->head)
    {
        q->tail->next = new_item;
    }
    else
    {
        q->head = new_item;
    }
    q->tail = new_item;
    return 0;
}
    
int
queue_not_empty(struct queue * q)
{
    return q->head != NULL;
}

void *
next_in_queue(struct queue * queue)
{
    return queue->head->data;
}

void *
deq(struct queue * q)
{
    void * i;
    struct list * old;

    if (!q->head) return NULL;
    old = q->head;
    i = q->head->data;
    q->head = q->head->next;
    free(old);
    if (!q->head) q->tail = NULL;
    return i; 
}

int
execute_command_queue()
{
    while(queue_not_empty(&ready))
    {
        void * j;
        if (run_queue_len < max_job_num)
        {
            int status;
            j = next_in_queue(&ready);
            status = start_job(&run, j);
            if (job_started(status))
            {
                deq(&ready);
            }
            run_queue_len++;
        }
        else 
        {
            int s; 
            int pid;

            pid = wait(&s);
            clean_up_job(j, pid);
            run_queue_len--;
        }
    }
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

