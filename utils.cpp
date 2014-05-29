#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace Utils;

unsigned int countDigits(int number) {
    if(number < 0)
        number *= -1;

    if (number < 10)
        return 1;

    return (1 + countDigits(number / 10));
}

// TODO: isNumber only works with positive numbers
bool isNumber(char *number) {
    return strlen(number) == countDigits(atoi(number));
}
