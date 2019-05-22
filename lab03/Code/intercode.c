#include "intercode.h"

int var_num = 1;
int label_num = 1;

ArgList arg_list = NULL;

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

void insertArg2ArgList(Operand arg)
{
    if(!arg_list)
    {
        arg_list = (ArgList)malloc(sizeof(struct ArgList_));
        arg_list->op = arg;
    }
    else
    {
        ArgList temp = (ArgList)malloc(sizeof(struct ArgList_));
        temp->op = arg;
        temp->next = arg_list;
        arg_list = temp;
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

Operand lookup(char* name)
{
    Operand op = findOpInVarMap(name);
    if(!op)
    {
        op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = VARIABLE;
        op->u.var_no = var_num;
        insert2VarMap(name, op);
        var_num = var_num + 1;
    }
    return op;
}

Operand new_temp()
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = VARIABLE;
    op->u.var_no = var_num;
    var_num = var_num + 1;
    return op;
}

Operand new_getaddress(int no)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = GETADDRESS;
    op->u.var_no = no;
    return op;
}

Operand new_getvalue(int no)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = GETVALUE;
    op->u.var_no = no;
    return op;
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

Intercode translate_args(struct treenode* args)
{
    struct treenode* exp = args->child;
    if(!exp->sibling)
    {
        Operand t1 = new_temp();
        Intercode code1 = translate_exp(exp, t1);
        insertArg2ArgList(t1);
        return code1;
    }
    else
    {
        struct treenode* args1 = exp->sibling->sibling;
        Operand t1 = new_temp();
        Intercode code1 = translate_exp(exp, t1);
        insertArg2ArgList(t1);
        Intercode code2 = translate_args(args1);
        return LinkCode(code1, code2);
    }   
}

Intercode translate_cond(struct treenode* exp, int label_true, int label_false)
{

}

Intercode translate_exp(struct treenode* exp, Operand op)
{
    if(!strcmp(exp->child->name, "INT"))
    {
        Operand right = (Operand)malloc(sizeof(struct Operand_));
        right->kind = CONSTANT;
        right->u.value = (char*)malloc(strlen(exp->child->value)+1);
        strcpy(right->u.value, exp->child->value);
        Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
        code1->kind = ASSIGN_;
        code1->u.assign.left = op;
        code1->u.assign.right = right;
        return code1;
    }
    else if(!strcmp(exp->child->name, "FLOAT"))
    {
        Operand right = (Operand)malloc(sizeof(struct Operand_));
        right->kind = CONSTANT;
        right->u.value = (char*)malloc(strlen(exp->child->value)+1);
        strcpy(right->u.value, exp->child->value);
        Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
        code1->kind = ASSIGN_;
        code1->u.assign.left = op;
        code1->u.assign.right = right;
        return code1;
    }
    else if(!strcmp(exp->child->name, "ID") && !(exp->child->sibling))
    {
        Operand right = lookup(exp->child->value);
        Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
        code1->kind = ASSIGN_;
        code1->u.assign.left = op;
        code1->u.assign.right = right;
        return code1;
    }
    else if(!strcmp(exp->child->name, "LP") && !strcmp(exp->child->sibling->name, "Exp") && !strcmp(exp->child->sibling->sibling->name, "RP"))
    {
        return translate_exp(exp->child->sibling, op);
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "LB") && !strcmp(exp->child->sibling->sibling->name, "Exp") && !strcmp(exp->child->sibling->sibling->sibling->name, "RB"))
    {
        Operand id = lookup(exp->child->child->value);
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        Operand t3 = new_getaddress(id->u.var_no);
        Operand t4 = new_temp();
        Operand t5 = new_getvalue(t4->u.var_no);
        Operand cons = (Operand)malloc(sizeof(struct Operand_));
        cons->kind = CONSTANT;
        cons->u.value = (char*)malloc(strlen("4")+1);
        strcpy(cons->u.value, "4");
        Intercode code1 = translate_exp(exp->child->sibling->sibling, t1);
        Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = MUL_;
        code2->u.binop.result = t2;
        code2->u.binop.op1 = t1;
        code2->u.binop.op2 = cons;
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = ADD_;
        code3->u.binop.result = t4;
        code3->u.binop.op1 = t3;
        code3->u.binop.op2 = t2;
        Intercode code4 = (Intercode)malloc(sizeof(struct Intercode_));
        code4->kind = ASSIGN_;
        code4->u.assign.left = op;
        code4->u.assign.right = t5;
        return LinkCode(code1, LinkCode(code2, LinkCode(code3, code4)));
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "ASSIGNOP") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        struct treenode* exp1 = exp->child;
        struct treenode* exp2 = exp1->sibling->sibling;
        if(!strcmp(exp1->child->name, "ID"))
        {
            Operand id = lookup(exp1->child->value);
            Operand t1 = new_temp();
            Intercode code1 = translate_exp(exp2, t1);
            Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
            code2->kind = ASSIGN_;
            code2->u.assign.left = id;
            code2->u.assign.right = t1;
            Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
            code3->kind = ASSIGN_;
            code3->u.assign.left = op;
            code3->u.assign.right = id;
            return LinkCode(code1, LinkCode(code2, code3));
        }
        else if(!strcmp(exp1->child->child->name, "ID"))
        {
            Operand t5 = new_temp();
            Operand id = lookup(exp1->child->child->value);
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            Operand t3 = new_getaddress(id->u.var_no);
            Operand t4 = new_temp();
            Operand t6 = new_getvalue(t4->u.var_no);
            Operand cons = (Operand)malloc(sizeof(struct Operand_));
            cons->kind = CONSTANT;
            cons->u.value = (char*)malloc(strlen("4")+1);
            strcpy(cons->u.value, "4");
            Intercode code1 = translate_exp(exp1->child->sibling->sibling, t1);
            Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
            code2->kind = MUL_;
            code2->u.binop.result = t2;
            code2->u.binop.op1 = t1;
            code2->u.binop.op2 = cons;
            Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
            code3->kind = ADD_;
            code3->u.binop.result = t4;
            code3->u.binop.op1 = t3;
            code3->u.binop.op2 = t2;
            Intercode code4 = translate_exp(exp2, t5);
            Intercode code5 = (Intercode)malloc(sizeof(struct Intercode_));
            code5->kind = ASSIGN_;
            code5->u.assign.left = t6;
            code5->u.assign.right = t5;
            Intercode code6 = (Intercode)malloc(sizeof(struct Intercode_));
            code6->kind = ASSIGN_;
            code6->u.assign.left = op;
            code6->u.assign.right = t6;
            return LinkCode(code4, LinkCode(code1, LinkCode(code2, LinkCode(code3, LinkCode(code5, code6)))));
        }
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "PLUS") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        struct treenode* exp1 = exp->child;
        struct treenode* exp2 = exp1->sibling->sibling;
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        Intercode code1 = translate_exp(exp1, t1);
        Intercode code2 = translate_exp(exp2, t2);
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = ADD_;
        code3->u.binop.result = op;
        code3->u.binop.op1 = t1;
        code3->u.binop.op2 = t2;
        return LinkCode(code1, LinkCode(code2, code3));
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "MINUS") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        struct treenode* exp1 = exp->child;
        struct treenode* exp2 = exp1->sibling->sibling;
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        Intercode code1 = translate_exp(exp1, t1);
        Intercode code2 = translate_exp(exp2, t2);
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = SUB_;
        code3->u.binop.result = op;
        code3->u.binop.op1 = t1;
        code3->u.binop.op2 = t2;
        return LinkCode(code1, LinkCode(code2, code3));
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "STAR") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        struct treenode* exp1 = exp->child;
        struct treenode* exp2 = exp1->sibling->sibling;
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        Intercode code1 = translate_exp(exp1, t1);
        Intercode code2 = translate_exp(exp2, t2);
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = MUL_;
        code3->u.binop.result = op;
        code3->u.binop.op1 = t1;
        code3->u.binop.op2 = t2;
        return LinkCode(code1, LinkCode(code2, code3));
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "DIV") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        struct treenode* exp1 = exp->child;
        struct treenode* exp2 = exp1->sibling->sibling;
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        Intercode code1 = translate_exp(exp1, t1);
        Intercode code2 = translate_exp(exp2, t2);
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = DIV_;
        code3->u.binop.result = op;
        code3->u.binop.op1 = t1;
        code3->u.binop.op2 = t2;
        return LinkCode(code1, LinkCode(code2, code3));
    }
    else if(!strcmp(exp->child->name, "MINUS") && !strcmp(exp->child->sibling->name, "Exp"))
    {
        struct treenode* exp1 = exp->child->sibling;
        Operand t1 = new_temp();
        Operand cons = (Operand)malloc(sizeof(struct Operand_));
        cons->kind = CONSTANT;
        cons->u.value = (char*)malloc(strlen("0")+1);
        strcpy(cons->u.value, "0");
        Intercode code1 = translate_exp(exp1, t1);
        Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = SUB_;
        code2->u.binop.result = op;
        code2->u.binop.op1 = t1;
        code2->u.binop.op2 = cons;
        return LinkCode(code1, code2);
    }
    else if(!strcmp(exp->child->name, "ID") && !strcmp(exp->child->sibling->name, "LP") && !strcmp(exp->child->sibling->sibling->name, "RP"))
    {
        struct treenode* id = exp->child;
        if(!strcmp(id->value, "read"))
        {
            Intercode code1 = (Intercode)mallloc(sizeof(struct Intercode_));
            code1->kind = READ_;
            code1->u.read.op = op;
            return code1;
        }
        else
        {
            Intercode code1 = (Intercode)mallloc(sizeof(struct Intercode_));
            code1->kind = CALL_;
            code1->u.call.result = op;
            code1->u.call.funcname = (char*)malloc(strlen(id->value)+1);
            strcpy(code1->u.call.funcname, id->value);
            return code1;
        }     
    }
    else if(!strcmp(exp->child->name, "ID") && !strcmp(exp->child->sibling->name, "LP") && !strcmp(exp->child->sibling->sibling->name, "Args") && !strcmp(exp->child->sibling->sibling->sibling->name, "RP"))
    {
        struct treenode* id = exp->child;
        struct treenode* args = id->sibling->sibling;
        arg_list = NULL;
        Intercode code1 = translate_args(args);
        if(!strcmp(id->value, "write"))
        {
            Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
            code2->kind = WRITE_;
            code2->u.write.op = arg_list->op;
            return LinkCode(code1, code2);
        }
        else
        {
            Intercode code2 = NULL;
            while(arg_list)
            {
                Intercode code_temp = (Intercode)malloc(sizeof(struct Intercode_));
                code_temp->kind = ARG_;
                code_temp->u.arg.op = arg_list->op;
                code2 = LinkCode(code2, code_temp);
                arg_list = arg_list->next;
            }
            Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
            code3->kind = CALL_;
            code3->u.call.result = op;
            code3->u.call.funcname = (char*)malloc(strlen(id->value)+1);
            strcpy(code3->u.call.funcname, id->value);
            return LinkCode(code1, LinkCode(code2, code3));
        }   
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "AND") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        int label1 = label_num++;
        int label2 = label_num++;
        Operand cons = (Operand)malloc(sizeof(struct Operand_));
        cons->kind = CONSTANT;
        cons->u.value = (char*)malloc(strlen("0")+1);
        strcpy(cons->u.value, "0");
        Intercode code0 = (Intercode)malloc(sizeof(struct Intercode_));
        code0->kind = ASSIGN_;
        code0->u.assign.left = op;
        code0->u.assign.right = cons;
        Intercode code1 = translate_cond(exp, label1, label2);
        Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label1;
        Operand cons1 = (Operand)malloc(sizeof(struct Operand_));
        cons1->kind = CONSTANT;
        cons1->u.value = (char*)malloc(strlen("1")+1);
        strcpy(cons1->u.value, "1");
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = ASSIGN_;
        code3->u.assign.left = op;
        code3->u.assign.right = cons1;
        Intercode code4 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label2;
        return LInkCode(code0, LinkCode(code1, LinkCode(code2, LinkCode(code3, code4))));
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "OR") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        int label1 = label_num++;
        int label2 = label_num++;
        Operand cons = (Operand)malloc(sizeof(struct Operand_));
        cons->kind = CONSTANT;
        cons->u.value = (char*)malloc(strlen("0")+1);
        strcpy(cons->u.value, "0");
        Intercode code0 = (Intercode)malloc(sizeof(struct Intercode_));
        code0->kind = ASSIGN_;
        code0->u.assign.left = op;
        code0->u.assign.right = cons;
        Intercode code1 = translate_cond(exp, label1, label2);
        Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label1;
        Operand cons1 = (Operand)malloc(sizeof(struct Operand_));
        cons1->kind = CONSTANT;
        cons1->u.value = (char*)malloc(strlen("1")+1);
        strcpy(cons1->u.value, "1");
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = ASSIGN_;
        code3->u.assign.left = op;
        code3->u.assign.right = cons1;
        Intercode code4 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label2;
        return LInkCode(code0, LinkCode(code1, LinkCode(code2, LinkCode(code3, code4))));
    }
    else if(!strcmp(exp->child->name, "Exp") && !strcmp(exp->child->sibling->name, "RELOP") && !strcmp(exp->child->sibling->sibling->name, "Exp"))
    {
        int label1 = label_num++;
        int label2 = label_num++;
        Operand cons = (Operand)malloc(sizeof(struct Operand_));
        cons->kind = CONSTANT;
        cons->u.value = (char*)malloc(strlen("0")+1);
        strcpy(cons->u.value, "0");
        Intercode code0 = (Intercode)malloc(sizeof(struct Intercode_));
        code0->kind = ASSIGN_;
        code0->u.assign.left = op;
        code0->u.assign.right = cons;
        Intercode code1 = translate_cond(exp, label1, label2);
        Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label1;
        Operand cons1 = (Operand)malloc(sizeof(struct Operand_));
        cons1->kind = CONSTANT;
        cons1->u.value = (char*)malloc(strlen("1")+1);
        strcpy(cons1->u.value, "1");
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = ASSIGN_;
        code3->u.assign.left = op;
        code3->u.assign.right = cons1;
        Intercode code4 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label2;
        return LInkCode(code0, LinkCode(code1, LinkCode(code2, LinkCode(code3, code4))));
    }
    else if(!strcmp(exp->child->name, "NOT") && !strcmp(exp->child->sibling->name, "Exp"))
    {
        int label1 = label_num++;
        int label2 = label_num++;
        Operand cons = (Operand)malloc(sizeof(struct Operand_));
        cons->kind = CONSTANT;
        cons->u.value = (char*)malloc(strlen("0")+1);
        strcpy(cons->u.value, "0");
        Intercode code0 = (Intercode)malloc(sizeof(struct Intercode_));
        code0->kind = ASSIGN_;
        code0->u.assign.left = op;
        code0->u.assign.right = cons;
        Intercode code1 = translate_cond(exp, label1, label2);
        Intercode code2 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label1;
        Operand cons1 = (Operand)malloc(sizeof(struct Operand_));
        cons1->kind = CONSTANT;
        cons1->u.value = (char*)malloc(strlen("1")+1);
        strcpy(cons1->u.value, "1");
        Intercode code3 = (Intercode)malloc(sizeof(struct Intercode_));
        code3->kind = ASSIGN_;
        code3->u.assign.left = op;
        code3->u.assign.right = cons1;
        Intercode code4 = (Intercode)malloc(sizeof(struct Intercode_));
        code2->kind = LABEL_;
        code2->u.label.labelnum = label2;
        return LInkCode(code0, LinkCode(code1, LinkCode(code2, LinkCode(code3, code4))));
    }
    else
    {
        return NULL;
    }
    
}

Intercode translate_paramdec(struct treenode* paramdec)
{
    struct treenode* id = paramdec->child->sibling->child;
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = VARIABLE;
    op->u.var_no = var_num;
    insert2VarMap(id->value, op);
    var_num = var_num + 1;
    Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
    code1->kind = PARAM_;
    code1->u.param.op = op;
    return code1;
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
        struct treenode* id = vardec->child;
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = VARIABLE;
        op->u.var_no = var_num;
        insert2VarMap(id->value, op);
        var_num = var_num + 1;
        Intercode code1 = (Intercode)malloc(sizeof(struct Intercode_));
        code1->kind = DEC_;
        code1->u.dec.op = op;
        code1->u.dec.size = size;
        return code1;
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
        struct treenode* id = vardec->child;
        struct treenode* exp = vardec->sibling->sibling;
        Operand op = lookup(id->value);
        return translate_exp(exp, op);
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