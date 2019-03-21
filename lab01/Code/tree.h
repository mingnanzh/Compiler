#ifndef _TREE_H_
#define _TREE_H_

#include<stdio.h>
#include<string.h>
#include<malloc.h>

struct treenode{
    int type;
    char* name;
    int lineno;
    char* value;
    struct treenode* child;
    struct treenode* sibling;
};

extern struct treenode* root;

struct treenode* create_newnode(int tp,char* nm,int lno,char* vl);

void insert_child(struct treenode* father,struct treenode* child);

void insert_sibling(struct treenode* left,struct treenode* right);

void print_tree(struct treenode* r,int depth);

#endif