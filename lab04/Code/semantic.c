#include "semantic.h"

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        int basic;
        struct{
            int size;
            Type elem;
        }array;
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

struct StruTable{
    char* name;
    Type type;
    struct StruTable* next;
};

struct FuncTable{
    char* name;
    Type type;
    int varnum;
    FieldList structure;
    struct FuncTable* next;
};

struct VarTable* head;
struct VarTable* end;

struct StruTable* StruTable_head;
struct StruTable* StruTable_end;

struct FuncTable* FuncTable_head;
struct FuncTable* FuncTable_end;

int IfOrWhileOrReturn = 0;

Type functypenow;

FieldList getstructure(struct treenode* node, FieldList fks);

struct StruTable* findStru(char* name);

Type gettype(struct treenode* specifier);

int check(struct treenode* exp);

Type returntype(struct treenode* exp);

struct VarTable* findVar(char* name);

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

void addStru2Table(char* name, Type type){
    if(!StruTable_head){
        StruTable_head = (struct StruTable*)malloc(sizeof(struct StruTable));
        StruTable_head -> name = (char*)malloc(strlen(name)+1);
        strcpy(StruTable_head -> name, name);
        StruTable_head -> type = type;
        StruTable_end = StruTable_head;
    }
    else{
        struct StruTable* t = (struct StruTable*)malloc(sizeof(struct StruTable));
        t -> name = (char*)malloc(strlen(name)+1);
        strcpy(t -> name, name);
        t -> type = type;
        StruTable_end -> next = t;
        StruTable_end = t;  
    }
}

void addFunc2Table(char* name, Type type, int varnum, FieldList structure){
    if(!strcmp(name, "read")){
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->u.basic = 0;
    }
    else if(!strcmp(name, "write")){
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        type->u.basic = 0;
        structure = (FieldList)malloc(sizeof(struct FieldList_));
        structure->type = type;
    }
    if(!FuncTable_head){
        FuncTable_head = (struct FuncTable *)malloc(sizeof(struct FuncTable));
        FuncTable_head -> name = (char*)malloc(strlen(name)+1);
        strcpy(FuncTable_head -> name, name);
        FuncTable_head -> type = type;
        FuncTable_head -> varnum = varnum;
        FuncTable_head -> structure = structure;
        FuncTable_end = FuncTable_head;
    }
    else{
        struct FuncTable* t = (struct FuncTable *)malloc(sizeof(struct FuncTable));
        t -> name = (char*)malloc(strlen(name)+1);
        strcpy(t -> name, name);
        t -> type = type;
        t -> varnum = varnum;
        t -> structure = structure;
        FuncTable_end -> next = t;
        FuncTable_end = t;
    }
}

void insert(struct treenode* node, Type type){
    if(!strcmp(node -> name, "ID")){
        if(findVar(node -> value) || findStru(node -> value)){
            printf("Error type 3 at Line %d: Redefined Variable \"%s\".\n", node -> lineno, node -> value);
            addVar2Table(node -> value, type);
        }
        else{
            addVar2Table(node -> value, type);
            //printf("add variable '%s' of type [%d]\n", node -> value, type -> kind);
            //printf("size: %d*%d\n", type -> u.array.size, type -> u.array.elem ->u.array.size);
        }
        
    }
    else if(!strcmp(node -> name, "VarDec")){
        Type type_temp = (Type)malloc(sizeof(struct Type_));
        type_temp -> kind = ARRAY;
        type_temp -> u.array.size = atoi(node -> sibling -> sibling -> value);
        type_temp -> u.array.elem = type;
        insert(node -> child, type_temp);  
    }
}

Type insert2(struct treenode* node, Type type){
    if(!strcmp(node -> name, "ID")){
        if(findVar(node -> value) || findStru(node -> value)){
            printf("Error type 3 at Line %d: Redefined Variable \"%s\".\n", node -> lineno, node -> value);
            addVar2Table(node -> value, type);
            return type;
        }
        else{
            addVar2Table(node -> value, type);
            //printf("add variable '%s' of type [%d]\n", node -> value, type -> kind);
            return type;
            //printf("size: %d*%d\n", type -> u.array.size, type -> u.array.elem ->u.array.size);
        }
        
    }
    else if(!strcmp(node -> name, "VarDec")){
        Type type_temp = (Type)malloc(sizeof(struct Type_));
        type_temp -> kind = ARRAY;
        type_temp -> u.array.size = atoi(node -> sibling -> sibling -> value);
        type_temp -> u.array.elem = type;
        return insert2(node -> child, type_temp); 
    }
}

char* getid(struct treenode* node){
    if(!strcmp(node -> name, "ID")){
        return node -> value;
        //printf("size: %d*%d\n", type -> u.array.size, type -> u.array.elem ->u.array.size);
    }
    else
        return getid(node -> child); 
}

struct StruTable* findStru(char* name){
    struct StruTable* temp = StruTable_head;
    while(temp){
        if(!strcmp(temp -> name, name))
            return temp;
        else
            temp = temp -> next;
    }
    return NULL;
}

struct VarTable* findVar(char* name){
    struct VarTable* temp = head;
    while(temp){
        if(!strcmp(temp -> name, name))
            return temp;
        else
            temp = temp -> next;
    }
    return NULL;
}

struct FuncTable* findFunc(char* name){
    struct FuncTable* temp = FuncTable_head;
    while(temp){
        if(!strcmp(temp -> name, name))
            return temp;
        else
            temp = temp -> next;
    }
    return NULL;
}

Type findField(char* name, FieldList stru){
    FieldList temp = stru;
    while(temp){
        if(!strcmp(temp -> name, name))
            return temp -> type;
        else
            temp = temp -> tail;
    }
    return NULL;
}

FieldList getstructure(struct treenode* node, FieldList fks){
    if(node -> type == 2){
        return NULL;
    }
    else{
        if(!strcmp(node -> child -> name, "Def")){
            struct treenode* def = node -> child;
            Type type = gettype(def -> child);
            struct treenode* declist = def -> child -> sibling;    
            FieldList ks = NULL;
            FieldList js = NULL;
            while(declist){
                if(declist -> child -> child -> sibling){
                    printf("Error type 15 at Line %d: Initialized variable \"%s\" in struct.\n", declist -> child -> child -> lineno, getid(declist -> child -> child));
                }
                FieldList structureField = (FieldList)malloc(sizeof(struct FieldList_));
                structureField -> name = (char*)malloc(strlen(getid(declist -> child -> child))+1);
                strcpy(structureField -> name, getid(declist -> child -> child));
                if(findField(getid(declist -> child -> child),fks)){
                    printf("Error type 15 at Line %d: Redefined field \"%s\".\n",declist -> child -> child -> lineno, getid(declist -> child -> child));
                }else{
                    structureField -> type = insert2(declist -> child -> child -> child, type);
                }
                if(!ks){
                    ks = structureField;
                    js = ks;
                    if(!fks)
                        fks = ks;
                }
                else{
                    js -> tail = structureField;
                    js = js -> tail;
                }
                if(!declist -> child -> sibling)
                    declist = NULL;
                else
                    declist = declist -> child -> sibling -> sibling;
            }
            js -> tail = getstructure(def -> sibling, fks);
            return ks;
        }
    }   
}

Type gettype(struct treenode* specifier){
    Type type = (Type)malloc(sizeof(struct Type_));
    // int or float
    if(!strcmp(specifier -> child -> name, "TYPE")){
        if(!strcmp(specifier -> child -> value, "int")){
            type -> kind = BASIC;
            type -> u.basic = 0;
        }
        else if(!strcmp(specifier -> child -> value, "float")){
            type -> kind = BASIC;
            type -> u.basic = 1;
        }
    }
    // STRUCT
    else if(!strcmp(specifier -> child -> name, "StructSpecifier")){
        //STRUCT OptTag LC DefList RC
        if(!strcmp(specifier -> child -> child -> name, "STRUCT") && !strcmp(specifier -> child -> child -> sibling -> name, "OptTag")){
            type -> kind = STRUCTURE;
            // OptTag != empty
            if(specifier -> child -> child -> sibling -> type != 2){
                struct treenode* deflist = specifier -> child -> child -> sibling -> sibling -> sibling;
                type -> u.structure = getstructure(deflist, NULL);
                if(findStru(specifier -> child -> child -> sibling -> child -> value)){
                    printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", specifier -> child -> child -> sibling -> child -> lineno, specifier -> child -> child -> sibling -> child -> value);
                }
                else{
                    //printf("add structure ''%s''.\n", specifier -> child -> child -> sibling -> child -> value);
                    addStru2Table(specifier -> child -> child -> sibling -> child -> value, type);
                }
            }
            // OptTag == empty
            else{
                struct treenode* deflist = specifier -> child -> child -> sibling -> sibling -> sibling;
                type -> u.structure = getstructure(deflist, NULL);
            }
        }
        // STRUCT Tag
        else if(!strcmp(specifier -> child -> child -> name, "STRUCT") && !strcmp(specifier -> child -> child -> sibling -> name, "Tag")){
            if(!findStru(specifier -> child -> child -> sibling -> child -> value)){
                printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", specifier -> child -> child -> sibling -> child -> lineno, specifier -> child -> child -> sibling -> child -> value);
            }else{
                type = findStru(specifier -> child -> child -> sibling -> child -> value) -> type;
            }
        }
    }
    return type;
}

int typecmp(Type t1, Type t2){
    if(t1 -> kind == ARRAY && t2 -> kind == ARRAY){
        Type temp1 = t1;
        Type temp2 = t2;
        while(t1 && t2){
            if(temp1 -> kind == ARRAY && temp2 -> kind == ARRAY){
                temp1 = temp1 -> u.array.elem;
                temp2 = temp2 -> u.array.elem;
            }
            else if(temp1 -> kind == temp2 -> kind){
                return typecmp(temp1, temp2);
            }
            else
                return 0;
        }
        return 0;
    }
    else if(t1 -> kind == BASIC && t2 -> kind == BASIC){
        return t1 -> u.basic == t2 -> u.basic;
    }
    else if(t1 -> kind == STRUCTURE && t2 -> kind == STRUCTURE){
        return t1 -> u.structure == t2 -> u.structure;
    }
    else
        return 0;
}

int isLeft(struct treenode* exp){
    if(!strcmp(exp -> child -> name, "ID") && !(exp -> child -> sibling))
        return 1;
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "LB") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp") && !strcmp(exp -> child -> sibling -> sibling -> sibling -> name, "RB"))
        return 1;
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "DOT") && !strcmp(exp -> child -> sibling -> sibling -> name, "ID"))
        return 1;
    else 
        return 0;
}

int checkarguments(struct treenode* exp){
    FieldList arg1 = findFunc(exp -> child -> value) -> structure;
    struct treenode* arg2 = exp -> child -> sibling -> sibling -> child;
    while(1){
        if(arg1 == NULL && arg2 == NULL)
            return 1;
        else if(arg1 != NULL && arg2 == NULL)
            return 0;
        else if(arg1 == NULL && arg2 != NULL)
            return 0;
        else{
            if(!check(arg2))
                return 0;
            else{
                if(typecmp(arg1 -> type, returntype(arg2))){
                    arg1 = arg1 -> tail;
                    if(arg2 -> sibling == NULL)
                        arg2 = NULL;
                    else 
                        arg2 = arg2 -> sibling -> sibling -> child;
                }
                else
                    return 0;
            }
        }
    }
    return 1;
}

int check(struct treenode* exp){
    if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "ASSIGNOP") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(!isLeft(exp -> child)){
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", exp -> child ->lineno);
                return 0;
            }
            else{
                if(!typecmp(returntype(exp -> child), returntype(exp -> child -> sibling -> sibling))){
                    printf("Error type 5 at Line %d: Type mismatched for assignment.\n", exp -> lineno);
                    return 0;
                }
            }
            return 1;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "AND") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(returntype(exp -> child) -> u.basic == 0 && returntype(exp -> child -> sibling -> sibling) -> u.basic == 0)
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "OR") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(returntype(exp -> child) -> u.basic == 0 && returntype(exp -> child -> sibling -> sibling) -> u.basic == 0)
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "RELOP") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(typecmp(returntype(exp -> child), returntype(exp -> child -> sibling -> sibling)))
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "PLUS") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(typecmp(returntype(exp -> child), returntype(exp -> child -> sibling -> sibling)))
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "MINUS") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(typecmp(returntype(exp -> child), returntype(exp -> child -> sibling -> sibling)))
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "STAR") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(typecmp(returntype(exp -> child), returntype(exp -> child -> sibling -> sibling)))
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "DIV") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind == BASIC && returntype(exp -> child -> sibling -> sibling) -> kind == BASIC){
                if(typecmp(returntype(exp -> child), returntype(exp -> child -> sibling -> sibling)))
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "LP") && !strcmp(exp -> child -> sibling -> name, "Exp") && !strcmp(exp -> child -> sibling -> sibling -> name, "RP")){
        if(check(exp -> child -> sibling)){
            return 1;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "MINUS") && !strcmp(exp -> child -> sibling -> name, "Exp")){
        if(check(exp -> child -> sibling)){
            if(returntype(exp -> child -> sibling) -> kind == BASIC){
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "NOT") && !strcmp(exp -> child -> sibling -> name, "Exp")){
        if(check(exp -> child -> sibling)){
            if(returntype(exp -> child -> sibling) -> kind == BASIC){
                if(returntype(exp -> child -> sibling) -> u.basic == 0)
                    return 1;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp -> lineno);
            return 0;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "ID") && !(exp -> child -> sibling)){
        if(findVar(exp -> child -> value))
            return 1;
        else
        {
            printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", exp -> child -> lineno, exp -> child -> value);
            return 0;
        }
    }
    else if(!strcmp(exp -> child -> name, "ID") && !strcmp(exp -> child -> sibling -> name, "LP") && !strcmp(exp -> child -> sibling -> sibling -> name, "Args") && !strcmp(exp -> child -> sibling -> sibling -> sibling -> name, "RP")){
        if(!findFunc(exp -> child ->value) && !findVar(exp -> child ->value)){
            printf("Error type 2 at Line %d: Undefined function \"%s\".\n", exp -> child -> lineno, exp -> child -> value);
            return 0;
        }
        else if(!findFunc(exp -> child ->value) && findVar(exp -> child ->value)){
            printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp -> child -> lineno, exp -> child -> value);
        }
        else if(!checkarguments(exp)){
            printf("Error type 9 at Line %d: Function \"%s\" is not applicable for the following arguments.\n", exp -> child -> lineno, exp -> child -> value);
            return 0;
        }
        return 1;
    }
    else if(!strcmp(exp -> child -> name, "ID") && !strcmp(exp -> child -> sibling -> name, "LP") && !strcmp(exp -> child -> sibling -> sibling -> name, "RP")){
        if(!findFunc(exp -> child ->value) && !findVar(exp -> child ->value)){
            printf("Error type 2 at Line %d: Undefined function \"%s\".\n", exp -> child -> lineno, exp -> child -> value);
            return 0;
        }
        else if(!findFunc(exp -> child ->value) && findVar(exp -> child ->value)){
            printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp -> child -> lineno, exp -> child -> value);
            return 0;
        }
        else if(findFunc(exp -> child -> value) -> varnum != 0){
            printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments.\n", exp -> child -> lineno, exp -> child -> value);
            return 0;
        }
        return 1;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "LB") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp") && !strcmp(exp -> child -> sibling -> sibling -> sibling -> name, "RB")){
        int t1 = check(exp -> child);
        int t2 = check(exp -> child -> sibling -> sibling);
        if(t1 && t2){
            if(returntype(exp -> child) -> kind != ARRAY){
                printf("Error type 10 at Line %d: Illegel use of \"[]\".\n", exp -> child -> lineno);
                return 0;
            }
            if(returntype(exp -> child -> sibling -> sibling) -> kind != BASIC || returntype(exp -> child -> sibling -> sibling) -> u.basic != 0){
                printf("Error type 12 at Line %d: Expression between \"[\" and \"]\" is not an integer.\n", exp -> child -> lineno);
                return 0;
            }
            return 1;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "DOT") && !strcmp(exp -> child -> sibling -> sibling -> name, "ID")){
        if(check(exp -> child)){
            if(returntype(exp -> child) -> kind != STRUCTURE){
                printf("Error type 13 at Line %d: Illegal use of \".\".\n", exp -> child -> sibling -> lineno);
                return 0;
            }
            else{
                if(!findField(exp -> child -> sibling -> sibling -> value, returntype(exp -> child) -> u.structure)){
                    printf("Error type 14 at Line %d: Non-existent field \"%s\".\n", exp -> child -> sibling -> sibling -> lineno, exp -> child -> sibling -> sibling -> value);
                    return 0;
                }
            }
            return 1;
        }
        else
            return 0;
    }
    else if(!strcmp(exp -> child -> name, "INT")){
        return 1;
    }
    else if(!strcmp(exp -> child -> name, "FLOAT")){
        return 1;
    }
}

Type returntype(struct treenode* exp){
    if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "ASSIGNOP") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "AND") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "OR") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "RELOP") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        Type INT = (Type)malloc(sizeof(struct Type_));
        INT -> kind = BASIC;
        INT -> u.basic = 0;
        return INT;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "PLUS") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "MINUS") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "STAR") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "DIV") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp")){
        return returntype(exp -> child);
    }
    else if(!strcmp(exp -> child -> name, "LP") && !strcmp(exp -> child -> sibling -> name, "Exp") && !strcmp(exp -> child -> sibling -> sibling -> name, "RP")){
        return returntype(exp -> child -> sibling);
    }
    else if(!strcmp(exp -> child -> name, "MINUS") && !strcmp(exp -> child -> sibling -> name, "Exp")){
        return returntype(exp -> child -> sibling);
    }
    else if(!strcmp(exp -> child -> name, "NOT") && !strcmp(exp -> child -> sibling -> name, "Exp")){
        return returntype(exp -> child -> sibling);
    }
    else if(!strcmp(exp -> child -> name, "ID") && !(exp -> child -> sibling)){
        return findVar(exp -> child -> value) -> type;
    }
    else if(!strcmp(exp -> child -> name, "ID") && !strcmp(exp -> child -> sibling -> name, "LP") && !strcmp(exp -> child -> sibling -> sibling -> name, "Args") && !strcmp(exp -> child -> sibling -> sibling -> sibling -> name, "RP")){
        return findFunc(exp -> child -> value) -> type;
    }
    else if(!strcmp(exp -> child -> name, "ID") && !strcmp(exp -> child -> sibling -> name, "LP") && !strcmp(exp -> child -> sibling -> sibling -> name, "RP")){
        return findFunc(exp -> child -> value) -> type;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "LB") && !strcmp(exp -> child -> sibling -> sibling -> name, "Exp") && !strcmp(exp -> child -> sibling -> sibling -> sibling -> name, "RB")){
        return returntype(exp -> child) -> u.array.elem;
    }
    else if(!strcmp(exp -> child -> name, "Exp") && !strcmp(exp -> child -> sibling -> name, "DOT") && !strcmp(exp -> child -> sibling -> sibling -> name, "ID")){
        return findField(exp -> child -> sibling -> sibling -> value, returntype(exp -> child) -> u.structure);
    }
    else if(!strcmp(exp -> child -> name, "INT")){
        Type INT = (Type)malloc(sizeof(struct Type_));
        INT -> kind = BASIC;
        INT -> u.basic = 0;
        return INT;
    }
    else if(!strcmp(exp -> child -> name, "FLOAT")){
        Type FLOAT = (Type)malloc(sizeof(struct Type_));
        FLOAT -> kind = BASIC;
        FLOAT -> u.basic = 1;
        return FLOAT;
    }
}

void scanning(struct treenode* r){
    if(r && r -> type <= 1){
        struct treenode* temp = r -> child;

        // "Def"
        if(!strcmp(r -> name,"Def")){
            Type type = gettype(r -> child);
            struct treenode* declist = r -> child -> sibling; 
            while(declist){
                insert(declist -> child -> child -> child, type);
                if(declist -> child -> child -> sibling){
                    if(check(declist -> child -> child -> sibling ->sibling))
                        if(!typecmp(type,returntype(declist -> child -> child -> sibling ->sibling)))
                            printf("Error type 5 at Line %d: Type mismatched for assignment.\n", declist -> child -> child -> lineno);
                }
                if(!declist -> child -> sibling)
                    declist = NULL;
                else
                    declist = declist -> child -> sibling -> sibling;
            };
            return;
        }

        // "ExtDef"
        if(!strcmp(r -> name,"ExtDef")){
            //global variable
            if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "ExtDecList") && !strcmp(r -> child -> sibling -> sibling -> name, "SEMI")){
                Type type = gettype(r -> child);
                struct treenode* extdeclist = r -> child -> sibling;    
                while(extdeclist){
                    insert(extdeclist -> child -> child, type);
                    if(!extdeclist -> child -> sibling)
                        extdeclist = NULL;
                    else
                        extdeclist = extdeclist -> child -> sibling -> sibling;
                }
                return;
            }
            
            // STRUCTURE
            else if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "SEMI")){
                if(!strcmp(r -> child -> child -> name, "StructSpecifier")){
                    //STRUCT OptTag LC DefList RC
                    if(!strcmp(r -> child -> child -> child -> name, "STRUCT") && !strcmp(r -> child -> child -> child -> sibling -> name, "OptTag")){
                        Type type = (Type)malloc(sizeof(struct Type_));
                        type -> kind = STRUCTURE;
                        // OptTag != empty
                        if(r -> child -> child -> child -> sibling -> type != 2){
                            //printf("%s %s\n", r -> child -> child -> child -> value, r -> child -> child -> child -> sibling -> child -> value);
                            struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                            type -> u.structure = getstructure(deflist, NULL);
                            if(findStru(r -> child -> child -> child -> sibling -> child -> value)){
                                printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", r -> child -> child -> child -> sibling -> child -> lineno, r -> child -> child -> child -> sibling -> child -> value);
                            }
                            else{
                                //printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                                addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                            }
                            
                        }
                        else{
                            struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                            type -> u.structure = getstructure(deflist, NULL);
                            //printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                            //addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                        }
                    }
                    // STRUCT Tag
                }
                return;
            }
            
            // FUNCTION
            else if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "FunDec") && !strcmp(r -> child -> sibling -> sibling -> name, "CompSt")){
                Type type = gettype(r -> child);
                functypenow = type;
                struct treenode* fundec = r -> child -> sibling;
                if(!strcmp(fundec -> child -> name, "ID") && !strcmp(fundec -> child -> sibling -> sibling -> name, "VarList")){
                    struct treenode* varlist = fundec -> child -> sibling -> sibling;
                    FieldList ks = NULL;
                    FieldList js = NULL;
                    int count = 0;
                    while(varlist){
                        Type type_ = gettype(varlist -> child -> child);
                        FieldList structureField = (FieldList)malloc(sizeof(struct FieldList_));
                        structureField -> name = (char*)malloc(strlen(getid(varlist -> child -> child -> sibling))+1);
                        structureField -> type = insert2(varlist -> child -> child -> sibling -> child, type_);
                        if(!ks){
                            ks = structureField;
                            js = ks;
                        }
                        else{
                            js -> tail = structureField;
                            js = js -> tail;
                        }
                        if(!varlist -> child -> sibling)
                            varlist = NULL;
                        else
                            varlist = varlist -> child -> sibling -> sibling;
                        count += 1;
                    }
                    if(findFunc(fundec -> child -> value)){
                        printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec -> child -> lineno, fundec -> child ->value);
                    }
                    else{
                        //printf("add function {%s} of type [%d]\n", fundec -> child -> value, type -> kind);
                        addFunc2Table(fundec -> child -> value, type, count, ks);
                    }
                }
                else if(!strcmp(fundec -> child -> name, "ID") && !strcmp(fundec -> child -> sibling -> sibling -> name, "RP")){
                    if(findFunc(fundec -> child -> value)){
                        printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec -> child -> lineno, fundec -> child ->value);
                    }
                    else{
                        //printf("add function {%s} of type [%d]\n", fundec -> child -> value, type -> kind);
                        addFunc2Table(fundec -> child -> value, type, 0, NULL);
                    }
                }
            }
        }

        // "IF" or "WHILE"
        if(!strcmp(r -> name, "IF") || !strcmp(r -> name, "WHILE")){
            int t1 = check(r -> sibling -> sibling);
            if(t1){
                if(returntype(r -> sibling -> sibling) -> kind != BASIC || returntype(r -> sibling -> sibling) -> u.basic != 0)
                    printf("Error type 7 at Line %d: Expression between \"(\" and \")\" is not an integer.\n", r -> sibling -> sibling -> lineno);
            }
            IfOrWhileOrReturn = 1;
            return ;
        }

        // "RETURN"
        if(!strcmp(r -> name, "RETURN")){
            int t1 = check(r -> sibling);
            if(t1){
                if(!typecmp(functypenow, returntype(r -> sibling)))
                    printf("Error type 8 at Line %d: Type mismatched for return.\n", r -> sibling -> lineno);
            }
            IfOrWhileOrReturn = 1;
            return ;
        }

        // "Exp"
        if(!strcmp(r -> name, "Exp")){
            if(IfOrWhileOrReturn){
                IfOrWhileOrReturn = 0;
            }
            else{
                if(check(r)){

                }
            }
            return;
        }

        while(temp){
            scanning(temp);
            temp=temp->sibling;
        }
    }
}