#include "tree.h"

struct treenode* create_newnode(int tp,char* nm,int lno,char* vl){
    struct treenode* temp=(struct treenode*)malloc(sizeof(struct treenode));
    temp->type=tp;
    temp->name=(char* )malloc(strlen(nm)+1);
    strcpy(temp->name,nm);
    temp->lineno=lno;
    if(vl){
        temp->value=(char* )malloc(strlen(vl)+1);
        strcpy(temp->value,vl);
    }
    return temp;
}

void insert_child(struct treenode* father,struct treenode* child){
    if(father&&child)
        father->child=child;
}

void insert_sibling(struct treenode* left,struct treenode* right){
    if(left&&right)
        left->sibling=right;
}

void print_tree(struct treenode* r,int depth){
    if(r&&r->type<=1){
        struct treenode* temp=r->child;
        for(int i=1;i<=depth;i++)
            printf("  ");
        if(r->type==1)
            printf("%s (%d)\n",r->name,r->lineno);
        else if(!strcmp(r->name,"ID")||!strcmp(r->name,"TYPE")||!strcmp(r->name,"INT")||!strcmp(r->name,"FLOAT"))
            printf("%s: %s\n",r->name,r->value);
        else
            printf("%s\n",r->name);
        while(temp){
            print_tree(temp,depth+1);
            temp=temp->sibling;
        }
    }
}
