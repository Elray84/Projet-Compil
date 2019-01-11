Chiffre [0-9]
Entier {Chiffre}+
Lettre [a-zA-Z]
LC ({Lettre}|{Chiffre}) /* Lettre ou chiffre */
COI [A-Z]{LC}* /* Classe ou objet isolé */
Id [a-z]{LC}*
Delim [ \n\t\r]
Star "*"
NoStar ^{Star}
Slash "/"
NoStarOrSlash {NoStar}\{Slash}
StarPlus {Star}+
Com {Slash}{Star}({NoStar}*{StarPlus}{NoStarOrSlash})*{NoStar}*{StarPlus}{Slash}

%{
#include "tp.h"
#include "tp_y.h"
#include <string.h>

extern YYSTYPE yylval;
extern int yylineno;

int keywordOrId(char *text){
  if(strcmp(text, "class")) return class;
  if(strcmp(text, "extends")) return extends;
  if(strcmp(text, "is")) return is;
  if(strcmp(text, "var")) return var;
  if(strcmp(text, "def")) return def;
  if(strcmp(text, "object")) return object;
  if(strcmp(text, "override")) return override;
  if(strcmp(text, "new")) return new;
  if(strcmp(text, "return")) return return;
  if(strcmp(text, "if")) return if;
  if(strcmp(text, "then")) return then;
  if(strcmp(text, "else")) return else;
  if(strcmp(text, "this")) return this;
  if(strcmp(text, "super")) return super;
  if(strcmp(text, "result")) return result;
  return ID;

}

%}

%%
{Entier} { yylval.I = atoi(yytext); return CSTE; }

{Id} { yylval.S = strdup(yytext); return keywordOrId(yytext); }

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

":=" { return AFF;}
