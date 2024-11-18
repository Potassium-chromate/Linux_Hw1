#include <stdint.h>
#include <stdlib.h>

#pragma once

#ifndef LIST_H
#define LIST_H
#endif 


#define FFFF 1

typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t) (head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static struct list_head *merge(void *priv,
                               list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head;
    //AAAA
    struct list_head **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
        	//cmp will return a's value - b's value
            *tail = a;
            //BBBB
            tail = &(a->next);
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            //CCCC
            tail = &(b->next);
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{	
	//assume list is not doubly linked
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    //DDDD
    //EEEE
    tail->next = head;
	head->prev = tail;  
    
}

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    build_prev_link(head, tail, b);
}

static struct pair find_run(void *priv,
                            struct list_head *list,
                            list_cmp_func_t cmp)
{
    size_t len = 1; // Initialize length of the run
    struct list_head *next = list->next, *head = list;
    struct pair result;

    // If the list has only one node, return immediately
    if (!next) {
        result.head = head, result.next = next;
        return result;
    }
	//The last node of run, its `next` should point to NULL
    // Check if the run is descending
    if (cmp(priv, list, next) > 0) {
        // If the next node is smaller, it indicates a descending run
        // Reverse the order of nodes in the run to convert it to ascending order
        struct list_head *prev = NULL;
        do {
            len++;
            // Point the current node's `next` to the previous node to reverse the order
            list->next = prev;
            prev = list;
            list = next; 
            next = list->next;
            head = list; // Update `head` to the new start of the reversed list
        } while (next && cmp(priv, list, next) > 0); // Continue if in descending order

        // Set the last node in the reversed run to point to the previous node
        list->next = prev;
    } else {
        // If the run is ascending, keep the order as is
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && cmp(priv, list, next) <= 0);
        
        
        list->next = NULL;
    }
    
    head->prev = NULL; // Initialize `prev` of `head` to NULL

    // The list is now a singly linked list.
    // Use `head->next->prev` to store `len` (run length) to reduce the use of additional variables.
    head->next->prev = (struct list_head *) len;
    
    result.head = head, result.next = next;
    return result;
}


static struct list_head *merge_at(void *priv,
                                  list_cmp_func_t cmp,
                                  struct list_head *at)
{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = merge(priv, cmp, at->prev, at);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(void *priv,
                                              list_cmp_func_t cmp,
                                              struct list_head *tp)
{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(priv, cmp, tp->prev);
        } else {
            tp = merge_at(priv, cmp, tp);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(void *priv,
                                        list_cmp_func_t cmp,
                                        struct list_head *tp)
{
    int n;
    // Loop until there are fewer than 2 runs in the stack
    while ((n = stk_size) >= 2) {
    	// Check conditions to decide whether to merge runs
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            //Check if the third-last run is smaller or equal to sizes of the last runs.               
            if (run_size(tp->prev->prev) < run_size(tp)) {
            	//Merge third-last and second-last
                tp->prev = merge_at(priv, cmp, tp->prev);
            } else {
            	//Merge last and second-last
                tp = merge_at(priv, cmp, tp);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
        	//Merge last and second-last
            tp = merge_at(priv, cmp, tp);
        } else {
            break;
        }
    }

    return tp;
}

void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    stk_size = 0;

    struct list_head *list = head->next, *tp = NULL;
    if (head == head->prev)
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        /* Find next run */
        struct pair result = find_run(priv, list, cmp);
        result.head->prev = tp;
        tp = result.head;
        //Move list to the head of next run
        list = result.next;
        stk_size++;
        tp = merge_collapse(priv, cmp, tp);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(priv, cmp, tp);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= FFFF) {
        build_prev_link(head, head, stk0);
        return;
    }
    merge_final(priv, cmp, head, stk1, stk0);
}
