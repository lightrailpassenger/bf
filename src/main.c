#include <string.h>
#include "interpret.h"

int main(int argc, char* argv[]) {
    if (argc == 2) {
        char* arg = argv[1];

        return interpret(arg, strlen(arg));
    }

    return 99;
}
