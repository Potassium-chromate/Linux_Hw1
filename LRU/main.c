# include "LRU.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void generate_random_data(int *keys, int n, int max_key) {
    for (int i = 0; i < n; i++) {
        keys[i] = rand() % max_key;
    }
}

void test_performance(LRUCache *cache, int *keys, int n) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        lRUCachePut(cache, keys[i], i);
        lRUCacheGet(cache, keys[i]);
    }
    clock_t end = clock();
    printf("Time taken: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
}

int main() {
    const int capacity = 1000000;
    const int data_size = 1000000;
    const int max_key = 1000000;

    int *keys = malloc(data_size * sizeof(int));
    generate_random_data(keys, data_size, max_key);

    //multiplicative default
    LRUCache *cache2 = lRUCacheCreate(capacity, "default");
    if(cache2){
	    test_performance(cache2, keys, data_size);
	    lRUCacheFree(cache2);
	}
    free(keys);
    return 0;
}

