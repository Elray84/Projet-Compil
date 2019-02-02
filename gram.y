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

%type <T> L Bloc DeclStruct LParamOpt LParam DeclClassBody DeclObjectBody LDeclFields Herit Field Super LArgOpt LArg Arg ListOptMethods Over LInstructions Instruction Expression EnvoiOuSelect Envoi Selection ExpressionBase ReturnType LDecl Decl LEOpt LE
%type <V> DeclClass DeclObject Param Instantiation
%type <M> ClassConstruct ObjectConstruct Method


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
programme : L Bloc /*{ printAST($1, $2); }*/
;

L : /*{ $$ = NIL(Tree); }*/
| DeclStruct L /*{ $$ = makeTree(hLST, 2 $1, $2); }*/
;


DeclStruct : DeclClass /*{ $$ = $1); }*/
| DeclObject /*{ $$ = $1); }*/
;


DeclClass : CLA COI '(' LParamOpt ')' Herit IS '{' DeclClassBody '}'
;

DeclObject : OBJ COI IS '{' DeclObjectBody '}'
;

LParamOpt : LParam
| /*{ $$ = NIL(Tree);}*/
;

LParam : Param ',' LParam /*{ $$ = makeTree(hLST, 2, $1, $3);}*/
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

LDeclFields : /*{ $$ = NIL(Tree); }*/
| Field LDeclFields /*{ $$ = makeTree(hLST, 2, $1, $2);}*/
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

LArg : Arg ',' LArg /*{ $$ = makeTree(hLST, 2, $1, $3); }*/
| Arg /*{ $$ = $1 }*/
;

Arg : Expression
;

ListOptMethods : /*{ $$ = NIL(Tree);}*/
| Method ListOptMethods /*{ $$ = makeTree(hLST, 2, $1, $2);}*/
;

Method : Over DEF ID '('LParamOpt')'  ':' COI AFF Expression
| Over DEF ID '('LParamOpt')' ReturnType IS Bloc
;

Over :
| OVR
;

LInstructions : /*{ $$ = NIL(Tree);}*/
| Instruction LInstructions; /*{ $$ = makeTree(hLST, 2, $1, $2);}*/

Instruction : Expression ';'
| Bloc
| RTN ';'
| IF Expression THE Instruction ELS Instruction
| Expression AFF Expression ';'
;

Expression : Expression RELOP Expression
| Expression ADD Expression /*{ $$ = makeTree(hADD, 2, $1, $3);}*/
| Expression SUB Expression /*{ $$ = makeTree(hSUB, 2, $1, $3);}*/
| Expression MUL Expression /*{ $$ = makeTree(hMUL, 2, $1, $3);}*/
| Expression DIV Expression /*{ $$ = makeTree(hDIV, 2, $1, $3);}*/
| ADD Expression %prec unary /*{ $$ = $2; }*/
| SUB Expression %prec unary /*{ $$ = makeTree(hSUB, 2, makeLeafInt(CONST, 0), $2); }*/
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

 ExpressionBase : ID /*{ $$ = makeLeafStr(hID, $1); }*/
 | '(' Expression ')' /*{ $$ = $2; }*/
 | '(' COI ExpressionBase')'
 | THI
 | SPR
 | RES
 | CST /*{ $$ = makeLeafInt(hCST, $1); }*/
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

LDecl :/*{ $$ = NIL(Tree);}*/
| Decl LDecl /*{ $$ = makeTree(hLST, 2, $1, $2); }*/
;

Decl: ID ':' COI ';'
| ID ':' COI AFF Expression ';'
;

LEOpt :
| LE
;

LE : Expression
| Expression ',' LE /*{ $$ = makeTree(hLST, 2, $1, $3); }*/
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
