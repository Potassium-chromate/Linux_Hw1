#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 10000
#define HASH(val, size) ((val) % (size))

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (size_t) &(((type *) 0)->member)))

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

int search_cost = 0;

struct hlist_node {
    struct hlist_node *next, **pprev;
};
struct hlist_head {
    struct hlist_node *first;
};

static inline void INIT_HLIST_HEAD(struct hlist_head *h) {
    h->first = NULL;
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h) {
    if (h->first)
        h->first->pprev = &(n->next);
    n->next = h->first;
    n->pprev = &h->first;
    h->first = n;
}

struct TreeNode {
    int val;
    struct TreeNode *left, *right;
};

struct order_node {
    struct hlist_node node;
    int val;
    int idx;
};

static int find(int num, const struct hlist_head *heads, int size) {
    int hash_idx = HASH(num, size);
    struct hlist_node *p;
    hlist_for_each(p, &heads[hash_idx]) {
        struct order_node *on = list_entry(p, struct order_node, node);
        search_cost++;
        if (num == on->val)
            return on->idx;
    }
    return -1;
}

static struct TreeNode *dfs(int *preorder, int pre_low, int pre_high, int *inorder, int in_low, int in_high,
                            struct hlist_head *in_heads, int size) {
    if (in_low > in_high || pre_low > pre_high)
        return NULL;

    struct TreeNode *tn = malloc(sizeof(*tn));
    if (!tn) {
        perror("Memory allocation failed\n");
        return NULL;
    }
    tn->val = preorder[pre_low];
    int idx = find(preorder[pre_low], in_heads, size);
    tn->left = dfs(preorder, pre_low + 1, pre_low + (idx - in_low), inorder, in_low, idx - 1, in_heads, size);
    tn->right = dfs(preorder, pre_high - (in_high - idx - 1), pre_high, inorder, idx + 1, in_high, in_heads, size);
    return tn;
}

static inline void node_add(int val, int idx, int size, struct hlist_head *heads) {
    struct order_node *on = malloc(sizeof(*on));
    if (!on) {
        printf("Memory allocation failed\n");
        return;
    }
    on->val = val;
    on->idx = idx;
    int hash_idx = HASH(val, size);
    hlist_add_head(&on->node, &heads[hash_idx]);
}

static struct TreeNode *buildTree(int *preorder, int preorderSize, int *inorder, int inorderSize) {
    int table_size = inorderSize / 2;
    struct hlist_head *in_heads = malloc(table_size * sizeof(*in_heads));
    if (!in_heads) {
        printf("Memory allocation for hash table failed\n");
        return NULL;
    }

    for (int i = 0; i < table_size; i++)
        INIT_HLIST_HEAD(&in_heads[i]);

    for (int i = 0; i < inorderSize; i++)
        node_add(inorder[i], i, table_size, in_heads);

    struct TreeNode *root = dfs(preorder, 0, preorderSize - 1, inorder, 0, inorderSize - 1, in_heads, table_size);
    free(in_heads);
    return root;
}

int main() {
	
    int preorder[MAX_SIZE];
    int inorder[MAX_SIZE];
    int preorderSize = 0, inorderSize = 0;
	
	
    // Open the file
    FILE *pre_file = fopen("Preorder_traversal.txt", "r");
    if (!pre_file) {
        perror("Unable to open file");
        return EXIT_FAILURE;
    }
    
    FILE *in_file = fopen("Inorder_traversal.txt", "r");
    if (!in_file) {
        perror("Unable to open file");
        return EXIT_FAILURE;
    }

    // Read preorder values
    while (fscanf(pre_file, "%d", &preorder[preorderSize]) == 1) {
        preorderSize++;
        if (fgetc(pre_file) == '\n') break;  // Move to next line after reaching newline
    }

    // Read inorder values
    while (fscanf(in_file, "%d", &inorder[inorderSize]) == 1) {
        inorderSize++;
        if (fgetc(in_file) == '\n') break;
    }

    // Close the file
    fclose(pre_file);
    fclose(in_file);
	printf("preorderSize: %d\n",preorderSize);
	printf("inorderSize: %d\n",inorderSize);
	
	
    struct TreeNode *tree = buildTree(preorder, preorderSize, inorder, inorderSize);
	printf("Search cost: %d\n", search_cost);

    return 0;
}
