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
%token <R> RELOP
%token NEW
%token RTN
%token THI SPR
%token RES
%token CONCAT

%{#include "proj.h"     /* les definition des types et les etiquettes des noeuds */

extern int yylex();	/* fournie par Flex */
extern void yyerror();  /* definie dans tp.c */
%}

%nonassoc RELOP
%left ADD SUB
%left MUL DIV
%left CONCAT
%nonassoc unary
%left '.'

%%
programme : L Bloc;


L :
| DeclStruct L
;


DeclStruct : DeclClass
| DeclObject
;


DeclClass : CLA COI '(' LParamOpt ')' Herit IS '{' DeclClassBody '}'
;

DeclObject : OBJ COI IS '{' DeclObjectBody '}'
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


DeclClassBody : LDeclFields ClassConstruct ListOptMethods
;

DeclObjectBody : LDeclFields ObjectConstruct ListOptMethods
;

LDeclFields :
| Field LDeclFields
;

Field : ID ':' COI ';'
| VAR ID ':' COI ';'
;

ClassConstruct : DEF COI '(' LParamOpt ')' Super IS '{' LInstructions '}'
;

ObjectConstruct : DEF COI Super IS '{' LInstructions '}'
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

Arg : Expression
;

ListOptMethods : 
| Method ListOptMethods
;

Method : Over DEF ID '('LParamOpt')'  ':' COI AFF Expression
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
| Expression AFF Expression ';'
;

Expression : Expression RELOP Expression
| Expression ADD Expression
| Expression SUB Expression
| Expression MUL Expression
| Expression DIV Expression
| ADD Expression %prec unary
| SUB Expression %prec unary
| Expression CONCAT Expression
| EnvoiOuSelect
/*| ExpressionNonAffectable*/
 ;

 EnvoiOuSelect : Envoi
 | Selection
 | ExpressionBase
 ;

 Envoi : EnvoiOuSelect '.' ID '(' LEOpt ')'
 | COI '.' ID '(' LEOpt ')'
 /*| Selection '.' ID '(' LIDOpt ')'*/
 /*| Selection*/
 ;

 Selection : EnvoiOuSelect '.' ID
 /*| Envoi '.' ID
 | ExpressionBase*/
 ;

 ExpressionBase : ID
 | '(' Expression ')'
 | '(' COI ExpressionBase')'
 | THI
 | SPR
 | RES
 | CST
 | Instantiation
 ;

Instantiation : NEW COI '(' LEOpt ')'
;



ReturnType :
| ':' COI
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

LEOpt :
| LE
;

LE : Expression
| Expression ',' LE
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
