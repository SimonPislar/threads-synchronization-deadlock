/**
 * Rendezvous
 *
 * Two threads executing chunks of work in lock step.
 *
 * Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
 *
 */

#include <stdio.h>     /* printf() */
#include <stdlib.h>    /* abort(), [s]rand() */
#include <unistd.h>    /* sleep() */
#include <pthread.h>   /* pthread_...() */

#include "psem.h"

#define LOOPS 10
#define NTHREADS 3
#define MAX_SLEEP_TIME 3

// Declare global semaphore variables. Note, they must be initialized before use.
psem_t *semA;
psem_t *semB;

/* TODO: Make the two threads perform their iterations in lockstep. */

void *
threadA(void *param __attribute__((unused)))
{
    int i;

    for (i = 0; i < LOOPS; i++) {
        psem_wait(semA);
        printf("A%d\n", i); //perform A:s work
        psem_signal(semB);
        //psem_wait(semA);
        sleep(rand() % MAX_SLEEP_TIME);
        //psem_signal(semA);
    }

    pthread_exit(0);
}

/* TODO: Make the two threads perform their iterations in lockstep. */

void *
threadB(void *param  __attribute__((unused)))
{
    int i;

    for (i = 0; i < LOOPS; i++) {
        psem_wait(semB);
        printf("B%d\n", i); //perform B:s work
        psem_signal(semA);
        //psem_wait(semB);
        sleep(rand() % MAX_SLEEP_TIME);
        //psem_signal(semB);
    }

    pthread_exit(0);
}

int
main()
{
    pthread_t tidA, tidB;

    // Todo: Initialize semaphores.

    semA = psem_init(1);
    semB = psem_init(1);

    srand(time(NULL));
    pthread_setconcurrency(3);

    if (pthread_create(&tidA, NULL, threadA, NULL) ||
        pthread_create(&tidB, NULL, threadB, NULL)) {
        perror("pthread_create");
        abort();
    }
    if (pthread_join(tidA, NULL) != 0 ||
        pthread_join(tidB, NULL) != 0) {
        perror("pthread_join");
        abort();
    }

    // Todo: Destroy semaphores.
    psem_destroy(semA);
    psem_destroy(semB);

    return 0;
}