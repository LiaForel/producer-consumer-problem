//
//  pandc.c
//  
//
//  Created by Bakulia Kurmant on 5/8/19.
//

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <dispatch/dispatch.h>
#include <semaphore.h>

typedef struct {
    pthread_mutex_t lock;
    sem_t semFull;
    sem_t semEmpty;
    int *items;
    int size;
    int in;
    int out;
} queue;

typedef struct {
    int threadid;
    int consume;
    int wait_each;
    queue *q;
    int *consumer_array;
} consumer_params;

typedef struct {
    int threadid;
    int produce;
    int wait_each;
    queue *q;
    int *producer_array;
} producer_params;

int get_next_product() {
    static int counter = 0;
    static pthread_mutex_t lock;
    int result = 0;
    pthread_mutex_lock(&lock);
    result = counter++;
    pthread_mutex_unlock(&lock);
    return result;
}

int enqueue_item(int item){
    pthread_mutex_lock(q->lock);
    while(q-<in == q->out){
        continue;
    }
    pthread_mutex_unlock(q->lock);
    return 0;
}

int dequeue_item(queue *q){
    pthread_mutex_lock(q->lock);
    // It's empty
    while (q->in == q->out) {
        continue;
    }
    pthread_mutex_unlock(q->lock);
    return 0;
}

void *producer(producer_params *params){
    while(params->produce--) {
        sleep(params->wait_each);
        sem_wait(&params->q->semFull);
        pthread_mutex_lock(&q->lock);
        int product = get_next_product();
        enqueue_item(product);
        printf("%d Was produced by Producer: \t%d\n", product, params->threadid);
        pthread_mutex_unlock(*q->unlock);
        sem_post(&params->q->semEmpty);
    }
    free(params);
    return 0;
}

void *consumer(consumer_params *params){
    while(params->consume--) {
        sleep(params->wait_each);
        sem_wait(&params->q->semEmpty);
        pthread_mutex_lock(&q->lock);
        int product = dequeue_item();
        printf("%d Was consumed by Consumer: \t%d\n", product, params->threadid);
        pthread_mutex_unlock(*q->unlock);
        sem_post(&params->q->semFull);
    }
    free(params);
    return 0;
}

queue*
queue_new(int size) {
    queue* q = malloc(sizeof(queue));
    if(pthread_mutex_init(&q->lock, NULL) != 0){
        free(q);
        return NULL;
    }
    q->items = malloc(sizeof(int) * size);
    q->size = size;
    sem_init(&q->semFull, 0, size);
    sem_init(&q->semEmpty, 0, 0;
             q->in = 0;
             q->out = 0;
             return q;
             }
             
             void
             queue_destroy(queue *q) {
                 pthread_mutex_destroy(&q->lock);
             }
             
             int main(int argc, char* argv[]) {
                 if(argc == 7){
                     
                     int size = atoi(argv[1]);
                     queue *q = queue_new(size);
                     if (!q) {
                         printf("Unable to create queue\n");
                         return -1;
                     }
                     
                     //initialize command line values
                     int numProducer = atoi(argv[2]);
                     int numConsumer = atoi(argv[3]);
                     int totalAmount = atoi(argv[4]);
                     int Ptime = atoi(argv[5]);
                     int Ctime = atoi(argv[6]);
                     
                     int *producerArray = malloc(sizeof(int) * (totalAmount * numProducer));
                     int *consumerArray = malloc(sizeof(int) * (totalAmount * numProducer));
                     
                     int overConsume = ((numProducer * totalAmount)%numConsumer);
                     int amountConsume = (numProducer * totalAmount)/numConsumer;
                     
                     // capture start time
                     time_t startTime;
                     time(&startTime);
                     
                     printf("Current time: %ld \n", startTime);
                     printf("Number of Buffers: %s\n", argv[1]);
                     printf("Number of Producers: %s\n", argv[2]);
                     printf("Number of Consumers: %s\n", argv[3]);
                     printf("Number of items Produced by each producer :%s\n", argv[4]);
                     printf("Number of items consumed by each consumer :%d\n", amountConsume);
                     printf("Over consume on?: %d\n", (overConsume > 0) ? 1 : 0);
                     printf("Over consume amount: %d\n", overConsume);
                     printf("Time each Producer Sleeps (seconds): %s\n", argv[5]);
                     printf("Time each Consumer Sleeps (seconds): %s\n", argv[6]);
                     
                     //create producer and consumer threads
                     pthread_t PThreads[numProducer];
                     pthread_t CThreads[numConsumer];
                     
                     for(int i=0; i < numConsumer; i++){
                         consumer_params* params = malloc(sizeof(consumer_params));
                         params->threadid = i;
                         params->consume = amountConsume; // figure this out
                         params->wait_each = Ctime;
                         params->q = q;
                         params->consumer_array = consumerArray;
                         if (overConsume > 0) {
                             params->consume++;
                             overConsume--;
                         }
                         pthread_create(&CThreads[i], NULL, &consumer, (void*)params);
                     }
                     
                     for(int i = 0; i < numProducer; i++){
                         producer_params* params = malloc(sizeof(producer_params));
                         params->threadid = i;
                         params->produce = 0; // figure this out
                         params->wait_each = Ptime;
                         params->q = q;
                         params->producer_array = producerArray;
                         pthread_create(&PThreads[i], NULL, &producer, (void*)params);
                     }
                     
                     //join threads
                     for(int i=0; i < numProducer; i++){
                         pthread_join(PThreads[i], NULL);
                         printf("Producer thread joined: %d\n", i);
                     }
                     for(int i=0; i < numConsumer; i++){
                         pthread_join(CThreads[i], NULL);
                         printf("Consumer thread joined: %d\n", i);
                     }
                     
                     time_t endTime = time(0);
                     time_t exeTime = endTime - startTime;
                     printf("Current time: %ld\n", exeTime);
                     
                     free(producerArray);
                     free(consumerArray);
                     queue_destroy(q);
                 }
                 return 0;
             }
