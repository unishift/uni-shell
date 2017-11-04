#include <stdlib.h>
#include "list.h"

void insert(node **root, char *word)
{
        node *p = (node*)malloc(sizeof(node));
        p->str = word;
        p->next = NULL;
        if (*root == NULL) {
                *root = p;
        }
        else {
                node *q = *root;
                while (q->next != NULL) q = q->next;
                q->next = p;
        }
}

void delete_list(node* node)
{
        if (node != NULL) {
                delete_list(node->next);
                free(node->str);
                free(node);
        }
}
