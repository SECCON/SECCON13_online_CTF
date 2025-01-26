#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define FLAG_SIZE 72

//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...) printf(fmt, ##args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

uint64_t arr[] = {
    2464019852476686830ull, 13978386934984593338ull, 13262146072703018499ull, 6964922062865936270ull,
    11710320807250859480ull, 18303995074085096850ull, 494768862820948253ull, 17166728605938654290ull,
    8051064940365832844ull, 1847915330233600553ull, 5643677552456032650ull, 13665583038990865920ull,
    4839177420633975264ull, 10535905440399232926ull, 339265043980492385ull, 13868161547447698995ull,
    6660559199040787302ull, 11120572917179246126ull, 3629644673142818946ull, 15550631430533118795ull,
    4365940597900273724ull, 492039240815247952ull, 544172886700752627ull, 14510165407499250930ull,
    15706932369075344385ull, 13794107422021936180ull, 2187126539960487763ull, 28715642332991504ull,
    11148293691496164660ull,
};


bool check_input(char *input) {
    int len = strlen(input);
    if (len != FLAG_SIZE) {
        return false;
    }

    if (strncmp(input, "SECCON{", 7) != 0) {
        return false;
    }

    for (int i = 7; i < len - 1; i++) {
        if (!(input[i] >= '0' && input[i] <= '9') && !(input[i] >= 'a' && input[i] <= 'f')) {
            return false;
        }
    }

    return true;
}

bool check(char *input) {
    if (check_input(input) == false) {
        return false;
    }

    uint32_t *winput = (uint32_t*)input;
    uint64_t *qinput = (uint64_t*)input;
    uint64_t result = 0;
    for (int i = 0; i < FLAG_SIZE / 8; i++) {
        uint64_t tmp, tmp2, tmp3;
        uint64_t cmp;

        asm("seccon0 %0, %1" : "=r"(tmp) : "r"(qinput[i]));
        DEBUG_PRINT("seccon0(0x%lx) = 0x%lx\n", qinput[i], tmp);

        asm("seccon1 %0, %1, %2" : "=r"(tmp2) : "r"(tmp), "r"(qinput[i]));
        DEBUG_PRINT("seccon1(0x%lx, 0x%lx) = 0x%lx\n", tmp, qinput[i], tmp2);

        asm("seccon0 %0, %1" : "=r"(tmp3) : "r"(tmp2));
        DEBUG_PRINT("seccon0(0x%lx) = 0x%lx\n", tmp2, tmp3);

        asm("seccon2 %0, %1, %2" : "=r"(cmp) : "r"(tmp3), "r"(i));
        DEBUG_PRINT("seccon2(0x%lx, 0x%lx) = 0x%lx\n", tmp3, i, cmp);

        result |= cmp;
    }

    return result == 0;
}

int main(void){
    char input[256];

    printf("Input flag: ");
    scanf("%200s", input);

    if (check(input)) {
        printf("Correct! The flag is %s\n", input);
    } else {
        printf("Wrong...\n");
    }
}
