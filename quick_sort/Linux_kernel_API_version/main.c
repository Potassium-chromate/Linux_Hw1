#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct node__{
    int value;
    struct list_head list;
} node_t;


/* Verify if list is order */
static bool list_is_ordered(struct list_head *head)
{       
    bool first = true;
    struct list_head *cur = head->next;
    int value = list_entry(cur, node_t, list)->value;
    cur = cur->next;

    while (cur != head) {
    	node_t *cur_node = list_entry(cur, node_t, list);
        
        if (cur_node->value < value)
            return false;
        value = cur_node->value;
        
        cur = cur->next;
    }
    return true;
}

void list_construct(struct list_head *head, int n)
{
    node_t *node = malloc(sizeof(node_t));
    INIT_LIST_HEAD(&node->list);
    list_add(&(node->list), head);
    node->value = n;
    //printf("%d\n",n);
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

int list_length(struct list_head *head){
	if(!head){
		return 0;
	}
	int length = 0;
	struct list_head *cur = head->next;
	while(cur != head){
		length ++;
		cur = cur->next;
	}
	
	return length;
}

struct list_head *new_list(){
	struct list_head *new = malloc(sizeof(struct list_head));
	INIT_LIST_HEAD(new);
	return new;
}

void INIT_LIST_ARAY(struct list_head **array, int len){
	for(int i = 0; i < len; i++){
		array[i] = new_list();
	}
	return;
}

void print_list(struct list_head *head){
	/*
	node_t *entry, *safe; 
	list_for_each_entry_safe(entry, safe, head, list) {
		printf("%d\n", safe->value);
	}
	
	*/
	struct list_head *cur = head->next;
	while(cur != head){
		printf("%d\n", list_entry(cur, node_t, list)->value);
		cur = cur->next;
	}
	
}

void quick_sort(struct list_head **head) {
    int n = list_length(*head);
    int value;
    int i = 0;
    int max_level = 2 * n;
    
    struct list_head **begin = malloc(max_level * sizeof(struct list_head*));  
    struct list_head *result = new_list(), *left = new_list(), *right = new_list();
    
    begin[0] = *head;
    

    while (i >= 0) {
        struct list_head *L = begin[i]->next, *R = begin[i]->prev;
      
        if (L != R) {
            struct list_head *pivot = L;
            //remove pivot from list
            list_del(pivot);
            INIT_LIST_HEAD(pivot);
            value = list_entry(pivot, node_t, list)->value;        
            
            struct list_head *p = begin[i]->next;
            INIT_LIST_HEAD(pivot);
            
            //split the list base on pivot
            while (p != begin[i]) {
                struct list_head *n = p;
                p = p->next;
                list_del(n);
                INIT_LIST_HEAD(n);
                int n_value = list_entry(n, node_t, list)->value;                
				
				//right: larger than pivot
				//left: less than pivot
                list_add(n, (n_value > value)? right: left);
            }

            begin[i] = left;
            struct list_head * pivot_head = new_list();
            list_add(pivot, pivot_head);
            begin[i + 1] = pivot_head;
            begin[i + 2] = right;
            
            left = new_list();
            right = new_list();
            i += 2;
        } else {     
        	
            if (L && list_length(L) > 0){
            	//free the head without data
            	struct list_head *tmp = L->prev;
            	list_del(L);
            	free(tmp);
            	
            	//insert to result          	
                list_add(L, result);               
            }
            i--;
        }
    }
    
    *head = result;
    
    free(begin);
    return;
}


int main(int argc, char **argv)
{
    node_t *list = NULL;

    size_t count = 10;

    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;
    shuffle(test_arr, count);
    
	
	struct list_head *head = new_list();
	INIT_LIST_HEAD(head);
	
    while (count--)
        list_construct(head, test_arr[count]);
	
    quick_sort(&head);
    assert(list_is_ordered(head));

    free(head);

    free(test_arr);

    return 0;
}
