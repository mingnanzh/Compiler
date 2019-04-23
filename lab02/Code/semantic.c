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

struct StruTable{
    char* name;
    Type type;
    struct StruTable* next;
};

struct VarTable* head;
struct VarTable* end;

struct StruTable* StruTable_head;
struct StruTable* StruTable_end;

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

Type shit2(struct treenode* node, Type type){
    if(!strcmp(node -> name, "ID")){
        addVar2Table(node -> value, type);
        printf("add variabel '%s' of type [%d]\n", node -> value, type -> kind);
        return type;
        //printf("size: %d*%d\n", type -> u.array.size, type -> u.array.elem ->u.array.size);
    }
    else if(!strcmp(node -> name, "VarDec")){
        Type type_temp = (Type)malloc(sizeof(struct Type_));
        type_temp -> kind = ARRAY;
        type_temp -> u.array.size = atoi(node -> sibling -> sibling -> value);
        type_temp -> u.array.elem = type;
        return shit2(node -> child, type_temp); 
    }
}

char* getid(struct treenode* node){
    if(!strcmp(node -> name, "ID")){
        return node -> value;
        //printf("size: %d*%d\n", type -> u.array.size, type -> u.array.elem ->u.array.size);
    }
    else if(!strcmp(node -> name, "VarDec")){
        return getid(node -> child); 
    }
}

Type findtype(char* name){
    struct StruTable* temp = StruTable_head;
    while(temp){
        if(!strcmp(temp -> name, name))
            return temp -> type;
        else
            temp = temp -> next;
    }
    return NULL;
}

FieldList getstructure(struct treenode* node){
    if(node -> type == 2){
        //return NULL;
    }
    else{
        if(!strcmp(node -> child -> name, "Def")){
            struct treenode* def = node -> child;
            Type type = (Type)malloc(sizeof(struct Type_));

            // int or float
            if(!strcmp(def -> child -> child -> name, "TYPE")){
                if(!strcmp(def -> child -> child -> value, "int")){
                    type -> kind = BASIC;
                    type -> u.basic = 0;
                }
                else if(!strcmp(def -> child -> child -> value, "float")){
                    type -> kind = BASIC;
                    type -> u.basic = 1;
                }
                struct treenode* declist = def -> child -> sibling;    
                FieldList ks = NULL;
                FieldList js = NULL;
                while(declist){
                    FieldList structureField = (FieldList)malloc(sizeof(struct FieldList_));
                    structureField -> name = (char*)malloc(strlen(getid(declist -> child -> child))+1);
                    strcpy(structureField -> name, getid(declist -> child -> child));
                    structureField -> type = shit2(declist -> child -> child -> child, type);
                    if(!ks){
                        ks = structureField;
                        js = ks;
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
                js -> tail = getstructure(def -> sibling);
                return ks;
            }
            
            // STRUCT
            if(!strcmp(def -> child -> child -> name, "StructSpecifier")){
                // STRUCT OptTag LC DefList RC
                if(!strcmp(def -> child -> child -> child -> name, "STRUCT") && !strcmp(def-> child -> child -> child -> sibling -> name, "OptTag")){
                    Type type = (Type)malloc(sizeof(struct Type_));
                    type -> kind = STRUCTURE;

                    // OptTag != empty
                    if(def -> child -> child -> child -> sibling -> type != 2){
                        //printf("%s %s\n", r -> child -> child -> child -> value, r -> child -> child -> child -> sibling -> child -> value);
                        struct treenode* deflist = def -> child -> child -> child -> sibling -> sibling -> sibling;
                        type -> u.structure = getstructure(deflist);
                        printf("add structure ''%s''.\n", def -> child -> child -> child -> sibling -> child -> value);
                        addStru2Table(def -> child -> child -> child -> sibling -> child -> value, type);
                    }
                    // OptTag ==empty
                    else{
                        struct treenode* deflist = def -> child -> child -> child -> sibling -> sibling -> sibling;
                        type -> u.structure = getstructure(deflist);
                    }
                    struct treenode* declist = def -> child -> sibling;    
                    FieldList ks = NULL;
                    FieldList js = NULL;
                    while(declist){
                        FieldList structureField = (FieldList)malloc(sizeof(struct FieldList_));
                        structureField -> name = (char*)malloc(strlen(getid(declist -> child -> child))+1);
                        strcpy(structureField -> name, getid(declist -> child -> child));
                        structureField -> type = shit2(declist -> child -> child -> child, type);
                        if(!ks){
                            ks = structureField;
                            js = ks;
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
                    js -> tail = getstructure(def -> sibling);
                    return ks;
                }

                // STRUCT Tag
                if(!strcmp(def -> child -> child -> child -> name, "STRUCT") && !strcmp(def-> child -> child -> child -> sibling -> name, "Tag")){
                    type = findtype(def-> child -> child -> child -> sibling -> child -> value);
                    struct treenode* declist = def -> child -> sibling;    
                    FieldList ks = NULL;
                    FieldList js = NULL;
                    while(declist){
                        FieldList structureField = (FieldList)malloc(sizeof(struct FieldList_));
                        structureField -> name = (char*)malloc(strlen(getid(declist -> child -> child))+1);
                        strcpy(structureField -> name, getid(declist -> child -> child));
                        structureField -> type = shit2(declist -> child -> child -> child, type);
                        if(!ks){
                            ks = structureField;
                            js = ks;
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
                    js -> tail = getstructure(def -> sibling);
                    return ks;
                }
            }
        }
    }   
}

void scanning(struct treenode* r){
    if(r && r -> type <= 1){
        struct treenode* temp = r -> child;

        // "Def"
        if(!strcmp(r -> name,"Def")){
            Type type = (Type)malloc(sizeof(struct Type_));
            
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
            }
            
            // STRUCT
            else if(!strcmp(r -> child -> child -> name, "StructSpecifier")){
                //STRUCT OptTag LC DefList RC
                if(!strcmp(r -> child -> child -> child -> name, "STRUCT") && !strcmp(r -> child -> child -> child -> sibling -> name, "OptTag")){
                    type -> kind = STRUCTURE;
                    // OptTag != empty
                    if(r -> child -> child -> child -> sibling -> type != 2){
                    //printf("%s %s\n", r -> child -> child -> child -> value, r -> child -> child -> child -> sibling -> child -> value);
                    struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                        type -> u.structure = getstructure(deflist);
                        printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                        addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                    }
                    // OptTag == empty
                    else{
                        struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                        type -> u.structure = getstructure(deflist);
                        //printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                        //addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                    }
                }
                // TODO: STRUCT Tag
                else if(!strcmp(r -> child -> child -> child -> name, "STRUCT") && !strcmp(r -> child -> child -> child -> sibling -> name, "Tag")){
                    type = findtype(r -> child -> child -> child -> sibling -> child -> value);
                }
            }

            struct treenode* declist = r -> child -> sibling; 
            while(declist){
                shit(declist -> child -> child -> child, type);
                if(!declist -> child -> sibling)
                    declist = NULL;
                else
                    declist = declist -> child -> sibling -> sibling;
            }

            return;
        }

        // "ExtDef"
        if(!strcmp(r -> name,"ExtDef")){
            //global variable
            if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "ExtDecList") && !strcmp(r -> child -> sibling -> sibling -> name, "SEMI")){
                Type type = (Type)malloc(sizeof(struct Type_));

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
                }
                // STRUCT
                else if(!strcmp(r -> child -> child -> name, "StructSpecifier")){
                    //STRUCT OptTag LC DefList RC
                    if(!strcmp(r -> child -> child -> child -> name, "STRUCT") && !strcmp(r -> child -> child -> child -> sibling -> name, "OptTag")){
                        type -> kind = STRUCTURE;
                        // OptTag != empty
                        if(r -> child -> child -> child -> sibling -> type != 2){
                            //printf("%s %s\n", r -> child -> child -> child -> value, r -> child -> child -> child -> sibling -> child -> value);
                            struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                            type -> u.structure = getstructure(deflist);
                            printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                            addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                        }
                        else{
                            struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                            type -> u.structure = getstructure(deflist);
                            //printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                            //addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                        }
                    }
                    // TODO: STRUCT Tag
                    else if(!strcmp(r -> child -> child -> child -> name, "STRUCT") && !strcmp(r -> child -> child -> child -> sibling -> name, "Tag")){
                        type = findtype(r -> child -> child -> child -> sibling -> child -> value);
                    }
                }
                struct treenode* extdeclist = r -> child -> sibling;    
                while(extdeclist){
                    shit(extdeclist -> child -> child, type);
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
                            type -> u.structure = getstructure(deflist);
                            printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                            addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                        }
                        else{
                            struct treenode* deflist = r -> child -> child -> child -> sibling -> sibling -> sibling;
                            type -> u.structure = getstructure(deflist);
                            //printf("add structure ''%s''.\n", r -> child -> child -> child -> sibling -> child -> value);
                            //addStru2Table(r -> child -> child -> child -> sibling -> child -> value, type);
                        }
                    }
                    // TODO: STRUCT Tag
                }
                return;
            }
            
            // TODO: FUNCTION
            else if(!strcmp(r -> child -> name, "Specifier") && !strcmp(r -> child -> sibling -> name, "FunDec") && !strcmp(r -> child -> sibling -> sibling -> name, "CompSt")){

            }
        }

        while(temp){
            scanning(temp);
            temp=temp->sibling;
        }
    }
}