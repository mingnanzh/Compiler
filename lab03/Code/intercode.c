#include "intercode.h"

Intercode GetTail(Intercode code)
{
    if(code->next)
        return GetTail(code->next);
    else
        return code;
}

Intercode LinkCode(Intercode code1, Intercode code2)
{
    if(code1)
    {
        Intercode code1_tail = GetTail(code1);
        code1_tail->next = code2;
        code2->prev = code1_tail;
    }
    return code1;
}

Intercode translate_extdef(struct treenode* extdef)
{
    if(!strcmp(extdef->child->name, "Specifier") && !strcmp(extdef->child->sibling->name, "SEMI"))
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
