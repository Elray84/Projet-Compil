%token CLA
%token OBJ
%token <S> COI
%token IS
%token VAR
%token EXT
%token <S> ID
%token DEF
%token <I> CST
%token AFF
%token OVR
%token IF THE ELS
%token ADD SUB MUL DIV
%token EQU DIF INF IEQ SUP SEQ
%token NEW
%token RTN
%token THI SPR
%token RES

%{#include "proj.h"     /* les definition des types et les etiquettes des noeuds */

extern int yylex();	/* fournie par Flex */
extern void yyerror();  /* definie dans tp.c */
%}

%left '.'

%%
programme : L Bloc;


L :
| DeclStruct L
;


DeclStruct : DeclClass
| DeclObject
;


DeclClass : CLA COI '(' LParamOpt ')' Herit IS '{' DeclBody '}'
;

DeclObject : OBJ COI IS '{' DeclBody '}'
;

LParamOpt : LParam
|
;

LParam : Param ',' LParam
| Param
;


Param : VAR ID ':' COI
| ID ':' COI
;


Herit :
| EXT COI
;


DeclBody : LDeclFields Construct ListOptMethods
;

LDeclFields :
| Field LDeclFields
;

Field : ID ':' COI ';'
| VAR ID ':' COI ';'
;

Construct : DEF COI '(' LParamOpt ')' Super IS '{' LInstructions '}'
;

Super :
| ':' COI '(' LArgOpt ')'
;

LArgOpt :
| LArg
;

LArg : Arg ',' LArg
| Arg
;

Arg : ID
| CST
;


ListOptMethods :
| Over DEF ID '('LParamOpt')'  ':' COI AFF Expression ';'
| Over DEF ID '('LParamOpt')' ReturnType IS Bloc
;

Over :
| OVR
;

LInstructions :
| Instruction LInstructions;

Instruction : Expression ';'
| Bloc
| RTN ';'
| IF Expression THE Instruction ELS Instruction
| ID AFF Expression
;

Expression : '(' Expression ')'
| '(' COI Expression')'
| Envoi
 ;

 EnvoiOuSelect : Envoi
 | Selection
 |ExpressionBase;

 Envoi : EnvoiOuSelect '.' ID '(' LIDOpt ')'
 /*| Selection '.' ID '(' LIDOpt ')'*/
 /*| Selection*/
 ;

 Selection : EnvoiOuSelect '.' ID
 /*| Envoi '.' ID
 | ExpressionBase*/
 ;

 ExpressionBase : ID
 | CST
 | Instantiation
 ;

 LIDOpt :
 | LID
 ;

LID : ID ',' LID
| ID
;

Instantiation : NEW COI '(' LIDOpt ')'
;



ReturnType :
| COI
;

Bloc : '{' LInstructions '}'
| '{' Decl LDecl IS Instruction LInstructions '}'
;

LDecl :
| Decl LDecl
;

Decl: ID ':' COI ';'
| ID ':' COI AFF Expression ';'
;


/* expr : If bexpr Then expr Else expr

| expr ADD expr
| expr SUB expr
| expr MUL expr
| expr DIV expr
| ADD expr %prec unary */
/* Pour l'AST on traite le - unaire comme un - binaire , comme cela on ne
 * s'en soucie plus dans la suite
 */
 /*
| SUB expr %prec unary
| CST
| ID
| '(' expr ')'
;

/* Expression booleenne seulement presente dans un IF */
/* bexpr : expr RELOP expr
| '(' bexpr ')'
; */
/* bexpr : expr RELOP expr
| '(' bexpr ')'
; */
