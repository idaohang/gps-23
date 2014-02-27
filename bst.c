#include "bst.h"

void 
insert(node **root, int key, int value)
{
	if (!(*root)) {
		(*root) = malloc(sizeof(node));
		(*root)->key = key;
		(*root)->value = value;
		(*root)->left = NULL;
		(*root)->right = NULL;
	}
	else if ((*root)->key > key) {
			insert(&(*root)->right, key, value);
		} 
		else if ((*root)->key < key) {
			insert(&(*root)->left, key, value);
		}
		else {
			(*root)->value = value;
		}
}


node *
find(node *root, int key)
{
	if (!root) {
		return NULL;
	}

	if (root->key > key) {
		return find(root->right, key);
	}
	else if (root->key < key) {
		return find(root->left, key);
	}
	else {
		return root;
	}
}

void 
free_tree(node *root)
{
	if (root) {
		free_tree(root->right);
		free_tree(root->left);
		free(root);
	}
}

void 
walk(node *root, dispersionHelper *result)
{
	if (root) {
		walk(root->left, result);
		result->sum_vals += root->value;
		result->expect += root->key * root->value;
		result->expect_sq += root->key * root->key * root->value;
		walk(root->right, result);
	}
}

void 
print_walk(node *root)
{
	if (root) {
		print_walk(root->left);
		printf("speed: %ld freq: %ld\n", root->key, root->value);
		print_walk(root->right);
	}
}