%{
    #include "lex.yy.c"
    #include <stdio.h>
    #include <string.h>
    #include <malloc.h>
    #include "tree.h"

    void yyerror(char *msg);

    struct treenode* root;
    int allright=1;
%}

%expect 3

%union{
    struct treenode* node;
}

/* declared tokens */
%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args

%token <node> STRUCT RETURN IF ELSE WHILE TYPE
%token <node> SEMI COMMA
%token <node> ASSIGNOP RELOP PLUS MINUS STAR DIV
%token <node> AND OR DOT NOT
%token <node> LP RP LB RB LC RC
%token <node> ID INT FLOAT

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* high-level definitions */
Program : ExtDefList{
        $$=create_newnode(1,"Program",@$.first_line,"");
        root=$$;
        insert_child(root,$1);
    }
    ;
ExtDefList : /* empty */{$$=create_newnode(2,"ExtDefList",@$.first_line,"");}
    | ExtDef ExtDefList{
        $$=create_newnode(1,"ExtDefList",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | error ExtDefList{}
    ;
ExtDef : Specifier ExtDecList SEMI{
        $$=create_newnode(1,"ExtDef",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Specifier SEMI{
        $$=create_newnode(1,"ExtDef",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | Specifier FunDec CompSt{
        $$=create_newnode(1,"ExtDef",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    ;
ExtDecList : VarDec{
        $$=create_newnode(1,"ExtDecList",@$.first_line,"");
        insert_child($$,$1);
    }
    | VarDec COMMA ExtDecList{
        $$=create_newnode(1,"ExtDecList",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    ;

/* specifiers */
Specifier : TYPE{
        $$=create_newnode(1,"Specifier",@$.first_line,"");
        insert_child($$,$1);
    }
    | StructSpecifier{
        $$=create_newnode(1,"Specifier",@$.first_line,"");
        insert_child($$,$1);
    }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC{
        $$=create_newnode(1,"StructSpecifier",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
        insert_sibling($4,$5);
    }
    | STRUCT Tag{
        $$=create_newnode(1,"StructSpecifier",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    ;
OptTag : /* empty */    {$$=create_newnode(2,"OptTag",@$.first_line,"");}
    | ID{
        $$=create_newnode(1,"OptTag",@$.first_line,"");
        insert_child($$,$1);
    }
    ;
Tag : ID{
        $$=create_newnode(1,"Tag",@$.first_line,"");
        insert_child($$,$1);
    }
    ;

/* declarators */
VarDec : ID{
        $$=create_newnode(1,"VarDec",@$.first_line,"");
        insert_child($$,$1);
    }
    | VarDec LB INT RB{
        $$=create_newnode(1,"VarDec",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
    }
    ;
FunDec : ID LP VarList RP{
        $$=create_newnode(1,"FunDec",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
    }
    | ID LP RP{
        $$=create_newnode(1,"FunDec",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    ;
VarList : ParamDec COMMA VarList{
        $$=create_newnode(1,"VarList",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | ParamDec{
        $$=create_newnode(1,"VarList",@$.first_line,"");
        insert_child($$,$1);
    }
    ;
ParamDec : Specifier VarDec{
        $$=create_newnode(1,"ParamDec",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    ;

/* statements */
CompSt : LC DefList StmtList RC{
        $$=create_newnode(1,"CompSt",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
    }
    ;
StmtList : /* empty */  {$$=create_newnode(2,"StmtList",@$.first_line,"");}
    | Stmt StmtList{
        $$=create_newnode(1,"StmtList",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | error StmtList{}
    ;
Stmt : Exp SEMI{
        $$=create_newnode(1,"Stmt",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | CompSt{
        $$=create_newnode(1,"Stmt",@$.first_line,"");
        insert_child($$,$1);
    }
    | RETURN Exp SEMI{
        $$=create_newnode(1,"Stmt",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
        $$=create_newnode(1,"Stmt",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
        insert_sibling($4,$5);
    }
    | IF LP Exp RP Stmt ELSE Stmt{
        $$=create_newnode(1,"Stmt",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
        insert_sibling($4,$5);
        insert_sibling($5,$6);
        insert_sibling($6,$7);
    }
    | WHILE LP Exp RP Stmt{
        $$=create_newnode(1,"Stmt",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
        insert_sibling($4,$5);
    }
    ;

/* local definitions */
DefList : /* empty */   {$$=create_newnode(2,"DefList",@$.first_line,"");}
    | Def DefList{
        $$=create_newnode(1,"DefList",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | error DefList{}
    ;
Def : Specifier DecList SEMI{
        $$=create_newnode(1,"Def",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    ;
DecList : Dec{
        $$=create_newnode(1,"DecList",@$.first_line,"");
        insert_child($$,$1);
    }
    | Dec COMMA DecList{
        $$=create_newnode(1,"DecList",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    ;
Dec : VarDec{
        $$=create_newnode(1,"Dec",@$.first_line,"");
        insert_child($$,$1);
    }
    | VarDec ASSIGNOP Exp{
        $$=create_newnode(1,"Dec",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    ;

/* expressions */
Exp : Exp ASSIGNOP Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp AND Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp OR Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp RELOP Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp PLUS Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp MINUS Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp STAR Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp DIV Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | LP Exp RP{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | MINUS Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | NOT Exp{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
    }
    | ID LP Args RP{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
    }
    | ID LP RP{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp LB Exp RB{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
        insert_sibling($3,$4);
    }
    | Exp LB error RB
    | Exp DOT ID{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | ID{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
    }
    | INT{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
    }
    | FLOAT{
        $$=create_newnode(1,"Exp",@$.first_line,"");
        insert_child($$,$1);
    }
    ;
Args : Exp COMMA Args{
        $$=create_newnode(1,"Args",@$.first_line,"");
        insert_child($$,$1);
        insert_sibling($1,$2);
        insert_sibling($2,$3);
    }
    | Exp{
        $$=create_newnode(1,"Args",@$.first_line,"");
        insert_child($$,$1);
    }
    ;

%%
void yyerror(char *msg)
{
    allright=0;
	fflush(stdout);
	printf("Error type B at line %d: %s.\n",yylineno,msg);
}
