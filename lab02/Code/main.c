#include<stdio.h>
#include "tree.h"
#include "semantic.h"

extern int yyrestart();
extern int yyparse();
extern int yylineno;
extern int allright;

extern struct treenode* root;

extern void scanning(struct treenode* r);

int main(int argc,char** argv){
    if(argc<=1)
        return 1;
    FILE* f=fopen(argv[1],"r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yylineno=1;
    yyrestart(f);
    yyparse();
    
    if(allright){
        //print_tree(root,0);
        scanning(root);
    }

    return 0;
}
