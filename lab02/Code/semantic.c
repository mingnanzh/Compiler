#include "semantic.h"

//extern struct treenode* root;

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union
    {
        // 基本类型
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct{
            int size;
            Type elem;
        }array;
        // 结构体类型信息是一个链表
        FieldList structure;
    }u;
};

struct FieldList_{
    char* name;
    Type type;
    FieldList tail;
};

struct VarTable{
    char* name;
    Type type;
    struct VarTable* next;
};

struct VarTable* head;
struct VarTable* end;


void addVar2Table(char* name, Type type){
    if(!head){
        head = (struct VarTable*)malloc(sizeof(struct VarTable));
        head -> name = (char*)malloc(strlen(name)+1);
        strcpy(head -> name, name);
        head -> type = type;
        end = head;
    }
    else{
        struct VarTable* t = (struct VarTable*)malloc(sizeof(struct VarTable));
        t -> name = (char*)malloc(strlen(name)+1);
        strcpy(t -> name, name);
        t -> type = type;
        end -> next = t;
        end = t;
        
    }
}

void shit(struct treenode* node, Type type){
    if(!strcmp(node -> name, "ID")){
        addVar2Table(node -> value, type);
        printf("add variabel '%s' of type [%d]\n", node -> value, type -> kind);
        //printf("size: %d*%d\n", type -> u.array.size, type -> u.array.elem ->u.array.size);
    }
    else if(!strcmp(node -> name, "VarDec")){
        Type type_temp = (Type)malloc(sizeof(struct Type_));
        type_temp -> kind = ARRAY;
        type_temp -> u.array.size = atoi(node -> sibling -> sibling -> value);
        type_temp -> u.array.elem = type;
        shit(node -> child, type_temp);  
    }
}

void scanning(struct treenode* r){
    if(r && r -> type <= 1){
        struct treenode* temp = r -> child;
        Type type = (Type)malloc(sizeof(struct Type_));

        // "Def"
        if(!strcmp(r -> name,"Def")){
            // int or float
            if(!strcmp(r -> child -> child -> name, "TYPE")){
                if(!strcmp(r -> child -> child -> value, "int")){
                    type -> kind = BASIC;
                    type -> u.basic = 0;
                }
                else if(!strcmp(r -> child -> child -> value, "float")){
                    type -> kind = BASIC;
                    type -> u.basic = 1;
                }
                struct treenode* declist = r -> child -> sibling;    
                while(declist){
                    shit(declist -> child -> child -> child, type);
                    if(!declist -> child -> sibling)
                        declist = NULL;
                    else
                        declist = declist -> child -> sibling -> sibling;
                }
            }
            return;
        }

        // "ExtDef"
        if(!strcmp(r -> name,"ExtDef")){
            //global variable
            if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "ExtDecList") && !strcmp(r -> child -> sibling -> sibling -> name, "SEMI")){
                // int or float
                if(!strcmp(r -> child -> child -> name, "TYPE")){
                    if(!strcmp(r -> child -> child -> value, "int")){
                        type -> kind = BASIC;
                        type -> u.basic = 0;
                    }
                    else if(!strcmp(r -> child -> child -> value, "float")){
                        type -> kind = BASIC;
                        type -> u.basic = 1;
                    }
                    struct treenode* extdeclist = r -> child -> sibling;    
                    while(extdeclist){
                        shit(extdeclist -> child -> child, type);
                        if(!extdeclist -> child -> sibling)
                            extdeclist = NULL;
                        else
                            extdeclist = extdeclist -> child -> sibling -> sibling;
                    }
                }
            }
            // define STRUCTURE
            else if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "CompSt")){
                
            }
            else if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "FunDec") && !strcmp(r -> child -> sibling -> sibling -> name, "CompSt")){

            }
            //return;
        }

        while(temp){
            scanning(temp);
            temp=temp->sibling;
        }
    }
}