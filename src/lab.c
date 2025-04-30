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
 * 
 * Claude LLM was used to generate comments, and functon headers and fixing some of my 
 * poor namiong conventions.
 * 
 * Copilot/IDE suggestions were also used to complete some of the code.
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
    //validate input
    if (capacity <= 0) {
        return NULL;
    }
    //allocate memory for the queue
    queue_t q = (queue_t)malloc(sizeof(struct queue));
    if (!q) {
        return NULL;
    }
    //allocate memory for the buffer
    q->buffer = (void **)malloc(sizeof(void *) * capacity);
    if (!q->buffer) {
        free(q);
        return NULL;
    }
    //initialize the queue
    q->capacity = capacity;
    q->size = 0;
    q->head = 0;
    q->tail = 0;
    q->shutdown = false;
    //initialize the mutex and condition variables
    if (pthread_mutex_init(&q->lock, NULL) != 0) {
        free(q->buffer);
        free(q);
        return NULL;
    }

    if (pthread_cond_init(&q->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&q->lock);
        free(q->buffer);
        free(q);
        return NULL;
    }
    if (pthread_cond_init(&q->not_full, NULL) != 0) {
        pthread_mutex_destroy(&q->lock);
        pthread_cond_destroy(&q->not_empty);
        free(q->buffer);
        free(q);
        return NULL;
    }
    //return the initialized queue
    return q;
}

/**
 * Destroy the queue
 * @param q The queue to destroy
 */
void queue_destroy(queue_t q) {
    if (!q || q==NULL) {
        return;
    }
    //lock the mutex to safely destroy the queue
    pthread_mutex_lock(&q->lock);
    //free the buffer
    free(q->buffer);
    //destroy the mutex and condition variables
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    //free the queue structure
    free(q);
    //unlock the mutex
    pthread_mutex_unlock(&q->lock);
    //return
    return;
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
    //lock the mutex to safely modify the queue
    pthread_mutex_lock(&q->lock);
    //check if the queue is full
    while (q->size == q->capacity && !q->shutdown) {
        //wait for the queue to be not full
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    //check if the queue is shutdown
    if (q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return;
    }
    //enqueue the element
    q->buffer[q->tail] = data;
    q->tail = (q->tail + 1) % q->capacity; //move the tail pointer
    q->size++; //increase the size of the queue
    //signal that the queue is not empty
    pthread_cond_signal(&q->not_empty);
    //unlock the mutex
    pthread_mutex_unlock(&q->lock);
    //return the data
    return;
}

/**
 * Dequeue an element from the queue
 * @param q The queue to dequeue from
 * @return The dequeued element
 */
void *dequeue(queue_t q) {
    //validate input
    if (!q || q==NULL) {
        return NULL;
    }

    //lock the mutex to safely modify the queue
    pthread_mutex_lock(&q->lock);

    //check if the queue is empty
    while (q->size == 0 && !q->shutdown) {
        //wait for the queue to be not empty
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    //check if the queue is shutdown
    if (q->shutdown && q->size == 0) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    //dequeue the element
    void *data = q->buffer[q->head];
    q->buffer[q->head] = NULL; //clear the buffer
    q->head = (q->head + 1) % q->capacity; //move the head pointer
    q->size--; //decrease the size of the queue

    //signal that the queue is not full
    pthread_cond_signal(&q->not_full);

    //unlock the mutex
    pthread_mutex_unlock(&q->lock);

    //return the dequeued element
    return data;
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
    //validate input
    if (!q || q==NULL) {
        return false;
    }
    //lock the mutex to safely check the queue
    pthread_mutex_lock(&q->lock);
    //check if the queue is empty
    if (q->size == 0) {
        pthread_mutex_unlock(&q->lock);
        return true;
    }
    //unlock the mutex
    pthread_mutex_unlock(&q->lock);
    //if the queue is not empty, return false
    return false;
}

/**
 * Check if the queue is full
 * @param q The queue to check
 * @return true if the queue is shutdown, false otherwise
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
        return q->shutdown;
    }

    //unlock the mutex
    pthread_mutex_unlock(&q->lock);
    //if the queue is not shutdown, return false
    return false;
}