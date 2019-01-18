#include <stdlib.h>

/* deux macros pratiques, utilisees dans les allocations de structure
 * Pour NEW on donne le nombre et le type de la stucture a allouer (pas le type
 * du pointeur) et on recupere un pointeur sur cette structure.
 * Pour NIL: on donne de meme le type de la structure (pas le type du pointeur)
 */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))
#define NIL(type) (type *) 0

#define TRUE 1
#define FALSE 0
typedef int bool;


/* Codes d'erreurs */
#define NO_ERROR	100
#define USAGE_ERROR	101
#define LEXICAL_ERROR	102
#define SYNTAX_ERROR    103
#define CONTEXT_ERROR	104
#define EVAL_ERROR	105
#define UNEXPECTED	110


/* Etiquettes pour les arbres de syntaxe abstraite */
#define hADD	1
#define hSUB	2
#define hMUL	3
#define hDIV	4
#define hEQU	5
#define hDIF	6
#define hSUP	7
#define hINF	8
#define hSEQ	9
#define hIEQ	10
#define hAFF	11
#define hCLA	12
#define hEXT	13
#define hIS	14
#define hVAR	15
#define hDEF   16
#define hOBJ   17
#define hOVR   18
#define hNEW   19
#define hRTN   20
#define hIF    21
#define hTHE   22
#define hELS   23
#define hTHI   24
#define hSPR   25
#define hRES   26
#define hCST   27
#define hID    28
#define hCOI   29


		/* Definition d'un arbre de syntaxe abstraite */

/* la structure d'un arbre (noeud ou feuille) */
typedef struct _Tree {
  short op;         /* etiquette de l'operateur courant */
  short nbChildren; /* nombre de sous-arbres */
  union {
    char *str;      /* valeur de la feuille si op = ID */
    int val;        /* valeur de la feuille si op = CST */
    struct _Tree **children; /* tableau des sous-arbres d'un noeud interne */
  } u;
} Tree, *TreeP;


/* la structure ci-dessous permet de cosntruire des listes de paires
 * (variable, valeur entiere).
 * On va construire des listes de la forme { (x 5), (y, 27) } au fur
 * et a mesure qu'on interprete les declarations dans un programme source.
 */
typedef struct _Decl
{ char *name;
  int val;
  struct _Decl *next;
} VarDecl, *VarDeclP;

/* La structure de la classe mamène */

typedef struct _Meth
{ char *name;
  
  VarDeclP decls;
  TreeP ListInstr;

  struct _Meth *next;
} Meth, *MethP;

typedef struct _Class
{ char *name;
  struct _Class *superClass;
  struct _Class *next;
  MethP meths;
} Class, *ClassP;

/* Type pour la valeur de retour de Flex et les actions de Bison
 * le premier champ est necessaire pour Flex.
 * les autres correspondent aux variantes utilisees dans les actions
 * associees aux productions de la grammaire.
*/
typedef union
{ char C;	/* caractere isole */
  char *S;	/* chaine de caractere */
  int I;	/* valeur entiere */
  VarDeclP D;	/* liste de paires (variable, valeur) */
  TreeP T;	/* AST */
} YYSTYPE;


/* necessaire sinon par defaut Bison definit YYSTYPE comme int ! */
#define YYSTYPE YYSTYPE


	/* Prototypes des fonctions (partiellement) mises a disposition */

/* construction pour les AST */
TreeP makeLeafStr(short op, char *str); 	    /* feuille (string) */
TreeP makeLeafInt(short op, int val);	            /* feuille (int) */
TreeP makeTree(short op, int nbChildren, ...);	    /* noeud interne */

/* Impression des AST */
void printAST(TreeP decls, TreeP main);

/* gestion des declarations et traitement de la portee */
VarDeclP addToScope(VarDeclP list, VarDeclP nouv);
VarDeclP declVar(char *name, TreeP tree, VarDeclP decls);

/* evaluateur pour les parties declarations */
VarDeclP evalDecls (TreeP tree);
int eval(TreeP tree, VarDeclP decls);

/* evaluateur pour l'expression principale. Elle prend aussi en parametre
 * la liste des couples (variable, valeur) deduite des declarations
 */
int evalMain(TreeP tree, VarDeclP decls);
