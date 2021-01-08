//
// Created by kameranis on 3/13/20.
//

#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sched.h>


#define ITERATIONS 10000000

#if defined(SYNC_ATOMIC) & defined(SYNC_MUTEX) == 1
#error You must #define exactly one of SYNC_ATOMIC or SYNC_MUTEX.
#endif

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

#if defined(SYNC_ATOMIC)
#define USE_ATOMIC 1
#else
#define USE_ATOMIC 0
#endif

#if defined(SYNC_MUTEX)
#define USE_MUTEX 1
#else
#define USE_MUTEX 0
#endif

int jj = 0;

void * worker_increment(void * arg) {
    volatile int *jj = (int *) arg;
    for (unsigned int i = 0; i < ITERATIONS; i++) {
        if (USE_MUTEX) {
            pthread_mutex_lock(&lock);
            ++(*jj);
            pthread_mutex_unlock(&lock);
        } else if (USE_ATOMIC) {
            __sync_fetch_and_add(jj, 1);
        } else {
            ++(*jj);
        }
    }
    return NULL;
}

void * worker_decrement(void * arg) {
    volatile int *jj = (int *) arg;
    for (unsigned int i = 0; i < ITERATIONS; i++) {
        if (USE_MUTEX) {
            int perr = pthread_mutex_lock(&lock);
            if (perr) {
                fprintf(stderr, "%s: Error acquiring lock: %s (%d)\n", __func__, strerror(perr), perr);
                return NULL;
            }
            --(*jj);
            perr = pthread_mutex_unlock(&lock);
            if (perr) {
                fprintf(stderr, "%s: Error releasing lock: %s (%d)\n", __func__, strerror(perr), perr);
                return NULL;
            }
        } else if (USE_ATOMIC) {
            __sync_fetch_and_sub(jj, 1);
        } else {
            --(*jj);
        }
    }
    return NULL;
}

int main() {
    pthread_t pthread_id[2];
    cpu_set_t cpuset;
    pthread_t self = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    CPU_SET(1, &cpuset);
    pthread_setaffinity_np(self, sizeof(cpu_set_t), &cpuset);
    int perr;
    perr = pthread_create(&pthread_id[0], NULL, worker_increment, &jj);
    if (perr) {
        fprintf(stderr, "%s: Error creating pthread: %s (%d)\n", __func__, strerror(perr), perr);
        return 1;
    }

    perr = pthread_create(&pthread_id[1], NULL, worker_decrement, &jj);
    if (perr) {
        fprintf(stderr, "%s: Error creating pthread: %s (%d)\n", __func__, strerror(perr), perr);
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(pthread_id[i], NULL);
    }
    printf("%d\n", jj);
    /*if (jj) {
        printf("Bad syncronization: jj = %d\n", jj);
    } else {
        // printf("Good syncronization: jj = %d\n", jj);
    }*/
    return 0;
}
