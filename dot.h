#include <stdio.h>


void export_to_dot(node_t *list, int step, char *tag) {
    char filename[50];
    snprintf(filename, sizeof(filename), "quicksort_step_%d_%s.dot", step, tag);
    FILE *file = fopen(filename, "w");

    fprintf(file, "digraph G {\n");
    fprintf(file, "  node [shape=record];\n");

    node_t *current = list;
    int node_id = 0;
    while (current) {
        fprintf(file, "  node%d [label=\"{%ld}\"];\n", node_id, current->value);
        if (current->next) {
            fprintf(file, "  node%d -> node%d;\n", node_id, node_id + 1);
        }
        current = current->next;
        node_id++;
    }

    fprintf(file, "}\n");
    fclose(file);
}

