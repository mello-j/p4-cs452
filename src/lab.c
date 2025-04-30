#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * lab.c
 * This file contains the implementation of a thread-safe queue.
 * The queue supports basic operations such as enqueue, dequeue,
 * initialization, destruction, and shutdown.
 * It is designed to be used in a multi-threaded environment.
 * 
 */


 /**
  * queue_t
  * This structure represents a thread-safe queue.
  */
struct queue {
    void **buffer; //buffer to hold the data
    int capacity; //queue capacity
    int size; // size of the queue
    int head; //start of queue
    int tail; //end of queue
    bool shutdown; //shutdown flag
    pthread_mutex_t lock; //mutex for thread safety
    pthread_cond_t not_empty; //condition variable for not empty
    pthread_cond_t not_full; //condition variable for not full
};

/**
 * Initialize the queue 
 * @capacity The maximum number of elements the queue can hold
 * @return A pointer to the initialized queue
 */
queue_t queue_init(int capacity) {
    // TODO: Implement queue initialization
    return NULL;
}

/**
 * Destroy the queue
 * @param q The queue to destroy
 */
void queue_destroy(queue_t q) {
    if (!q || q==NULL) {
        return;
    }
}

/**
 * Enqueue an element to the queue
 * @param q The queue to enqueue to
 * @param data The data to enqueue
 */
void enqueue(queue_t q, void *data) {
    //validate input
    if (!q || q==NULL) {
        return;
    }
}

/**
 * Dequeue an element from the queue
 * @param q The queue to dequeue from
 * @return The dequeued element
 */
void *dequeue(queue_t q) {
      /validate input
    if (!q || q==NULL) {
        return;
    }

    return;
}

/**
 * Shutdown the queue
 * @param q The queue to shutdown
 */
void queue_shutdown(queue_t q) {
    //validate input
    if (!q || q==NULL) {
        return;
    }
    //lock the mutex to safely modify the queue
    pthread_mutex_lock(&q->lock);

    //set the shutdown flag to true
    q->shutdown = true;

    //signal all threads waiting on the queue
    pthread_cond_broadcast(&q->not_empty);
    pthread_cond_broadcast(&q->not_full);
    pthread_mutex_unlock(&q->lock);
}


/**
 * Check if the queue is empty
 * @param q The queue to check
 * @return true if the queue is empty, false otherwise
 */
bool is_empty(queue_t q) {
    // TODO: Implement checking if the queue is empty
    return false;
}

/**
 * Check if the queue is full
 * @param q The queue to check
 * @return true if the queue is full, false otherwise
 */
bool is_shutdown(queue_t q) {
    //validate input
    if (!q || q==NULL) {
        return false;
    }
    //lock the mutex to safely check the queue
    pthread_mutex_lock(&q->lock);
    //check if the queue is shutdown
    if (q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return true;
    }

    //unlock the mutex
    pthread_mutex_unlock(&q->lock);
    //if the queue is not shutdown, return false
    return false;
}