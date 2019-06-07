#include<stdio.h>
#include "tree.h"
#include "semantic.h"
#include "intercode.h"
#include "asmcode.h"

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

    if(!f){
        perror(argv[1]);
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
        FILE* irfile = fopen(argv[2], "w+");
        FILE* sfile = fopen(argv[3], "w+");
        if(!irfile){
            perror(argv[2]);
            return 1;
        }
        fprintf(sfile, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\nwrite:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
        print_code(code, irfile);
        asmcode(code, sfile);
        fclose(irfile);
    }

    fclose(f);

    return 0;
}