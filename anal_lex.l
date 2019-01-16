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
  if(strcmp(text, "class")) return CLA;
  if(strcmp(text, "extends")) return EXT;
  if(strcmp(text, "is")) return IS;
  if(strcmp(text, "var")) return VAR;
  if(strcmp(text, "def")) return DEF;
  if(strcmp(text, "object")) return OBJ;
  if(strcmp(text, "override")) return OVR;
  if(strcmp(text, "new")) return NEW;
  if(strcmp(text, "return")) return RTN;
  if(strcmp(text, "if")) return IF;
  if(strcmp(text, "then")) return THE;
  if(strcmp(text, "else")) return ELS;
  if(strcmp(text, "this")) return THI;
  if(strcmp(text, "super")) return SPR;
  if(strcmp(text, "result")) return RES;
  return ID;

}

%}

%%
{Entier} { yylval.I = atoi(yytext); return CST; }

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
