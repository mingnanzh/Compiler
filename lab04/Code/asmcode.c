#include "asmcode.h"

int addr_begin = 0x10010020;
int param_num = 0;
int firstin = 1;
int infunc = 0;
int addrcount = 0;
SaveAddr saveaddr_begin = NULL;
SaveAddr saveaddr_end = NULL;
VarAddr varaddr_begin = NULL;
VarAddr varaddr_end = NULL;

void insert2SaveAddr(int ad)
{
    if(!saveaddr_begin)
    {
        saveaddr_begin = (SaveAddr)malloc(sizeof(struct SaveAddr_));
        saveaddr_begin->addr = ad;
        saveaddr_end = saveaddr_begin;
        addrcount = 1;
    }
    else
    {
        SaveAddr temp = (SaveAddr)malloc(sizeof(struct SaveAddr_));
        temp->addr = ad;
        saveaddr_end->next = temp;
        saveaddr_end = temp;
        addrcount ++;
    }
}

void insert2VarAddr(int var_no, int addr)
{
    if(!varaddr_begin)
    {
        varaddr_begin = (VarAddr)malloc(sizeof(struct VarAddr_));
        varaddr_begin->var_no = var_no;
        varaddr_begin->addr = addr;
        varaddr_end = varaddr_begin;
    }
    else
    {
        VarAddr temp = (VarAddr)malloc(sizeof(struct VarAddr_));
        temp->var_no = var_no;
        temp->addr = addr;
        varaddr_end->next = temp;
        varaddr_end = temp;
    }  
}

int returnaddr(int var_no)
{
    VarAddr temp = varaddr_begin;
    while(temp)
    {
        if(temp->var_no == var_no)
            return temp->addr;
        temp = temp->next;
    }
    return 0;
}

int getaddr(Operand op)
{
    if(op->addr >= 0x10010020)
        return op->addr;
    else
    {
        op->addr = returnaddr(op->u.var_no);
        if(op->addr == 0)
        {
            op->addr = addr_begin;
            addr_begin += 4;
            insert2VarAddr(op->u.var_no, op->addr);
        }
        return op->addr;
    }
}

void asmcode(Intercode code, FILE* f)
{
    if(code)
    {
        if(code->kind == LABEL_ )
            fprintf(f, "label%d:\n", code->u.label.labelnum);
        else if(code->kind == FUNCTION_)
        {
            if(strcmp(code->u.func.funcname, "main"))
            {
                infunc = 1;
            }
            param_num = 0;
            fprintf(f, "%s:\n", code->u.func.funcname);
        }
        else if(code->kind == ASSIGN_ && code->u.assign.left && code->u.assign.right)
        {
            if(infunc && code->u.assign.left->kind == VARIABLE)
                insert2SaveAddr(getaddr(code->u.assign.left));
            if(code->u.assign.left->kind == VARIABLE && code->u.assign.right->kind == VARIABLE)
            {
                int y_addr = getaddr(code->u.assign.right); 
                int x_addr = getaddr(code->u.assign.left);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.assign.left->kind == VARIABLE && code->u.assign.right->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.assign.left);
                fprintf(f, "  li $t1, %s\n", code->u.assign.right->u.value);
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.assign.left->kind == VARIABLE && code->u.assign.right->kind == GETADDRESS)
            {
                int y_addr = getaddr(code->u.assign.right); 
                int x_addr = getaddr(code->u.assign.left);
                fprintf(f, "la $t1, 0x%x\n", y_addr);
                fprintf(f, "sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.assign.left->kind == VARIABLE && code->u.assign.right->kind == GETVALUE)
            {
                int y_addr = getaddr(code->u.assign.right); 
                int x_addr = getaddr(code->u.assign.left);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  lw $t2, 0($t1)\n");
                fprintf(f, "  sw $t2, 0x%x\n", x_addr);
            }
            else if(code->u.assign.left->kind == GETVALUE && code->u.assign.right->kind == VARIABLE)
            {
                int y_addr = getaddr(code->u.assign.right); 
                int x_addr = getaddr(code->u.assign.left);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  lw $t2, 0x%x\n", x_addr);
                fprintf(f, "  sw $t1, 0($t2)\n");
            }
        }
        else if(code->kind == ADD_ && code->u.binop.result && code->u.binop.op1 && code->u.binop.op2)
        {
            int x_addr = getaddr(code->u.binop.result);
            if(infunc && code->u.binop.result->kind == VARIABLE)
                insert2SaveAddr(getaddr(code->u.assign.left));

            if(code->u.binop.op1->kind == VARIABLE)
            {
                int y_addr = getaddr(code->u.binop.op1);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT)
            {
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
            }
            else if(code->u.binop.op1->kind == GETADDRESS)
            {
                int y_addr = getaddr(code->u.binop.op1);
                fprintf(f, "  la $t1, 0x%x\n", y_addr);
            }

            if(code->u.binop.op2->kind == VARIABLE)
            {
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
            }
            else if(code->u.binop.op2->kind == CONSTANT)
            {
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
            }
            else if(code->u.binop.op2->kind == GETADDRESS)
            {
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  la $t2, 0x%x\n", z_addr);
            }
            fprintf(f, "  add $t1, $t1, $t2\n");    
            fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            
        }
        else if(code->kind == SUB_ && code->u.binop.result && code->u.binop.op1 && code->u.binop.op2)
        {
            if(infunc && code->u.binop.result->kind == VARIABLE)
                insert2SaveAddr(getaddr(code->u.assign.left));
            if(code->u.binop.op1->kind == VARIABLE && code->u.binop.op2->kind == VARIABLE)
            {
                int x_addr = getaddr(code->u.binop.result);
                int y_addr = getaddr(code->u.binop.op1);
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
                fprintf(f, "  sub $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == VARIABLE && code->u.binop.op2->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.binop.result);
                int y_addr = getaddr(code->u.binop.op1);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
                fprintf(f, "  sub $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT && code->u.binop.op2->kind == VARIABLE)
            {
                int x_addr = getaddr(code->u.binop.result);
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
                fprintf(f, "  sub $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT && code->u.binop.op2->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.binop.result);
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
                fprintf(f, "  sub $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
        }
        else if(code->kind == MUL_ && code->u.binop.result && code->u.binop.op1 && code->u.binop.op2)
        {
            if(infunc && code->u.binop.result->kind == VARIABLE)
                insert2SaveAddr(getaddr(code->u.assign.left));
            if(code->u.binop.op1->kind == VARIABLE && code->u.binop.op2->kind == VARIABLE)
            {
                int x_addr = getaddr(code->u.binop.result);
                int y_addr = getaddr(code->u.binop.op1);
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
                fprintf(f, "  mul $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == VARIABLE && code->u.binop.op2->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.binop.result);
                int y_addr = getaddr(code->u.binop.op1);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
                fprintf(f, "  mul $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT && code->u.binop.op2->kind == VARIABLE)
            {
                int x_addr = getaddr(code->u.binop.result);
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
                fprintf(f, "  mul $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT && code->u.binop.op2->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.binop.result);
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
                fprintf(f, "  mul $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
        }
        else if(code->kind == DIV_ && code->u.binop.result && code->u.binop.op1 && code->u.binop.op2)
        {
            if(infunc && code->u.binop.result->kind == VARIABLE)
                insert2SaveAddr(getaddr(code->u.assign.left));
            if(code->u.binop.op1->kind == VARIABLE && code->u.binop.op2->kind == VARIABLE)
            {
                int x_addr = getaddr(code->u.binop.result);
                int y_addr = getaddr(code->u.binop.op1);
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
                fprintf(f, "  div $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == VARIABLE && code->u.binop.op2->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.binop.result);
                int y_addr = getaddr(code->u.binop.op1);
                fprintf(f, "  lw $t1, 0x%x\n", y_addr);
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
                fprintf(f, "  div $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT && code->u.binop.op2->kind == VARIABLE)
            {
                int x_addr = getaddr(code->u.binop.result);
                int z_addr = getaddr(code->u.binop.op2);
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
                fprintf(f, "  lw $t2, 0x%x\n", z_addr);
                fprintf(f, "  div $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
            else if(code->u.binop.op1->kind == CONSTANT && code->u.binop.op2->kind == CONSTANT)
            {
                int x_addr = getaddr(code->u.binop.result);
                fprintf(f, "  li $t1, %s\n", code->u.binop.op1->u.value);
                fprintf(f, "  li $t2, %s\n", code->u.binop.op2->u.value);
                fprintf(f, "  div $t1, $t1, $t2\n");
                fprintf(f, "  sw $t1, 0x%x\n", x_addr);
            }
        }
        else if(code->kind == GOTO_)
            fprintf(f, "  j label%d\n", code->u.goto_.labelnum);
        else if(code->kind == CONDGOTO_ && code->u.condgoto.op1 && code->u.condgoto.op2)
        {
            int x_addr = getaddr(code->u.condgoto.op1);
            int y_addr = getaddr(code->u.condgoto.op2);
            fprintf(f, "  lw $t1, 0x%x\n", x_addr);
            fprintf(f, "  lw $t2, 0x%x\n", y_addr);
            if(!strcmp(code->u.condgoto.relop, "=="))
                fprintf(f, "  beq $t1, $t2, label%d\n", code->u.condgoto.labelnum);
            else if(!strcmp(code->u.condgoto.relop, "!="))
                fprintf(f, "  bne $t1, $t2, label%d\n", code->u.condgoto.labelnum);
            else if(!strcmp(code->u.condgoto.relop, ">"))
                fprintf(f, "  bgt $t1, $t2, label%d\n", code->u.condgoto.labelnum);
            else if(!strcmp(code->u.condgoto.relop, "<"))
                fprintf(f, "  blt $t1, $t2, label%d\n", code->u.condgoto.labelnum);
            else if(!strcmp(code->u.condgoto.relop, ">="))
                fprintf(f, "  bge $t1, $t2, label%d\n", code->u.condgoto.labelnum);
            else if(!strcmp(code->u.condgoto.relop, "<="))
                fprintf(f, "  ble $t1, $t2, label%d\n", code->u.condgoto.labelnum);
        }
        else if(code->kind == RETURN_ && code->u.return_.op)
        {
            param_num = 0;
            int x_addr = getaddr(code->u.return_.op);
            fprintf(f, "  lw $v0, 0x%x\n", x_addr);
            fprintf(f, "  jr $ra\n");
        }
        else if(code->kind == ARG_ && code->u.arg.op)
        {
            if(firstin)
            {   
                param_num = -1;
                Intercode temp = code;
                while(temp && temp->kind == ARG_)
                {
                    param_num ++;
                    temp = temp->next;
                }
                firstin = 0;
            }
            int x_addr = getaddr(code->u.arg.op);
            fprintf(f, "  lw $a%d, 0x%x\n", param_num--, x_addr);
        }
        else if(code->kind == PARAM_ && code->u.param.op)
        {
            int x_addr = getaddr(code->u.arg.op);
            fprintf(f, "  sw $a%d, 0x%x\n", param_num++, x_addr);
        }
        else if(code->kind == READ_ && code->u.read.op)
        {
            int x_addr = getaddr(code->u.call.result);
            fprintf(f, "  addi $sp, $sp, -4\n");
            fprintf(f, "  sw $ra, 0($sp)\n");
            fprintf(f, "  jal read\n");
            fprintf(f, "  lw $ra, 0($sp)\n");
            fprintf(f, "  addi $sp, $sp, 4\n");
            fprintf(f, "  sw $v0, 0x%x\n", x_addr);
        }
        else if(code->kind == WRITE_ && code->u.write.op)
        {
            int x_addr = getaddr(code->u.call.result);
            fprintf(f, "  lw $a0, 0x%x\n", x_addr);
            fprintf(f, "  addi $sp, $sp, -4\n");
            fprintf(f, "  sw $ra, 0($sp)\n");
            fprintf(f, "  jal write\n");
            fprintf(f, "  lw $ra, 0($sp)\n");
            fprintf(f, "  addi $sp, $sp, 4\n");
        }
        else if(code->kind == DEC_ && code->u.dec.op)
        {
            code->u.dec.op->addr = addr_begin;
            addr_begin += 4*code->u.dec.size;
            insert2VarAddr(code->u.dec.op->u.var_no, code->u.dec.op->addr);
        }
        else if(code->kind == CALL_ && code->u.call.result)
        {
            firstin = 1;
            int x_addr = getaddr(code->u.call.result);
            fprintf(f, "  addi $sp, $sp, %d\n", -4*(addrcount+1));
            SaveAddr temp = saveaddr_begin;
            int count = 0;
            while(temp)
            {
                fprintf(f, "  lw $s1, 0x%x\n", temp->addr);
                fprintf(f, "  sw $s1, %d($sp)\n", 4*count);
                count++;
                temp = temp->next;
            }
            fprintf(f, "  sw $ra, %d($sp)\n", 4*count);
            fprintf(f, "  jal %s\n", code->u.call.funcname);
            temp = saveaddr_begin;
            count = 0;
            while(temp)
            {
                fprintf(f, "  lw $s1, %d($sp)\n", 4*count);
                fprintf(f, "  sw $s1, 0x%x\n", temp->addr);
                count++;
                temp = temp->next;
            }
            fprintf(f, "  lw $ra, %d($sp)\n", 4*count);
            fprintf(f, "  addi $sp, $sp, %d\n", 4*(addrcount+1));
            fprintf(f, "  sw $v0, 0x%x\n", x_addr);
            saveaddr_begin = NULL;
        }
        asmcode(code->next, f);
    }
}
