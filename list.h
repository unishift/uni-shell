#pragma once

struct node {
        char* str;
        struct node *next;
};

typedef struct node node;

void insert(node **root, char *word);
void delete_list(node* node);
