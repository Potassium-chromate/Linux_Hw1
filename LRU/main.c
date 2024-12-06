# include "LRU.h"
#include <stdio.h>

int main() {
    // Create an LRU Cache with a capacity of 2
    LRUCache *cache = lRUCacheCreate(2);

    // Put some values in the cache
    lRUCachePut(cache, 1, 1); // Cache: {1: 1}
    lRUCachePut(cache, 2, 2); // Cache: {2: 2, 1: 1}

    // Test LRUCacheGet for existing and non-existing keys
    printf("Get key 1: %d\n", lRUCacheGet(cache, 1)); // Returns 1, Cache: {1: 1, 2: 2}
    lRUCachePut(cache, 3, 3); // Evicts key 2, Cache: {3: 3, 1: 1}
    printf("Get key 2: %d\n", lRUCacheGet(cache, 2)); // Returns -1 (not found)
    lRUCachePut(cache, 4, 4); // Evicts key 1, Cache: {4: 4, 3: 3}
    printf("Get key 1: %d\n", lRUCacheGet(cache, 1)); // Returns -1 (not found)
    printf("Get key 3: %d\n", lRUCacheGet(cache, 3)); // Returns 3, Cache: {3: 3, 4: 4}
    printf("Get key 4: %d\n", lRUCacheGet(cache, 4)); // Returns 4, Cache: {4: 4, 3: 3}

    // Free the LRU Cache
    lRUCacheFree(cache);

    return 0;
}

