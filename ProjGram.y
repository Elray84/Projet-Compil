%token class
%token object
%token <S> COI
%token is
%token var
%token extends
%token <S> ID
%token def
%token <I> CSTE

programme : L BlockInst TODO
;


L :
| DeclStruct L
;


DeclStruct : DeclClass
| DeclObject
;


DeclClass :
| class COI '(' LParamOpt ')' Herit is '{' DeclBody '}'
;

DeclObject : object COI is '{' DeclBody '}'
;

LParamOpt : LParam
|
;

LParam : Param ',' LParam
| Param
;


Param : var ID ':' COI
| ID ':' COI
;


Herit :
| extends COI
;


DeclBody : LDeclfields Construct LMethods
;

LDeclFields :
| Field LDeclFields
;

Field : ID ':' COI ';'
| var ID ':' COI ';'
;

Construct : def COI '(' LParamOpt ')' Super is '{' BodyConstruct '}'
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
| CSTE
;

BodyConstruct :
|
;

ListOptMethod :
| Over def ID '('LParamOpt')'  ':' COI ':=' Expression ';'
| Over def ID '('LParamOpt')' ReturnType is '{'Bloc'}'
;

Over :
| override
;

Expression :;

ReturnType :
| COI
;

Bloc :;

Decl: ID AFFECT Expr ';'

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
