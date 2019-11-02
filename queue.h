#ifndef A6_QUEUE_H
#define A6_QUEUE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>

TAILQ_HEAD(tailhead, entry) head;

typedef struct {
  ssize_t tattoo_nr;
  ssize_t* got_tattoo;
  //STUDENT TODO:
  // * insert any members you may need here
ssize_t id_‚;
  //END STUDENT TODO
} Tattoo;


struct entry {
  Tattoo tattoo;

  TAILQ_ENTRY(entry) entries;
};

void queue_init();
void queue_push_back(Tattoo tattoo);
bool queue_pop_front(Tattoo* tattoo);

#endif //A6_QUEUE_H
