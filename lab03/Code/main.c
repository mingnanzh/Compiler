#include<stdio.h>
#include "tree.h"
#include "semantic.h"
#include "intercode.h"

extern int yyrestart();
extern int yyparse();
extern int yylineno;
extern int allright;

extern struct treenode* root;

extern void scanning(struct treenode* r);

int main(int argc,char** argv){
    if(argc<=1)
        return 1;

    FILE* f = fopen(argv[1],"r");
    FILE* irfile = fopen(argv[2], "w+");

    if(!f){
        perror(argv[1]);
        return 1;
    }

    if(!irfile){
        perror(argv[2]);
        return 1;
    }

    yylineno = 1;
    yyrestart(f);
    yyparse();
    
    if(allright){
        //print_tree(root,0);
        addFunc2Table("write", NULL, 1, NULL);
        addFunc2Table("read", NULL, 0, NULL);
        scanning(root);
        Intercode code = translate_program(root);
        print_code(code, irfile);
    }

    fclose(f);
    fclose(irfile);

    return 0;
}