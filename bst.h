#ifndef BST_H
#define BST_H

#include <stdio.h>
#include <stdlib.h>

struct node {
	unsigned long key;
	unsigned long value;
	struct node *left, *right;
};

typedef struct node node;

typedef struct {
	unsigned long sum_vals, expect, expect_sq;
} dispersionHelper;

void insert(node **root, int key, int value);

node *find(node *root, int key);

void free_tree(node *root);

void walk(node *root, dispersionHelper *result);

void print_walk(node *root);

#endif