#ifndef _ASMCODE_H_
#define _ASMCODE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "intercode.h"

typedef struct SaveAddr_* SaveAddr;
typedef struct VarAddr_* VarAddr;

struct SaveAddr_
{
    int addr;
    SaveAddr next;
};

struct VarAddr_
{
    int var_no;
    int addr;
    VarAddr next;
};

void asmcode(Intercode code, FILE* f);

#endif