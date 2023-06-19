#include <stdlib.h>
#include <unistd.h>
#include "exit_code.h"
#include "interpret.h"
#include "tape.h"

typedef struct size_t_node {
    struct size_t_node* next;
    size_t value;
} size_t_node;

typedef struct size_t_stack {
    size_t_node* first;
} size_t_stack;

int push_stack(size_t_stack* s, size_t value) {
    size_t_node* node = malloc(sizeof(size_t_node));

    if (node) {
        node->value = value;
        node->next = NULL;
    } else {
        return EXIT_MEM_ALLOC_FAILED;
    }

    if (s->first == NULL) {
        s->first = node;
    } else {
        node->next = s->first;
        s->first = node;
    }

    return 0;
}

int is_stack_empty(size_t_stack* s) {
    return s->first == NULL;
}

size_t pop_stack(size_t_stack* s) {
    size_t_node* second = s->first->next;
    size_t value = s->first->value;

    free(s->first);
    s->first = second;

    return value;
}

typedef struct size_t_entry {
    size_t open;
    size_t close;
} size_t_entry;

int interpret(char* str, size_t size) {
    char* reduced = malloc(sizeof(char) * size);

    if (!reduced) {
        return EXIT_MEM_ALLOC_FAILED;
    }

    size_t assigned = 0;
    size_t_stack open_bracket_stack;
    open_bracket_stack.first = NULL;

    size_t bracket_lookup_size = 16;
    size_t bracket_lookup_used = 0;
    size_t_entry* bracket_lookup = malloc(sizeof(size_t_entry) * bracket_lookup_size);

    if (!bracket_lookup) {
        free(reduced);

        return EXIT_MEM_ALLOC_FAILED;
    }

    for (size_t i = 0; i < size; i++) {
        char operator = str[i];

        switch (operator) {
            case '+':
            case '-':
            case '>':
            case '<':
            case ',':
            case '.':
                reduced[assigned] = operator;
                assigned++;
                break;
            case '[':
                reduced[assigned] = operator;
                int failed = push_stack(&open_bracket_stack, assigned++);

                if (failed) {
                    free(bracket_lookup);
                    free(reduced);

                    return failed;
                }

                break;
            case ']':
                reduced[assigned] = operator;
                int is_empty = is_stack_empty(&open_bracket_stack);

                if (is_empty) {
                    free(bracket_lookup);
                    free(reduced);

                    return EXIT_BRACKETS_MISMATCH;
                }

                size_t open = pop_stack(&open_bracket_stack);

                if (bracket_lookup_size == bracket_lookup_used) {
                    void* reallocated = realloc(bracket_lookup, sizeof(size_t_entry) * 2 * bracket_lookup_size);

                    if (!reallocated) {
                        free(bracket_lookup);
                        free(reduced);

                        return EXIT_MEM_ALLOC_FAILED;
                    }

                    bracket_lookup = reallocated;
                    bracket_lookup_size *= 2;
                }

                size_t_entry entry;
                entry.open = open;
                entry.close = assigned++;

                bracket_lookup[bracket_lookup_used++] = entry;
                break;
            default:
                break;
        }
    }

    tape_t tape;
    int failed = tape_init(&tape);

    if (failed) {
        free(bracket_lookup);
        free(reduced);

        return failed;
    }

    size_t operator_at = 0;

    while (operator_at < assigned) {
        switch (reduced[operator_at]) {
            case '+':
                tape_inc(&tape);
                break;
            case '-':
                tape_dec(&tape);
                break;
            case '>': {
                int failed = tape_move_right(&tape);

                if (failed) {
                    free(bracket_lookup);
                    tape_free(&tape);
                    free(reduced);

                    return failed;
                }
                break;
            }
            case '<': {
                int failed = tape_move_left(&tape);

                if (failed) {
                    free(bracket_lookup);
                    tape_free(&tape);
                    free(reduced);

                    return failed;
                }
                break;
            }
            case ',': {
                unsigned char c;
                size_t count = read(0, &c, 1);

                if (count) {
                    tape_set_value(&tape, c);
                } else {
                    free(bracket_lookup);
                    tape_free(&tape);
                    free(reduced);

                    return EXIT_READ_STDIN_FAILED;
                }
                break;
            }
            case '.': {
                unsigned char c = tape_get_value(&tape);

                write(1, &c, sizeof(unsigned char));
                break;
            }
            case '[': {
                unsigned char c = tape_get_value(&tape);

                if (c == 0) {
                    // jump
                    size_t next_operator_at = operator_at;

                    for (size_t i = 0; i < bracket_lookup_used; i++) {
                        if (bracket_lookup[i].open == operator_at) {
                            next_operator_at = bracket_lookup[i].close;
                            break;
                        }
                    }

                    if (next_operator_at != operator_at) {
                        operator_at = next_operator_at;
                    } else {
                        free(bracket_lookup);
                        tape_free(&tape);
                        free(reduced);

                        return EXIT_INTERNAL_SERVER_ERROR;
                    }
                }

                break;
            }
            case ']': {
                unsigned char c = tape_get_value(&tape);

                if (c) {
                    size_t next_operator_at = operator_at;

                    for (size_t i = 0; i < bracket_lookup_used; i++) {
                        if (bracket_lookup[i].close == operator_at) {
                            next_operator_at = bracket_lookup[i].open;
                            break;
                        }
                    }

                    if (next_operator_at != operator_at) {
                        operator_at = next_operator_at;
                    } else {
                        free(bracket_lookup);
                        tape_free(&tape);
                        free(reduced);

                        return EXIT_INTERNAL_SERVER_ERROR;
                    }
                }

                break;
            }
        }
        operator_at++;
    }

    return EXIT_SUCCESS;
}
