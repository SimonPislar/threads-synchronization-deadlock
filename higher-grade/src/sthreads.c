/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.
*/
#define _XOPEN_SOURCE 700

/* On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/

#include <signal.h>   /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>    /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h>   /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */
#include <stdbool.h>  /* true, false */

#include "sthreads.h"

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ * 100

/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/
typedef struct thread_queue
{
    thread_t *running;
    thread_t *first;
    thread_t *last;
} thread_queue_t;

thread_queue_t *thread_queue;

tid_t tid = 100;

/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/
tid_t get_tid()
{
    return tid += 1;
}

thread_t *thread_create()
{
    thread_t *new_thread = malloc(sizeof(thread_t));
    return new_thread;
}

void *stack_create()
{
    void *stack = malloc(STACK_SIZE);
    return stack;
}

void dequeue()
{
    thread_t *to_run = thread_queue->first;
    thread_queue->running = to_run;
    to_run->state = running;
    thread_queue->first = thread_queue->first->next;
}

void enqueue(thread_t *thread)
{
    if (thread_queue->first)
    {
        thread_queue->last->next = thread;
        thread_queue->last = thread;
    }
    else
    {
        thread_queue->first = thread;
        thread_queue->last = thread;
    }
}

void begin()
{
    dequeue();
    setcontext(&thread_queue->running->ctx); // ändrar current ctx till running ctx
}
/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/

int init()
{
    thread_queue = malloc(sizeof(thread_queue_t));

    if (thread_queue != NULL)
    {
        return 1;
    }
    return -1;
}

tid_t spawn(void (*begin)(), ucontext_t *ctx)
{
    thread_t *new_thread = thread_create();
    void *stack = stack_create();

    if (stack == NULL)
    {
        return -1;
    }

    int result = getcontext(ctx); // ???

    if (result < 0)
    {
        return -1;
    }

    ctx->uc_link = NULL;
    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = STACK_SIZE;
    ctx->uc_stack.ss_flags = 0;

    makecontext(ctx, begin, 0); // set up for swapcontext när vi kallar swap kommer kördet på ctx kör begin

    new_thread->tid = get_tid();
    new_thread->state = ready;
    new_thread->ctx = *ctx;
    new_thread->next = NULL;

    enqueue(new_thread);

    return new_thread->tid;
}
// changes the state ready to runnig
void yield()
{
    thread_queue->running->state = ready; // changes the state of the running thread to ready
    enqueue(thread_queue->running);       // den som kördes lägger vi i slutet av kön

    dequeue(); // gå vi till nästa tråd och ändra den till running

    if (thread_queue->first)
    {
        swapcontext(&thread_queue->last->ctx, &thread_queue->running->ctx); // ??? lägger currently running i last och sen running ctx till running
    }
}

void done()
{
}

tid_t join(tid_t thread)
{
    return -1;
}
