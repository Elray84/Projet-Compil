/*
 * Un petit programme de demonstration qui n'utilise que l'analyse lexicale.
 * Permet principalement de tester la correction de l'analyseur lexical et de
 * l'interface entre celui-ci et le programme qui l'appelle.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "proj.h"
#include "gram_y.h"


/* Fonction appelee par le programme principal pour obtenir l'unite lexicale
 * suivante. Elle est produite par Flex (fichier tp_l.c)
 */
extern int yylex (void);

/* Le texte associe au token courant: defini et mis a jour dans tp_l.c */
extern char *yytext;

/* Le numero de ligne courant : defini et mis a jour dans tp_l.c */
extern int yylineno;

/* Variable pour interfacer flex avec le programme qui l'appelle, notamment
 * pour transmettre de l'information, en plus du type d'unite reconnue.
 * Le type YYSTYPE est defini dans tp.h.
 */
YYSTYPE yylval;

bool verbose = FALSE;

void setError(int code) {
  /* present ici uniquement pour des raisons de compatibilite */
}

/* format d'appel */
void help() {
  fprintf(stderr, "Appel: tp [-h] [-v] programme.txt\n");
}

/* Appel:
 *   test_lex [-option]* programme.txt
 * Les options doivent apparaitre avant le nom du fichier du programme.
 * Options: -[vV] -[hH?]
 */
int main(int argc, char **argv) {
  int fi;
  int token;
  int i;

  for(i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'v': case 'V':
	verbose = TRUE; continue;
      case '?': case 'h': case 'H':
	help();
	exit(1);
      default:
	fprintf(stderr, "Option inconnue: %c\n", argv[i][1]);
	exit(1);
      }
    } else break;
  }

  if (i == argc) {
    fprintf(stderr, "Erreur: Fichier programme manquant\n");
    help();
    exit(1);
  }

  if ((fi = open(argv[i++], O_RDONLY)) == -1) {
    fprintf(stderr, "Erreur: fichier inaccessible %s\n", argv[i-1]);
    help();
    exit(1);
  }

  /* redirige l'entree standard sur le fichier... */
  close(0); dup(fi); close(fi);

  while (1) {
    token = yylex();
    switch (token) {
    case 0: /* EOF */
      printf("Fin de l'analyse lexicale\n");
      return 0;
    case ID:
      if (verbose) printf("Identificateur:\t\t%s\n", yylval.S);
      else printf ("Identificateur\n");
      break;
    case CST:
      /* ici on suppose qu'on a recupere la valeur de la constante, pas sa
       * representation sous forme de chaine de caracteres.
       */
      if (verbose) printf("Constante:\t\t%d\n", yylval.I);
      else printf ("Constante\n");
      break;
    case IF:
    case THE:
    case ELS:
      if (verbose) printf("Mot-clef:\t\t%s\n",  yytext);
      break;
    case '(': 
    case ')': 
    case ';':
      printf("Symbole:\t\t%s\n",  yytext);
      break;
    case ADD:
      printf("Operateur arithmetique '+'\n"); break;
    case SUB:
      printf("Operateur arithmetique '-'\n"); break;
    case MUL:
      printf("Operateur arithmetique '*'\n"); break;
    case DIV:
      printf("Operateur arithmetique '/'\n"); break;
    case EQU: printf("Operateur comparaison '='\n"); break;
	case DIF: printf("Operateur comparaison '<>'\n"); break;
	case INF: printf("Operateur comparaison '<'\n"); break;
	case IEQ: printf("Operateur comparaison '<='\n"); break;
	case SUP: printf("Operateur comparaison '>'\n"); break;
	case SEQ: printf("Operateur comparaison '>='\n"); break;
    case AFF: printf("Symbole d'affectation\t:=\n"); break;
    case CLA: printf("Symbole class\n"); break;
    case EXT: printf("Symbole extends\n"); break;
    case IS : printf("Symbole is\n"); break;
    case VAR: printf("Symbole var\n"); break;
    case DEF: printf("Symbole def\n"); break;
    case OBJ: printf("Symbole object\n"); break;
    case OVR: printf("Symbole override\n"); break;
    case NEW: printf("Symbole new\n"); break;
    case RTN: printf("Symbole return\n"); break;
    case THI: printf("Symbole this\n"); break;
    case SPR: printf("Symbole super\n"); break;
    case RES: printf("Symbole result\n"); break;
    case COI: printf("Symbole coi\n"); break;
    default:
      printf("Token non reconnu:\t\"%d\"\n", token);
    }
  }
}
