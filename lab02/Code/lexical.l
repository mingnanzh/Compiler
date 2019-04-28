%{
    #include "syntax.tab.h"
    #include "tree.h"
    #include "semantic.h"
    #include <stdio.h>
    #include <math.h>
    #include <string.h>

    extern int allright;
    int yycolumn = 1;
 
	#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;

    int htoi(char *s);
    int otoi(char *s);

%}

%option yylineno

/*regular definitions*/
delim           [ ]
digit           [0-9]
octdigit        [0-7]
hexdigit        [a-fA-F0-9]
nonzero         [1-9]
decinteger      0|{nonzero}{digit}*
octinteger      0{octdigit}+
hexinteger      0(x|X){hexdigit}+
letter          [a-zA-Z_]
integer         {decinteger}|{octinteger}|{hexinteger}
base            ({decinteger})?\.{digit}+|{decinteger}\.
exponent        (e|E)(\+|\-)?{digit}+   
float           {base}({exponent})?
id              {letter}({letter}|{digit})*


%%
{delim}                             {}
\t                                  {}
\n                                  {yycolumn=1;}
struct                              {yylval.node=create_newnode(0,"STRUCT",yylineno,yytext);        return STRUCT;}
return                              {yylval.node=create_newnode(0,"RETURN",yylineno,yytext);        return RETURN;}
if                                  {yylval.node=create_newnode(0,"IF",yylineno,yytext);            return IF;}
else                                {yylval.node=create_newnode(0,"ELSE",yylineno,yytext);          return ELSE;}
while                               {yylval.node=create_newnode(0,"WHILE",yylineno,yytext);         return WHILE;}
int|float                           {yylval.node=create_newnode(0,"TYPE",yylineno,yytext);          return TYPE;}
";"                                 {yylval.node=create_newnode(0,"SEMI",yylineno,yytext);          return SEMI;}
","                                 {yylval.node=create_newnode(0,"COMMA",yylineno,yytext);         return COMMA;}
"="                                 {yylval.node=create_newnode(0,"ASSIGNOP",yylineno,yytext);      return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|!=           {yylval.node=create_newnode(0,"RELOP",yylineno,yytext);         return RELOP;}
"+"                                 {yylval.node=create_newnode(0,"PLUS",yylineno,yytext);          return PLUS;}
"-"                                 {yylval.node=create_newnode(0,"MINUS",yylineno,yytext);         return MINUS;}
"*"                                 {yylval.node=create_newnode(0,"STAR",yylineno,yytext);          return STAR;}
"/"                                 {yylval.node=create_newnode(0,"DIV",yylineno,yytext);           return DIV;}
"&&"                                {yylval.node=create_newnode(0,"AND",yylineno,yytext);           return AND;}
"||"                                {yylval.node=create_newnode(0,"OR",yylineno,yytext);            return OR;}
"."                                 {yylval.node=create_newnode(0,"DOT",yylineno,yytext);           return DOT;}
"!"                                 {yylval.node=create_newnode(0,"NOT",yylineno,yytext);           return NOT;}
"("                                 {yylval.node=create_newnode(0,"LP",yylineno,yytext);            return LP;}
")"                                 {yylval.node=create_newnode(0,"RP",yylineno,yytext);            return RP;}
"["                                 {yylval.node=create_newnode(0,"LB",yylineno,yytext);            return LB;}
"]"                                 {yylval.node=create_newnode(0,"RB",yylineno,yytext);            return RB;}
"{"                                 {yylval.node=create_newnode(0,"LC",yylineno,yytext);            return LC;}
"}"                                 {yylval.node=create_newnode(0,"RC",yylineno,yytext);            return RC;}
{id}                                {yylval.node=create_newnode(0,"ID",yylineno,yytext);            return ID;}
{decinteger}                        {yylval.node=create_newnode(0,"INT",yylineno,yytext);           return INT;} 
{octinteger}                        {yylval.node=create_newnode(0,"INT",yylineno,"");               sprintf(yylval.node->value,"%d",otoi(yytext));          return INT;} 
{hexinteger}                        {yylval.node=create_newnode(0,"INT",yylineno,"");               sprintf(yylval.node->value,"%d",htoi(yytext));          return INT;} 
{float}                             {yylval.node=create_newnode(0,"FLOAT",yylineno,"");             sprintf(yylval.node->value,"%f",atof(yytext));          return FLOAT;}
.                                   {allright=0;  printf("Error type A at line %d: Mysterious character \'%s\'.\n",yylineno,yytext);}

%%
int htoi(char* s){
    int i = 0, t = 0, temp = 0, len=0;
    len = strlen(s);
    int res=0;
    for(int i=2;i<len;i++){
        if(s[i]<='9'&&s[i]>=0)
            t=s[i]-'0';
        else if(s[i]>='A'&&s[i]<='Z')
            t=s[i]-'A'+10;
        else if(s[i]>='a'&&s[i]<='z')
            t=s[i]-'a'+10;
        res=res*16+t;
    }
    return res;
}

int otoi(char* s){
    int i = 0, t = 0, temp = 0, len=0;
    len = strlen(s);
    int res=0;
    for(int i=1;i<len;i++){
        if(s[i]<='7'&&s[i]>=0)
            t=s[i]-'0';
        res=res*8+t;
    }
    return res;
}