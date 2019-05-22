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

struct Operand_
{
    enum {VARIABLE, TEMP, CONSTANT, ADDRESS} kind;
    union 
    {
        int var_no;
        int temp_no;
        int value;
        char* varname;
    }u;
};

struct VarMap_
{
    char* varname;
    Operand op;
    VarMap next;
};

struct Intercode_
{
    enum {ASSIGN_, ADD_, SUB_, MUL_, DIV_, LABEL_, FUNCTION_, GOTO_, CONDGOTO_, RETURN_, DEC_, ARG_, CALL_, PARAM_, REAN_, WRITE_} kind;
    union
    {
        struct {Operand op1, op2;} assign;
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

#endif