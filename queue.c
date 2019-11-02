#include "queue.h"
#include <stdio.h>
/*
 * Implementation of a single queue, you probably don't need to touch this file!
 * Warning: Not thread-safe at all!
 */

void queue_init() {
  TAILQ_INIT(&head);
}

void queue_push_back(Tattoo tattoo) {
  struct entry *elem;
  elem = malloc(sizeof(struct entry));
  if (elem) {
    elem->tattoo = tattoo;
  }
  TAILQ_INSERT_TAIL(&head, elem, entries);
}

bool queue_pop_front(Tattoo* tattoo) {
  struct entry *elem = head.tqh_first;
  if(!elem)
    return false;
  TAILQ_REMOVE(&head, head.tqh_first, entries);
  *tattoo = elem->tattoo;
  free(elem);
  return true;
}
