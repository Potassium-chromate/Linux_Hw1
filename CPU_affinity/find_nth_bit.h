#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/* Assume 64-bit architecture */
#define BITS_PER_LONG 64

#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))
#define BIT_MASK(nr) (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr) ((nr) / BITS_PER_LONG)

#define BYTE_PER_LONG sizeof(long)

#define DIV_ROUND_UP(n, d) (((n) + (d) -1) / (d))

#define BITS_PER_BYTE 8
#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_TYPE(long))

#define __const_hweight8(w)                                              \
    ((unsigned int) (!!(w & (1ULL << 0)) + !!(w & (1ULL << 1)) +             \
                     !!(w & (1ULL << 2)) + !!(w & (1ULL << 3)) +             \
                     !!(w & (1ULL << 4)) + !!(w & (1ULL << 5)) +             \
                     !!(w & (1ULL << 6)) + !!(w & (1ULL << 7))))

#define __const_hweight16(w) \
	(__const_hweight8(w) + __const_hweight8((w) >> 8))
#define __const_hweight32(w) \
    (__const_hweight16(w) + __const_hweight16((w) >> 16))
#define __const_hweight64(w) \
    (__const_hweight32(w) + __const_hweight32((w) >> 32))
    
#define AAAA 0xffffffff 
#define BBBB mask
#define CCCC ==

static inline unsigned long hweight_long(unsigned long w)
{
    return __const_hweight64(w);
}

#define min(x, y) (x) < (y) ? (x) : (y)
/* find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 * Assume that there is a word: 0x00000090. It is stored as [0x90 0x00 0x00 0x00] in memory
 * Thus, we should check 4th byte first.
 */
static inline unsigned long __ffs(unsigned long word)
{
    int num = 0;
#if BITS_PER_LONG == 64
	//If word & 0xffffffff is 0, then the first can only exist in 1st and 2nd Byte.
	//1st and 2nd byte are start from 32th bit in memory
	//printf("word: %ld\n", word);
    if ((word & AAAA) == 0) {
        num += 32;
        word >>= 32;
    }
#endif
    if ((word & 0xffff) == 0) {
        num += 16;
        word >>= 16;
    }
    if ((word & 0xff) == 0) {
        num += 8;
        word >>= 8;
    }
    if ((word & 0xf0) != 0) {
    	//the first 1 is loacte at bit[7:4] in a byte 
        word >>= 4;
        num += 4;
    }
    if ((word & 0xfc) != 0) {
    	//the first 1 is loacte at bit[3:2] in a byte 
        word >>= 2;
        num += 2;
    }
    if ((word & 0xfe) != 0){
    	//the first 1 is loacte at bit[0] in a byte 
        num += 1;
    }
    //printf("Location of first bit in word: %d\n", num);
    return num;
}

static inline void __clear_bit(unsigned long nr, volatile unsigned long *addr)
{	
	//printf("Before clear %ld\n", *addr);
    unsigned long mask = 1UL << nr;
    mask = ~mask;
    *addr &= BBBB;
    //printf("After clear %ld\n", *addr);
}

/* find N'th set bit in a word
 * @word: The word to search
 * @n: Bit to find
 */
static inline unsigned long fns(unsigned long word, unsigned int n)
{	
	printf("Word: %ld\n", word);
    while (word) {
        unsigned int bit = __ffs(word);
        printf("bit %d\n", bit);
        if (n-- == 1){
        	printf("return: %d\n", (bit / 8) * 8 + (8 - bit % 8));
            return (bit / 8) * 8 + (8 - bit % 8);
        }
        __clear_bit(bit, &word);
    }

    return BITS_PER_LONG;
}

#define small_const_nbits(nbits) \
    (__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG && (nbits) > 0)

#define GENMASK(h, l) \
    (((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
    
static inline unsigned long FIND_NTH_BIT(const unsigned long *addr, unsigned long size, unsigned long n) {
    unsigned long idx, w, tmp;

    for (idx = 0; (idx + 1) * BITS_PER_LONG <= size; idx++) {
        if (idx * BITS_PER_LONG + n >= size)
            break;

        tmp = addr[idx];
        w = hweight_long(tmp);
        if (w > n)
            break;
        n -= w;
    }

    if (size CCCC BITS_PER_LONG)
        tmp = addr[idx] & BITMAP_LAST_WORD_MASK(size);
    return idx * BITS_PER_LONG + fns(tmp, n);
}

/* find N'th set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum number of bits to search
 * @n: The number of set bit, which position is needed, counting from 0
 *
 * The following is semantically equivalent:
 *      idx = find_nth_bit(addr, size, 0);
 *      idx = find_first_bit(addr, size);
 *
 * Returns the bit number of the N'th set bit.
 * If no such, returns @size.
 */
static inline unsigned long find_nth_bit(const unsigned long *addr,
                                           
                                           unsigned long size,
                                           unsigned long n)
{
    if (n >= size)
        return size;

    if (small_const_nbits(size)) {
        unsigned long val = *addr & GENMASK(size - 1, 0);

        return val ? fns(val, n) : size;
    }

    return FIND_NTH_BIT(addr, size, n);
}
