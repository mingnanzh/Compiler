#include "intercode.h"

int var_num = 1;
int temp_num = 1;

VarMap vmbegin, vmend;

void insert2VarMap(char* name, Operand op)
{
    if(!vmbegin)
    {
        vmbegin = (VarMap)malloc(sizeof(struct VarMap_));
        vmbegin->varname = (char*)malloc(strlen(name)+1);
        strcpy(vmbegin->varname, name);
        vmbegin->op = op;
        vmend = vmbegin;
    }
    else
    {
        VarMap temp = (VarMap)malloc(sizeof(struct VarMap_));
        temp->varname = (char*)malloc(strlen(name)+1);
        strcpy(temp->varname, name);
        temp->op = op;
        vmend->next = temp;
        vmend = temp;
    }
}

Operand findOpInVarMap(char* name)
{
    VarMap temp = vmbegin;
    while(temp){
        if(!strcmp(temp->varname, name))
            return temp->op;
        else
            temp = temp->next;
    }
    return NULL;
}

Intercode GetTail(Intercode code)
{
    if(code->next)
        return GetTail(code->next);
    else
        return code;
}

Intercode LinkCode(Intercode code1, Intercode code2)
{
    if(code1 && code2)
    {
        Intercode code1_tail = GetTail(code1);
        code1_tail->next = code2;
        code2->prev = code1_tail;
        return code1;
    }
    else if(code1)
        return code1;
    else if(code2)
        return code2;
    else
        return NULL;
}

Intercode translate_paramdec(struct treenode* paramdec)
{
    struct treenode* id = paramdec->child->sibling->child;
    // TODO: 
    /*
    Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
    code1->kind = PARAM_;
    code1->u.param.varname = (char*)malloc(strlen(id->name)+1);
    strcpy(code1->u.param.varname, id->name);
    return code1;
    */
}

Intercode translate_varlist(struct treenode* varlist)
{
    struct treenode* paramdec = varlist->child;
    Intercode code1 = translate_paramdec(paramdec);
    if(paramdec->sibling)
    {
        struct treenode* varlist1 = paramdec->sibling->sibling;
        Intercode code2 = translate_varlist(varlist1);
        return LinkCode(code1, code2);
    }
    else
        return code1;
}

Intercode translate_fundec(struct treenode* fundec)
{
    Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
    code1->kind = FUNCTION_;
    code1->u.func.funcname = (char*)malloc(strlen(fundec->child->name)+1);
    strcpy(code1->u.func.funcname, fundec->child->name);
    if(!strcmp(fundec->child->sibling->sibling->name, "VarList"))
    {
        struct treenode* varlist = fundec->child->sibling->sibling;
        Intercode code2 = translate_varlist(varlist);
        return LinkCode(code1, code2);
    }
    else
        return code1;
}

Intercode translate_vardec(struct treenode* vardec, int size)
{
    if(!strcmp(vardec->child->name, "ID"))
    {
        // TODO:
        /*
        struct treenode* id = vardec->child;
        Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
        code1->kind = DEC_;
        code1->u.dec.varname = (char*)malloc(strlen(id->name)+1);
        strcpy(code1->u.dec.varname, id->name);
        code1->u.dec.size = size;
        return code1;
        */
    }
    else if(!strcmp(vardec->child->name, "VarDec"))
    {
        struct treenode* vardec1 = vardec->child; 
        return translate_vardec(vardec1, size * ctoi(vardec1->sibling->sibling->value));
    }
}

Intercode translate_dec(struct treenode* dec)
{
    struct treenode* vardec = dec->child;
    if(!vardec->sibling)
    {
        if(!strcmp(vardec->child->name, "ID"))
            return NULL;
        else
            return translate_vardec(vardec, 1);
    }
    else
    {

    }
}

Intercode translate_declist(struct treenode* declist)
{
    struct treenode* dec = declist->child;
    Intercode code1 = translate_dec(dec);
    if(dec->sibling)
    {
        struct treenode* declist1 = dec->sibling->sibling;
        Intercode code2 = translate_declist(declist1);
        return LinkCode(code1, code2);
    }
    else
        return code1;
}

Intercode translate_def(struct treenode* def)
{
    struct treenode* declist = def->child->sibling;
    return translate_declist(declist);
}

Intercode translate_deflist(struct treenode* deflist)
{
    if(deflist->type!=2)
    {
        struct treenode* def = deflist->child;
        struct treenode* deflist1 = def->sibling;
        Intercode code1 = translate_def(def);
        Intercode code2 = translate_deflist(deflist1);
        return LinkCode(code1, code2);
    }
    else
        return NULL;
}

Intercode translate_stmtlist(struct treenode* stmtlist)
{

}

Intercode translate_compst(struct treenode* compst)
{
    struct treenode* deflist = compst->child->sibling;
    struct treenode* stmtlist = deflist->sibling;
    Intercode code1 = translate_deflist(deflist);
    Intercode code2 =  translate_stmtlist(stmtlist);
    return LinkCode(code1, code2);
}

Intercode translate_extdef(struct treenode* extdef)
{
    if(!strcmp(extdef->child->name, "Specifier") && !strcmp(extdef->child->sibling->name, "SEMI"))
        return NULL;
    else if(!strcmp(extdef->child->name, "Specifier") && !strcmp(extdef->child->sibling->name, "ExtDecList") && !strcmp(extdef->child->sibling->sibling->name, "SEMI"))
        return NULL;
    else if(!strcmp(extdef->child->name, "Specifier") && !strcmp(extdef->child->sibling->name, "FunDec") && !strcmp(extdef->child->sibling->sibling->name, "CompSt"))
    {
        struct treenode* fundec = extdef->child->sibling;
        struct treenode* compst = fundec->sibling;
        Intercode code1 = translate_fundec(fundec);
        Intercode code2 = translate_compst(compst);
        return LinkCode(code1, code2);
    }
}

Intercode translate_extdeflist(struct treenode* extdeflist)
{
    if(extdeflist->type!=2)
    {
        struct treenode* extdef = extdeflist->child;
        struct treenode* extdeflist1 = extdef->sibling;
        Intercode code1 = translate_extdef(extdef);
        Intercode code2 = translate_extdeflist(extdeflist1);
        return LinkCode(code1, code2);
    }
    else
        return NULL;
}

Intercode translate_program(struct treenode* program)
{
    struct treenode* extdeflist = program->child; 
    return translate_extdeflist(extdeflist);
}