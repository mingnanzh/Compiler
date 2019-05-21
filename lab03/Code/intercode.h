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

struct Operand_
{
    enum {VARIABLE, CONSTANT, ADDRESS} kind;
    union 
    {
        int var_no;
        int value;
    }u;
};

struct Intercode_
{
    enum {ASSIGN_, ADD_, SUB_, MUL_, DIV_, LABEL_, FUNCTION_, GOTO_, CONDGOTO_, RETURN_, DEC_, ARG_, CALL_, PARAM_, REAN_, WRITE_} kind;
    union
    {
        struct {Operand op;} sigop;
        struct {Operand op1, op2;} douop;
        struct {Operand op1, op2, op3;} triop;
        struct {Operand op1, op2, op3; char* relop;} condgoto;
        struct {Operand op; int size;} dec;
    }u;
    Intercode prev;
    Intercode next;
};

#endif