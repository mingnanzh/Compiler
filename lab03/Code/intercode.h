#ifndef _INTERCODE_H_
#define _INTERCODE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "tree.h"
#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct Intercode_* Intercode;
typedef struct VarMap_* VarMap;
typedef struct ArgList_* ArgList;

struct Operand_
{
    enum {VARIABLE, CONSTANT, GETADDRESS, GETVALUE} kind;
    union 
    {
        int var_no;
        char* value;
    }u;
};

struct VarMap_
{
    char* varname;
    Operand op;
    VarMap next;
};

struct ArgList_
{
    Operand op;
    ArgList next;
};

struct Intercode_
{
    enum {ASSIGN_, ADD_, SUB_, MUL_, DIV_, LABEL_, FUNCTION_, GOTO_, CONDGOTO_, RETURN_, DEC_, ARG_, CALL_, PARAM_, READ_, WRITE_} kind;
    union
    {
        struct {Operand left, right;} assign;
        struct {Operand result, op1, op2;} binop;
        struct {char* funcname;} func;
        struct {int labelnum;} label;
        struct {int labelnum;} goto_;
        struct {Operand op1, op2; char* relop; int labelnum;} condgoto;
        struct {Operand op; int size;} dec;
        struct {Operand op;} arg;
        struct {Operand result; char* funcname;} call;
        struct {Operand op;} param;
        struct {Operand op;} read;
        struct {Operand op;} write;
    }u;
    Intercode prev;
    Intercode next;
};

void insert2VarMap(char* name, Operand op);
void insertArg2ArgList(Operand arg);
Operand findOpInVarMap(char* name);
Operand lookup(char* name);

Operand new_temp();
Operand new_getaddress(int no);
Operand new_getvalue(int no);

Intercode GetTail(Intercode code);
Intercode LinkCode(Intercode code1, Intercode code2);

Intercode translate_args(struct treenode* args);
Intercode translate_cond(struct treenode* exp, int label_true, int label_false);
Intercode translate_exp(struct treenode* exp, Operand op);
Intercode translate_paramdec(struct treenode* paramdec);
Intercode translate_varlist(struct treenode* varlist);
Intercode translate_fundec(struct treenode* fundec);
Intercode translate_vardec(struct treenode* vardec, int size);
Intercode translate_dec(struct treenode* dec);
Intercode translate_declist(struct treenode* declist);
Intercode translate_def(struct treenode* def);
Intercode translate_deflist(struct treenode* deflist);
Intercode translate_stmtlist(struct treenode* stmtlist);
Intercode translate_compst(struct treenode* compst);
Intercode translate_extdef(struct treenode* extdef);
Intercode translate_extdeflist(struct treenode* extdeflist);
Intercode translate_program(struct treenode* program);

#endif