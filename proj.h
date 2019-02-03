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

#define hDCL 30 /* TODO a remove c'est juste pour indiquer une declaration pour tester */
#define hLST 31 /* l'etiquette des arbres liste */

		/* Definition d'un arbre de syntaxe abstraite */

/* la structure d'un arbre (noeud ou feuille) */
typedef struct _Tree {
  short op;         /* etiquette de l'operateur courant */
  short nbChildren; /* nombre de sous-arbres */
  union {
    char *str;       /* ID (pas decl) OU string */
    int val;         /* int cst */
    struct _Meth *met;       /* Methode decl */
    struct _Var *var;        /* Variable decl */
    struct _Tree **children; /* tableau des sous-arbres d'un noeud interne */
  } u;
} Tree, *TreeP;


/* la structure ci-dessous permet de cosntruire des listes de paires
 * (variable, valeur entiere).
 * On va construire des listes de la forme { (x 5), (y, 27) } au fur
 * et a mesure qu'on interprete les declarations dans un programme source.
 */

enum Type {
  integ,
  string
};

enum VarType{
  objet,
  objetIsole,
  varBase,
  classe,
  methode
};

typedef struct _Decl
{ int val;
  enum Type type;
} VarDecl, *VarDeclP;

typedef struct _Var
{ char *name;
  int rank;
  enum VarType type; /* Etiquette objet ou variable normale */
  union
  {
  	struct _Objet *objet;
  	struct _ObjetIsole *objetIsole;
    VarDeclP varBase;
    struct _Class *classe;
    struct _Meth *methode;
  } u;
  struct _Var *next;
} Var, *VarP;

typedef struct _Meth
{ int constr;
  VarP decls;
  TreeP ListInstr;
} Meth, *MethP;


typedef struct _Class
{ struct _Class *superClass;
  MethP meths;
  VarP decls;
  // AttributP attributs;
} Class, *ClassP;

typedef struct _ObjetIsole
{ VarP decls;
  MethP meths;
} ObjetIsole, *ObjetIsoleP;

typedef struct _Objet
{ ClassP oClass;
  VarP attributs;
} Objet, *ObjetP;


enum Relop {
  EQU,
  DIF,
  INF,
  IEQ,
  SUP,
  SEQ
};
/* Type pour la valeur de retour de Flex et les actions de Bison
 * le premier champ est necessaire pour Flex.
 * les autres correspondent aux variantes utilisees dans les actions
 * associees aux productions de la grammaire.
*/
typedef union
{ char C;	/* caractere isole */
  enum Relop R; /* Contenu du relop */
  char *S;	/* chaine de caractere */
  int I;	/* valeur entiere */
  /*VarDeclP D;	 liste de paires (variable, valeur) */
  TreeP T;	/* AST */
  VarP V; /* Environnement : variable primitive, instance d'une classe, classe ou objet isolé */
  Meth M; /* Méthode */
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
VarP addToScope(VarP list, VarP nouv);
VarP declVar(char *name, TreeP tree, VarP decls);

/* evaluateur pour les parties declarations */
VarP evalDecls (TreeP tree);
int eval(TreeP tree, VarP decls);

/* evaluateur pour l'expression principale. Elle prend aussi en parametre
 * la liste des couples (variable, valeur) deduite des declarations
 */
int evalMain(TreeP tree, VarP decls);
