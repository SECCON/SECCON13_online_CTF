#include <stdio.h>
#include <stdbool.h>
#define FLAG_LEN 32

typedef enum { INIT, LOOP, CHECK, DONE, SUCCESS, FAILURE } State;
State state;
bool correct = true;
int idx;



void init() {
    idx = 0;
    state = LOOP;
}

void loop() {
    if (idx < FLAG_LEN) {
        state = CHECK;
    } else {
        state = DONE;
    }
}

void bb(int input) {
    correct &= ((input ^ 0xcafebabe) == 0xf9958ed6);
}

void jj(int input) {
    correct &= ((input ^ 0xc0ffee) == 0x5fb4ceb1);
}

void ss(int input) {
    correct &= ((input ^ 0xdeadbeef) == 0xebd6f0a0);
}

void uu(char *input) {
    int current = *(int*)(input + idx);
    int before = *(int*)(input + idx - 4);
    correct &= ((current + before) == 0x9d9d6295);
}

void kk(char *input) {
    int current = *(int*)(input + idx);
    int before = *(int*)(input + idx - 4);
    correct &= ((current + before) == 0x94d3a1d4);
}

void mm(char *input) {
    int current = *(int*)(input + idx);
    int before = *(int*)(input + idx - 4);
    correct &= ((current - before) == 0x47cb363b);
}

void cc(int input) {
    correct &= (input == 0x43434553);
}

void oo(char *input) {
    int current = *(int*)(input + idx);
    int before = *(int*)(input + idx - 4);
    correct &= ((current + before) == 0x9d949ddd);
}

void check(char *input) {
    state = LOOP;
    switch (idx) {
        case 24:
            uu(input);
            return;
        case 4:
            ss(*(int*)(input + idx));
            return;
        case 8:
            bb(*(int*)(input + idx));
            return;
        case 20:
            oo(input);
            return;
        case 0:
            cc(*(int*)(input));
            return;
        case 12:
            jj(*(int*)(input + idx));
        case 28:
            mm(input);
            return;
        case 16:
            kk(input);
            return;
    }
}

int checker(char *input) {
    state = INIT;

    while (1) {
        switch (state) {
            case INIT:
            case LOOP:
            case CHECK:
            {
                void (*vv[3])() = {[INIT]=init, [LOOP]=loop, [CHECK]=check};
                vv[state](input);
                idx += 4;
                break;
            }
            case DONE:
                if (correct) {
                    state = SUCCESS;
                } else {
                    state = FAILURE;
                }
                break;
            case SUCCESS:
                return 1;

            case FAILURE:
                return 0;
        }
    }
}

int main(int argc, char **argv) {
    if (argc == 2 && checker(argv[1]) == 1) {
        puts("Correct");
    } else {
        puts("Incorrect");
    }
}
