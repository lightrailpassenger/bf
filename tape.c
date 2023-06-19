#include <stdlib.h>

#include "exit_code.h"
#include "tape.h"

int tape_init(tape_t* t) {
    size_t initial_size = 64;
    unsigned char* buffer = malloc(sizeof(unsigned char) * initial_size);

    if (buffer) {
        buffer[0] = 0;
        t->buffer = buffer;
        t->pointer = 0;
        t->max_visited = 0;
        t->allocated_size = initial_size;

        return EXIT_SUCCESS;
    }

    return EXIT_MEM_ALLOC_FAILED;
}

void tape_free(tape_t* t) {
    free(t->buffer);
}

int tape_move_left(tape_t* t) {
    if (t->pointer == 0) {
        return EXIT_CANNOT_MOVE_LEFT;
    }

    t->pointer--;
    return EXIT_SUCCESS;
}

int tape_move_right(tape_t* t) {
    if (t->pointer == t->allocated_size - 1) {
        // reallocate
        void* reallocated = realloc(t->buffer, sizeof(unsigned char) * (t->allocated_size * 2));

        if (reallocated) {
            t->allocated_size *= 2;
            t->buffer = reallocated;
            t->pointer++;

            return EXIT_SUCCESS;
        } else {
            return EXIT_MEM_ALLOC_FAILED;
        }
    } else {
        t->pointer++;

        if (t->max_visited < t->pointer) {
            t->max_visited++;
            t->buffer[t->pointer] = 0;
        }

        return EXIT_SUCCESS;
    }

    return EXIT_MEM_ALLOC_FAILED;
}

unsigned char tape_get_value(tape_t* t) {
    return t->buffer[t->pointer];
}

void tape_set_value(tape_t* t, unsigned char value) {
    t->buffer[t->pointer] = value;
}

void tape_set_pointer(tape_t* t, size_t pointer) {
    t->pointer = pointer;
}

void tape_inc(tape_t* t) {
    t->buffer[t->pointer]++;
}

void tape_dec(tape_t* t) {
    t->buffer[t->pointer]--;
}
