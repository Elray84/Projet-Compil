Chiffre [0-9]
Entier {Chiffre}+
Lettre [a-zA-Z]
LC ({Lettre}|{Chiffre}) /* Lettre ou chiffre */
COI [A-Z]{LC}* /* Classe ou objet isolé */
Id [a-z]{LC}*
Delim [ \n\t\r]
Com \*/.*\*/

%{
#include "tp.h"
#include "tp_y.h"
#include <string.h>

extern YYSTYPE yylval;
extern int yylineno;

%}

%%
{Entier} { yylval.I = atoi(yytext); return (CSTE); }

{Id} { yylval.S = strdup(yytext); return ID; }

{COI} { yylval.S = strdup(yytext); return COI; }

{Delim}+ { /* RIENG */ }

{Com} { /* RIENG */ }

"+" { return ADD; }

"-" { return SUB; }

"*" { return MUL; }

"/" { return DIV; }

"=" { return EQU; }

"<>" { return DIF; }

">" { return SUP; }

"<" { return INF; }

">=" { return SEQ; }

"<=" { return IEQ; }
