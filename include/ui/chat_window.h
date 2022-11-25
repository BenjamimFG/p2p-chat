#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <ncurses.h>
#include <pthread.h>

#include "p2p_chat.h"
#include "queue.h"
#include "utils/queue.h"

// ncurses Color Pairs that will be initialized in start_chat function
typedef enum color_pairs {
  SELF_USERNAME_PAIR = 1,
  SYSTEM_USERNAME_PAIR,
  DEFAULT_USERNAME_PAIR
} ColorPairs;

/**
 * Function that starts the chat window after connecting to a peer.
 * 
 * @param message_queue Pointer to the queue that holds messages to be printed to the chat window
 * @param peer Peer struct of the connected peer
*/
void start_chat(Queue* restrict message_queue, const Peer* restrict peer);

#endif