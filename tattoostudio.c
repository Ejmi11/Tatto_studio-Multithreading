#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include "queue.h"

/* STUDENT TODO:
 *  * declare any global variables you need here
 */

pthread_mutex_t queue_vec_lock;
pthread_cond_t condition_variable[10000];

sem_t semap_tattooing;

#define NUM_TATTOO_GUNS 2
int tattoo_guns[NUM_TATTOO_GUNS];

const char* tattoos[] = {
    "Heart",
    "Skull",
    "Infinity Sign",
    "Swallow",
    "Tribal",
    "Black Work",
    "Tramp Stamp",
    "Roses",
    "Asian Characters",
    "8 Ball",
    "Koi Karp with Waves",
    "Dandelion",
    "Playing Cards with Fire",
    "Chinese Dragon",
    "Tiger",
    "Mandala",
    "Mexican Dead Mask",
    "Water Color",
    "Flower of Life",
};

void* customer_do(void* customerID) {
  /* STUDENT TODO:
   *  * make sure any shared resources are locked correctly!
   */
  ssize_t tattoo = rand() % (sizeof(tattoos) / sizeof(tattoos[0]));
  printf("Customer %zd want to get %s.\n", (ssize_t) customerID, tattoos[tattoo]);
  ssize_t got_tattoo = -1;
  Tattoo t = {tattoo, &got_tattoo, (ssize_t)customerID};

  pthread_mutex_lock (&queue_vec_lock);
  queue_push_back(t);
  pthread_mutex_unlock (&queue_vec_lock);

  /*STUDENT TODO:
   *  * wait for tattoo to be finished
   *  * Consider the efficiency of your solution!
   */

  pthread_mutex_lock (&queue_vec_lock); 
  pthread_cond_wait(&condition_variable[t.id_],&queue_vec_lock);
  pthread_mutex_unlock (&queue_vec_lock); 

  if( tattoo != got_tattoo) {
    if (got_tattoo < 0) {
      printf("It seems no one has tattooed customer %zd!\n", (ssize_t) customerID);
    }
    else if(got_tattoo >= sizeof(tattoos)) {
      printf("Customer %zd got a tattoo that is not on the tattoo list, how did that happen?\n", (ssize_t) customerID);
    }
    else {
      printf("Customer %zd got tattoo %s, although he wanted %s!\n", (ssize_t) customerID, tattoos[got_tattoo], tattoos[tattoo]);
    }
  }
  else {
    printf("Customer %zd likes his new tattoo %s.\n", (ssize_t) customerID, tattoos[got_tattoo]);
  }
  return NULL;
}

void* tattoo_artist_do(void *artistID) {
  while(1) {
    /* STUDENT TODO:
     * * make sure possible shared resources are locked correctly
     * * notify customer that it's tattoo is finished
     * * make sure a tattoo gun is available
     */
    Tattoo t;
    pthread_mutex_lock (&queue_vec_lock);
    bool got_work = queue_pop_front(&t);
    pthread_mutex_unlock (&queue_vec_lock);

    if(!got_work) {
      printf("Artist %zd has no work, so he takes a short nap!\n", (ssize_t) artistID);
      nanosleep((const struct timespec[]) {{0, 1000L*1000L*50L}}, NULL); //sleep for 50 ms
      continue;
    }

    ssize_t tattoo_to_poke = t.tattoo_nr;

    printf("Artist %zd wants to start tattooing %s...\n", (ssize_t) artistID, tattoos[tattoo_to_poke]);


    ssize_t microsec_to_sleep = 1000 + rand() % 9000;
    nanosleep((const struct timespec[]) {{0, 1000L* microsec_to_sleep}}, NULL);
sem_wait(&semap_tattooing);
    printf("Artist %zd is grepping a tattoo gun\n", (ssize_t) artistID);
pthread_mutex_lock(&queue_vec_lock);
    int i = 0;
    while(tattoo_guns[i] != 0)
      i++;
    tattoo_guns[i] = 1;
pthread_mutex_unlock(&queue_vec_lock);

    printf("Artist %zd got tattoo gun %d\n", (ssize_t) artistID, i);
    microsec_to_sleep = 1000 + rand() % 3000;
    nanosleep((const struct timespec[]) {{0, 1000L* microsec_to_sleep}}, NULL);
    printf("Artist %zd finished tattooing\n", (ssize_t) artistID);
    printf("Artist %zd is cleaning the tattoo gun \n", (ssize_t) artistID);
pthread_mutex_lock(&queue_vec_lock);
    tattoo_guns[i] = 0;
pthread_mutex_unlock(&queue_vec_lock);
        printf("Artist %zd done with tattoo %s.\n", (ssize_t) artistID,
           tattoos[tattoo_to_poke]);
sem_post (&semap_tattooing);
    *t.got_tattoo = tattoo_to_poke;
    pthread_mutex_lock (&queue_vec_lock); 
    pthread_cond_signal(&condition_variable[t.id_]);
    pthread_mutex_unlock (&queue_vec_lock); 
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  queue_init();
 if(argc != 3) {
    fprintf(stderr, "Usage: %s <num_artists> <num_customers>\n", argv[0]);
    exit(-1);
  }
  //parse parameters
  ssize_t num_artists = atoi(argv[1]);
  ssize_t num_customers = atoi(argv[2]);
  if(num_artists < 1 || num_artists > 50) {
    fprintf(stderr, "num_artists allowed range: [1,50].\n");
    exit(-1);
  }
  if(num_customers < 1 || num_customers > 10000) {
    fprintf(stderr, "num_customers allowed range: [1,10000].\n");
    exit(-1);
  }
  /* STUDENT TODO:
    *  * add any initialization code you may need here
    */
  pthread_mutex_init(&queue_vec_lock, NULL);
  sem_init(&semap_tattooing,0, 1);

  ssize_t c = 0;
  for(c = 0; c < 10000; c++){
    pthread_cond_init(&condition_variable[c], 0);
  }

  printf("Tattoo Studio opening!\n");
  pthread_t* artists  = malloc(num_artists*sizeof(pthread_t));
  pthread_t* customers = malloc(num_customers*sizeof(pthread_t));
  if(!customers || !artists) {
    free(customers);
    free(artists);
    fprintf(stderr, "Could not allocate memory!\n");
    exit(-1);
  }
  for(ssize_t i = 0; i < num_artists; i++) {
    pthread_create(&artists[i], NULL, tattoo_artist_do, (void*) i);
  }
  for(ssize_t i = 0; i < num_customers; i++) {
    pthread_create(&customers[i], NULL, customer_do, (void*)i);
  }

  for(ssize_t i = 0; i < num_customers; i++) {
    pthread_join(customers[i], NULL);
  }

  printf("All customers tattooed, telling artists to go home...\n");
  for(ssize_t i = 0; i < num_artists; i++) {
    pthread_cancel(artists[i]);
    pthread_join(artists[i], NULL);
  }

  printf("Tattoo Studio closed!\n");

  pthread_mutex_destroy(&queue_vec_lock);

  ssize_t i = 0;
  for(i = 0; i < 10000; i++)
    pthread_cond_destroy(&condition_variable[i]);

  sem_destroy(&semap_tattooing);

  free(customers);
  free(artists);

  return 0;
}