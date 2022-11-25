#include <locale.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat_window.h"
#include "p2p_chat.h"
#include "queue.h"
#include "sleep.h"
#include "string_utils.h"
#include "time_utils.h"
#include "window_utils.h"

// Arguments for the message_dequeuer thread function
typedef struct dequeuer_args {
  Queue* message_queue;
  WINDOW* chat_win;
  WINDOW* input_win;
  int max_messages;
  pthread_mutex_t* cursor_mutex;
} DequeuerArgs;

/**
 * Draws the input box window at input_win pointer.
 * 
 * @param input_win Pointer to the ncurses WINDOW to draw to
*/
void draw_input_box(WINDOW* input_win) {
  box(input_win, 0, 0);

  int max_x = getmaxx(input_win);

  // print '├' and '┤' characters to the corners of input box to make it appear
  // it's connected to the chat window on top of it
  mvwprintw(input_win, 0, 0, "├");
  mvwprintw(input_win, 0, max_x - 1, "┤");
  // print a "> " prompt for the user to type at
  mvwprintw(input_win, 1, 2, "> ");

  wrefresh(input_win);
}

/**
 * Clears the input box window at input_win.
 * 
 * @param input_win Pointer to the ncurses WINDOW of the input_box
*/
void clear_input_box(WINDOW* input_win) {
  int max_x = getmaxx(input_win);

  char* blank_line = char_repeat(' ', max_x - 5);

  mvwprintw(input_win, 1, 4, blank_line);

  // move cursor back to beginning of input box after the "> " prompt
  wmove(input_win, 1, 4);

  free(blank_line);
}

/**
 * Draws the chat box in chat_win window with a title.
 * 
 * @param chat_win Pointer to the ncurses WINDOW to draw to
 * @param title Title string to be added to the top of the window
*/
void draw_chat_box(WINDOW* chat_win, const char* title) {
  box(chat_win, 0, 0);

  add_title(chat_win, title);

  wrefresh(chat_win);
}

/**
 * Clears the chat_win window
 * 
 * @param chat_win Pointer to the window to be cleared
*/
void clear_chat_box(WINDOW* chat_win) {
  int y, x;
  getmaxyx(chat_win, y, x);

  char* blank_line = char_repeat(' ', x - 2);

  for (int i = 1; i < y - 1; ++i) {
    mvwprintw(chat_win, i, 1, blank_line);
  }

  free(blank_line);
}

/**
 * Adds msg to the messages array. This function changes the content of cur_messages pointer
 * if the number of cur_messages is changed. The max_messages variable is used to determine if the
 * the message can just be appended to the array or if messages should be shifted (causing a scroll
 * effect) to allow the new message to be appended.
 * 
 * @param messages Array of messages to add a message to
 * @param msg Pointer to the Message struct to be atted to the messages array
 * @param cur_messages Current number of messages in the messages array
 * @param max_messages Max number of messages in the messages array
*/
void add_message(Message* restrict messages, const Message* restrict msg, int* restrict cur_messages, const int max_messages) {
  // if there's space in the array simply append the message and return
  if (*cur_messages < max_messages) {
    messages[*cur_messages] = *msg;
    *cur_messages += 1;
    return;
  }

  // array is full so shift all elements to the left before appending
  for (int i = 0; i < max_messages - 1; ++i) {
    messages[i] = messages[i + 1];
  }

  messages[max_messages - 1] = *msg;
}

/**
 * Prints the messages from messages list to the chat_win window.
 * 
 * @param chat_win Pointer to the ncurses WINDOW to print the messages to
 * @param messages Array that contains the messages to be printed
 * @param cur_messages Number of messages in the messages array
*/
void print_messages(WINDOW* chat_win, Message* messages, int cur_messages) {
  clear_chat_box(chat_win);

  for (int i = 0; i < cur_messages; ++i) {
    Message msg = messages[i];
    mvwprintw(chat_win, 1 + i, 1, "[%s] ", msg.time_str);

    // activate color attribute for the username only an then turn it off
    wattron(chat_win, COLOR_PAIR(msg.username_color));
    wprintw(chat_win, "%s", msg.username);
    wattroff(chat_win, COLOR_PAIR(msg.username_color));
    
    wprintw(chat_win, ": %s", msg.content);
  }

  wrefresh(chat_win);
}

/**
 * Creates help messages and add them to the message queue
 * 
 * @param message_queue Pointer to the message_queue to add help messages to
*/
void add_help_messages_to_queue(Queue* message_queue) {
  char help_msgs[2][MAX_MESSAGE_SIZE + 1] = {
    "/disconnect - Exits this chat",
    "/help - Shows this help text"
  };

  for (int i = 0; i < 2; ++i) {
    Message* help_msg = (Message*) malloc(sizeof(Message));
    memset(help_msg, 0, sizeof(Message));
    
    strncpy(help_msg->username, "System", 7);
    help_msg->username_color = SYSTEM_USERNAME_PAIR;

    strncpy(help_msg->content, help_msgs[i], strlen(help_msgs[i]));
    get_time_str(help_msg->time_str);

    queue_add(message_queue, (void*) help_msg);
  }
}

/**
 * Thread function that dequeues messages and then prints then to chat window.
 * 
 * @param args Arguments of type DequeuerArgs*
 * 
 * @returns NULL
*/
void* message_dequeuer(void* args) {
  DequeuerArgs t_args = *(DequeuerArgs*) args;
  
  // array of Messages that are currently displayed in the chat window
  Message* displayed_messages = (Message*) malloc(t_args.max_messages * sizeof(Message));
  int cur_messages = 0;

  // thread loop
  while(true) {
    if (t_args.message_queue->size == 0) {
      // if the queue is empty wait 100ms to avoid unnecessary spike in CPU usage
      ms_sleep(100);
      continue;
    }

    Message* message = (Message*) queue_dequeue(t_args.message_queue);

    add_message(displayed_messages, message, &cur_messages, t_args.max_messages);

    // message's  content was added to the array so it can be freed
    free(message);

    // save previous position y, x of cursor before printing messages
    int prev_y, prev_x;
    getyx(t_args.input_win, prev_y, prev_x);

    // acquire cursor mutex before printing messages to avoid multiple threads
    // moving the cursor at the same time
    pthread_mutex_lock(t_args.cursor_mutex);
    print_messages(t_args.chat_win, displayed_messages, cur_messages);

    // move cursor back to input box
    wmove(t_args.input_win, prev_y, prev_x);
    wrefresh(t_args.input_win);
    pthread_mutex_unlock(t_args.cursor_mutex);
  }
}

void start_chat(Queue* restrict message_queue, const Peer* restrict peer) {
  // initialize ncurses colors
  start_color();

  init_pair(SELF_USERNAME_PAIR, COLOR_GREEN, COLOR_BLACK);
  init_pair(SYSTEM_USERNAME_PAIR, COLOR_RED, COLOR_BLACK);
  init_pair(DEFAULT_USERNAME_PAIR, COLOR_WHITE, COLOR_BLACK);

  int maxy, maxx;
  getmaxyx(stdscr, maxy, maxx);

  // create chat_win (part of chat where messages will appear) and input_win (user's input box)
  WINDOW* chat_win = newwin(maxy - 2, maxx, 0, 0);
  WINDOW* input_win = newwin(3, maxx, maxy-3, 0);

  refresh();

  // define max number of messages on chat_win, used to know when to scroll when printing messages
  int max_messages = getmaxy(chat_win) - 2;

  char chat_title[16 + MAX_USERNAME_SIZE];
  snprintf(chat_title, 16 + MAX_USERNAME_SIZE, "Chatting with: %s", peer->username);

  draw_chat_box(chat_win, chat_title);
  draw_input_box(input_win);

  // cursor_mutex is used to determine which thread should be able to use ncurses cursor to
  // avoid 2 threads (main thread handles input box and dequeuer thread handles chat_win)
  // moving the cursor at the same time
  pthread_mutex_t cursor_mutex;
  pthread_mutex_init(&cursor_mutex, NULL);

  pthread_t dequeuer_tid;

  DequeuerArgs dequeuer_args = {
    .chat_win = chat_win,
    .input_win = input_win,
    .max_messages = max_messages,
    .message_queue = message_queue,
    .cursor_mutex = &cursor_mutex
  };

  // create the dequeuer thread, this thread will wait for messages to arrive at the queue
  // then dequeue them and print to the chat_win
  pthread_create(&dequeuer_tid, NULL, &message_dequeuer, (void*) &dequeuer_args);

  // event loop of the chat window
  while (true) {
    // lock mutex before clearing the input box, then unlock it
    pthread_mutex_lock(&cursor_mutex);
    clear_input_box(input_win);
    pthread_mutex_unlock(&cursor_mutex);

    // construct message
    Message* msg = (Message*) malloc(sizeof(Message));
    memset(msg, 0, sizeof(Message));
    
    strcpy(msg->username, "You");
    msg->username_color = SELF_USERNAME_PAIR;

    wgetnstr(input_win, msg->content, MAX_MESSAGE_SIZE);
    get_time_str(msg->time_str);

    queue_add(message_queue, (void*) msg);

    // if user's message starts with '/' treat is specially
    if (starts_with(msg->content, '/')) {
      if (strcmp(msg->content, "/help") == 0) {
        add_help_messages_to_queue(message_queue);
      }
      else if (strcmp(msg->content, "/disconnect") == 0) {
        // end the event loop if user's message is /disconnect
        break;
      }
    }
    // else just send it
    else {
      send_message(peer->fd, msg->content);
    }
  }

  pthread_cancel(dequeuer_tid);
}
