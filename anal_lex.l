Chiffre [0-9]
Entier {Chiffre}+
Lettre [a-zA-Z]
LC ({Lettre}|{Chiffre})
COI [A-Z]{LC}*
Id [a-z]{LC}*
Delim [ \n\t\r]
Star "*"
NoStar [^\*]
Slash "/"
NoStarOrSlash [^\*\/]
StarPlus {Star}+
Com {Slash}{Star}({NoStar}*{StarPlus}{NoStarOrSlash})*{NoStar}*{StarPlus}{Slash}
Sym [(),;:{}.]
NoGuillemet [^\"]
String \"{NoGuillemet}*\"

%{
#include "proj.h"
#include "gram_y.h"
#include <string.h>

extern YYSTYPE yylval;
extern int yylineno;
extern char *strdup(const char*);

int keywordOrId(char *text){
  if(!strcmp(text, "class")) return CLA;
  if(!strcmp(text, "extends")) return EXT;
  if(!strcmp(text, "is")) return IS;
  if(!strcmp(text, "var")) return VAR;
  if(!strcmp(text, "def")) return DEF;
  if(!strcmp(text, "object")) return OBJ;
  if(!strcmp(text, "override")) return OVR;
  if(!strcmp(text, "new")) return NEW;
  if(!strcmp(text, "return")) return RTN;
  if(!strcmp(text, "if")) return IF;
  if(!strcmp(text, "then")) return THE;
  if(!strcmp(text, "else")) return ELS;
  if(!strcmp(text, "this")) return THI;
  if(!strcmp(text, "super")) return SPR;
  if(!strcmp(text, "result")) return RES;
  return ID;

}

void lexError(char c) {
	fprintf(stderr, "Token error: %c\n At line : %i\n", c, yylineno);
	setError(LEXICAL_ERROR);
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

"=" { yylval.R = EQU; return RELOP; }

"<>" { yylval.R = DIF; return RELOP; }

">" { yylval.R = SUP; return RELOP; }

"<" { yylval.R = INF; return RELOP; }

">=" { yylval.R = SEQ; return RELOP; }

"<=" { yylval.R = IEQ; return RELOP; }

":=" { return AFF;}

"&" { return CONCAT;}

{Sym} { return yytext[0];}

{String} { yylval.S = strdup(yytext); return CST;}

.  { lexError(yytext[0]); }
