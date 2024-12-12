# include "find_nth_bit.h"
#include <stdio.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int main() {
    unsigned long bitmap[] = {0b10010000, 0b00110011};
    printf("%ld\n",bitmap[0]);
    printf("%ld\n",bitmap[1]);
    
    unsigned long size = ARRAY_SIZE(bitmap) * BITS_PER_LONG;
    printf("Bits for long: %d\n", BITS_PER_LONG);
    printf("The size is %ld\n", size);
    
    unsigned long bit_to_find = 3; // Find the 4th set bit (0-indexed)

    unsigned long position = find_nth_bit(bitmap, size, bit_to_find);

    if (position < size) {
        printf("The %lu-th set bit is at position: %lu\n", bit_to_find, position);
    } else {
        printf("The %lu-th set bit does not exist.\n", bit_to_find);
    }

    return 0;
}

