#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types.h>
struct node{
    struct node *left;
    struct node *right;
    bool isleaf;

};


struct node *insert(char *a, struct node *root, int *n);

int  insert_route_in_trie(__uint64_t *key,int p);

struct node * lookup(char *a, struct node *root,int * prefix,int *f_prefix);

int lookup_in_trie(__uint64_t *key);

struct node * create_node();

