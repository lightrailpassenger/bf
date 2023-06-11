#ifndef BF_TAPE_H
#define BF_TAPE_H

#include <stddef.h>

struct _tape {
    size_t pointer;
    size_t allocated_size;
    size_t max_visited;
    unsigned char* buffer;
};

typedef struct _tape tape_t;

int tape_init(tape_t* t);
void tape_free(tape_t* t);
tape_t* tape_move_right(tape_t* t);
int tape_move_left(tape_t* t);

unsigned char tape_get_value(tape_t* t);
void tape_set_value(tape_t* t, unsigned char value);
void tape_set_pointer(tape_t* t, size_t pointer);
void tape_inc(tape_t* t);
void tape_dec(tape_t* t);

#endif
