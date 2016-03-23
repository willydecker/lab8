#include <stdio.h>
#include <pthread.h>

#define LOOP_LIMIT 10
#define MAXIMUM 5

// globbal vars
pthread_mutex_t mutex;
pthread_cond_t condvar;
int shared = 0;

//function prototypes
void *consumer_fun(void *);
void *producer_fun(void *);

int main(void)
{
  pthread_t consumer;
  pthread_t producer;

  pthread_mutex_init(&mutex, 0);
  pthread_cond_init(&condvar, 0);

  //create two threads
  pthread_create(&consumer, 0, consumer_fun, 0);
  pthread_create(&producer, 0, producer_fun, 0);

  //join threads when they finish
  pthread_join(consumer, 0);
  pthread_join(producer, 0);

  //destroy mutex and condition variables
  pthread_cond_destroy(&condvar);
  pthread_mutex_destroy(&mutex);

  //exit
  exit(0);
}

//Producer function
// generate a random number and write it to a shared variable
void *producer_fun(void *ptr)
{
  int number = 0; //a meaningless number
  int i = 0;

  //start thread
  printf("Producer thread starting\n\n");

  //init the pseudo-random number
  srandom((unsigned int)time(NULL));

  for (i=0; i<LOOP_LIMIT; i++)
  {
    //pick a number less than the max
    //have to add something to random to garentee randomness
  number = random() % MAXIMUM + 1;

  //critical region begin
  pthread_mutex_lock (&mutex);

  while (shared != 0)
  {
    pthread_cond_wait(&condvar, &mutex);
  }

  //this threads turn
  shared = number;
  printf("Producer storing the number %d, on loop %d.\n", shared, i);
  //signal the other thead in case its asleep
  pthread_cond_signal(&condvar);
  // done so unlock the mutex
  pthread_mutex_unlock(&mutex);
  //end crit region
  }

  pthread_exit(0);
}

//consumer_fun
void *consumer_fun(void *ptr)
{
  int i = 0;

  printf("Consumer thread starting\n");

  for(i=0; i<LOOP_LIMIT; i++)
  {
    pthread_mutex_lock(&mutex);

    while (shared == 0)
    {
      pthread_cond_wait(&condvar, &mutex);
    }
    //this threads turn
    printf("Consumer retrieving the number %d, on loop %d.\n", shared, i);
    shared = 0;
    //signal the other thread in case its asleep
    pthread_cond_signal(&condvar);
    //unlock the mutex
    pthread_mutex_unlock(&mutex);
    //end crit region
  }

  //exit
  pthread_exit(0);
}
