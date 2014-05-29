#ifndef UTIL_H
#define UTIL_H

namespace Utils {
#define max(a, b) (a > b) ? a : b
#define min(a, b) (a < b) ? a : b
    bool isNumber(char *number);
    unsigned int countDigits(unsigned int number);

    inline bool isNumberPowerOfTwo(unsigned int number) {
        return (number & (number-1)) == 0;
    }
}

#endif // UTIL_H
