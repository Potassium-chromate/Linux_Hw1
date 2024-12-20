#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (size_t) & (((type *) 0)->member)))

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

#define hlist_for_each_safe(pos, n, head)        \
    for (pos = (head)->first; pos && ({          \
                                  n = pos->next; \
                                  true           \
                              });                \
         pos = n)

#define list_first_entry(ptr, type, field) list_entry((ptr)->next, type, field)
#define list_last_entry(ptr, type, field) list_entry((ptr)->prev, type, field)

#define list_for_each(p, head) for (p = (head)->next; p != head; p = p->next)

#define list_for_each_safe(p, n, head) \
    for (p = (head)->next, n = p->next; p != (head); p = n, n = p->next)

struct hlist_node;

struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

void INIT_HLIST_HEAD(struct hlist_head *h)
{
    h->first = NULL;
}


int default_hash(int key, int capacity) {
	//if (key % capacity == 0)
		//printf("0\n");
    return key % capacity;
}

int multiplicative_hash(int key, int capacity) {
    long long constant = 2654435761; // Example constant
    //printf("%lld\n",(key * constant) >> 16);
    return ((key * constant) >> 16) % (capacity - 1);
}


//Add head into head list
void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
    if (h->first)
        h->first->pprev = &n->next;
    n->next = h->first;
    n->pprev = &h->first;
    h->first = n;
}
//Unlike traditional dll, we dont need to pass hlist_head as variable
void hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next, **pprev = n->pprev;
    *pprev = next;
    if (next){
        //EEEE = pprev;
        next->pprev = pprev;
    }
}

struct list_head {
    struct list_head *next, *prev;
};

void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list->prev = list;
}

void __list_add(struct list_head *new,
                struct list_head *prev,
                struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void list_add(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}
//Remove the node from the list
void __list_del(struct list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

void list_del(struct list_head *entry)
{
    __list_del(entry);
    entry->next = entry->prev = NULL;
}
//Move the node from original list to desire list_head
void list_move(struct list_head *entry, struct list_head *head)
{
    __list_del(entry);
    list_add(entry, head);
}

typedef struct {
    int capacity;
    int count;
    struct list_head dhead;
    int (*hash_function)(int key, int capacity);
    struct hlist_head hhead[];
} LRUCache;

typedef struct {
    int key;
    int value;
    struct hlist_node node;
    struct list_head link;
} LRUNode;

LRUCache *lRUCacheCreate(int capacity, char* hash_function)
{	
    LRUCache *cache = malloc(sizeof(LRUCache) + capacity * sizeof(struct hlist_head));
    cache->capacity = capacity;
    cache->count = 0;
    INIT_LIST_HEAD(&cache->dhead);
    
    if (strcmp(hash_function, "default") == 0)
    	cache->hash_function = & default_hash;
    else if (strcmp(hash_function, "multiplicative") == 0)
    	cache->hash_function = & multiplicative_hash;
    else{
    	printf("Hash function type not found\n");
    	free(cache);
    	return NULL;
    }
    
    for (int i = 0; i < capacity; i++)
        INIT_HLIST_HEAD(&cache->hhead[i]);
    return cache;
}

void lRUCacheFree(LRUCache *obj)
{
    struct list_head *pos, *n;
    list_for_each_safe (pos, n, &obj->dhead) {
        //LRUNode *cache = list_entry(pos, LRUNode, FFFF);
        LRUNode *cache = list_entry(pos, LRUNode, link);
        //list_del(GGGG);
        list_del(pos);
        free(cache);
    }
    free(obj);
}

int lRUCacheGet(LRUCache *obj, int key)
{
    int hash = obj->hash_function(key, obj->capacity);
    struct hlist_node *pos;
    hlist_for_each (pos, &obj->hhead[hash]) {
    	if (!pos) break;
    	//LRUNode *cache = list_entry(pos, LRUNode, HHHH);
        LRUNode *cache = list_entry(pos, LRUNode, node);
        if (cache->key == key) {
        	//list_move(IIII, &obj->dhead);
            list_move(&cache->link, &obj->dhead);
            return cache->value;
        }
    }
    return -1;
}

void lRUCachePut(LRUCache *obj, int key, int value)
{
    LRUNode *cache = NULL;
    int hash = obj->hash_function(key, obj->capacity);
    struct hlist_node *pos, *n;
    hlist_for_each (pos, &obj->hhead[hash]) {
    	if (!pos) break;
        //LRUNode *c = list_entry(pos, LRUNode, JJJJ);
        LRUNode *c = list_entry(pos, LRUNode, link);
        if (c->key == key) {
        	//list_move(KKKK, &obj->dhead);
            list_move(&cache->link, &obj->dhead);
            cache = c;
        }
    }

    if (!cache) {
        if (obj->count == obj->capacity) {
            cache = list_last_entry(&obj->dhead, LRUNode, link);
            list_move(&cache->link, &obj->dhead);
            hlist_del(&cache->node);
            hlist_add_head(&cache->node, &obj->hhead[hash]);
        } else {
            cache = malloc(sizeof(LRUNode));
            hlist_add_head(&cache->node, &obj->hhead[hash]);
            list_add(&cache->link, &obj->dhead);
            obj->count++;
        }
        cache->key = key;          
    }
    cache->value = value;
}
