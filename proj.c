#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "proj.h"
#include "gram_y.h"

extern int yyparse();
extern int yylineno;

extern char *strdup(const char *s);
extern int genCode(TreeP, VarP);

/* Niveau de 'verbosite'.
 * Par defaut, n'imprime que le resultat et les messages d'erreur
 */
bool verbose = FALSE;

/* Evaluation ou pas. Par defaut, on evalue les expressions */
bool noEval = FALSE;

/* code d'erreur a retourner: liste dans tp.h */
int errorCode = NO_ERROR;

/* yyerror:  fonction importee par Bison et a fournir explicitement.
 * Elle est appelee quand Bison detecte une erreur syntaxique.
 * Ici on se contente d'un message minimal.
 */
void yyerror(char *ignore) {
  fprintf(stderr, "Syntax error on line: %d\n", yylineno);
}


/* mémorise le code d'erreur et s'arrange pour bloquer l'évaluation */
void setError(int code) {
  errorCode = code;
  if (code != NO_ERROR) { noEval = TRUE; }
}


/* Appel:
 *   tp [-option]* programme.txt donnees.dat
 * Le fichier de donnees est obligatoire si le programme execute la
 * construction GET (pas de lecture au clavier), facultatif sinon.
 * Les options doivent apparaitre avant le nom du fichier du programme.
 * Options: -[eE] -[vV] -[hH?]
 */
int main(int argc, char **argv) {
  int fi;
  int i, res;

  for(i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'v': case 'V':
	verbose = TRUE; continue;
      case 'e': case 'E':
	noEval = TRUE; continue;
      case '?': case 'h': case 'H':
	fprintf(stderr, "Syntax: tp -e -v program.txt\n");
	exit(USAGE_ERROR);
      default:
	fprintf(stderr, "Error: Unknown Option: %c\n", argv[i][1]);
	exit(USAGE_ERROR);
      }
    } else break;
  }

  if (i == argc) {
    fprintf(stderr, "Error: Program file is missing\n");
    exit(USAGE_ERROR);
  }

  if ((fi = open(argv[i++], O_RDONLY)) == -1) {
    fprintf(stderr, "Error: Cannot open %s\n", argv[i-1]);
    exit(USAGE_ERROR);
  }

  /* redirige l'entree standard sur le fichier... */
  close(0); dup(fi); close(fi);

  if (i < argc) { /* fichier dans lequel lire les valeurs pour get() */
    fprintf(stderr, "Error: extra argument: %s\n", argv[i]);
    exit(USAGE_ERROR);
  }

  /* Lance l'analyse syntaxique de tout le source, en appelant yylex au fur
   * et a mesure. Execute les actions semantiques en parallele avec les
   * reductions.
   * yyparse renvoie 0 si le source est syntaxiquement correct, une valeur
   * differente de 0 en cas d'erreur syntaxique (eventuellement dues a des
   * erreurs lexicales).
   * Comme l'interpretation globale est automatiquement lancee par les actions
   * associees aux reductions, une fois que yyparse a termine il n'y
   * a plus rien a faire (sauf fermer les fichiers)
   * Si le code du programme contient une erreur, on bloque l'evaluation.
   * S'il n'y a que des erreurs contextuelles on essaye de ne pas s'arreter
   * a la premiere mais de continuer l'analyse pour en trovuer d'autres, quand
   * c'est possible.
   */
  res = yyparse();
  if (res == 0 && errorCode == NO_ERROR) return 0;
  else {
    return res ? SYNTAX_ERROR : errorCode;
  }
}


/* Fonction auxiliaire pour la construction d'arbre : renvoie un squelette
 * d'arbre avec 'nbChildre'n fils et d'etiquette 'op' donnee. Les appelants
 * doivent eux-mêmes stocker les fils une fois que MakeNode a renvoye son
 * resultat
 */
TreeP makeNode(int nbChildren, short op) {
  TreeP tree = NEW(1, Tree);
  tree->op = op; tree->nbChildren = nbChildren;
  tree->u.children = nbChildren > 0 ? NEW(nbChildren, TreeP) : NIL(TreeP);
  return(tree);
}


/* Construction d'un arbre a nbChildren branches, passees en parametres
 * 'op' est une etiquette symbolique qui permet de memoriser la construction
 * dans le programme source qui est representee par cet arbre.
 * Une liste preliminaire d'etiquettes est dans tp.h; il faut l'enrichir selon
 * vos besoins.
 * Cette fonction prend un nombre variable d'arguments: au moins deux.
 * Les eventuels arguments supplementaires doivent etre de type TreeP
 * (defini dans tp.h)
 */
TreeP makeTree(short op, int nbChildren, ...) {
  va_list args;
  int i;
  TreeP tree = makeNode(nbChildren, op);
  va_start(args, nbChildren);
  for (i = 0; i < nbChildren; i++) {
    tree->u.children[i] = va_arg(args, TreeP);
  }
  va_end(args);
  return(tree);
}


/* Retourne le i-ieme fils d'un arbre (de 0 a n-1) */
TreeP getChild(TreeP tree, int i) {
  return tree->u.children[i];
}


/* Constructeur de feuille dont la valeur est un entier */
TreeP makeLeafInt(short op, int val) {
  TreeP tree = makeNode(0, op);
  tree->u.val = val;
  return(tree);
}


/* Verifie que nouv n'apparait pas deja dans list. l'ajoute en tete et
 * renvoie la nouvelle liste
 */
VarP addToScope(VarP list, VarP nouv) {
  VarP p; int i;
  for(p=list, i = 0; p != NIL(Var); p = p->next, i = i+1) {
    if (! strcmp(p->name, nouv->name)) {
      fprintf(stderr, "Error: Multiple declaration in the same scope of %s\n",
	      p->name);
      setError(CONTEXT_ERROR);
      break;
    }
  }
  /* On continue meme en cas de double declaration, pour pouvoir eventuellement
   * detecter plus d'une erreur
   */
  nouv->rank = i; nouv->next=list;
  return nouv;
}


/* Construit le squelette d'un element de description d'une variable */
/*VarDeclP makeVar(char *name) {
  VarDeclP res = NEW(1, VarDecl);
  res->name = name; res->next = NIL(VarDecl);
  return(res);
}*/


/**
 * 	A partir d'ici les fonctions ont besoin d'etre modifiees/completees
 **/

char *getLabel() {
  static char buf[5];
  static int cpt = 1;
  sprintf(buf, "Eti%d", cpt++);
  return strdup(buf);
}


/* Avant evaluation, verifie si tout identificateur qui apparait dans tree a
 * bien ete declare (dans ce cas il doit etre dans la liste lvar).
 * On impose que ca soit le cas y compris si on n'a pas besoin de cet
 * identificateur pour l'evaluation, comme par exemple x dans
 * begin if 1 = 1 then 1 else x end
 * Le champ 'val' de la structure VarDecl n'est pas significatif
 * puisqu'on n'a encore rien evalue.
 */
bool checkScope(TreeP tree, VarP lvar) {
  VarP p; char *name;
  if (tree == NIL(Tree)) { return TRUE; }
  switch (tree->op) {
  case hID :
    name = tree->u.str;
    for(p=lvar; p != NIL(Var); p = p->next) {
      if (! strcmp(p->name, name)) { return TRUE; }
    }
    fprintf(stderr, "\nError: undeclared variable %s\n", name);
    setError(CONTEXT_ERROR);
    return FALSE;
  case hCST:
    return TRUE;
  case hIF:
    return checkScope(getChild(tree, 0), lvar)
      && checkScope(getChild(tree, 1), lvar)
      && checkScope(getChild(tree, 2), lvar);
  case hTHE:
  case hELS:
  case hEQU:
  case hDIF:
  case hSUP:
  case hSEQ:
  case hINF:
  case hIEQ:
  case hADD:
  case hSUB:
  case hMUL:
  case hDIV:
    return checkScope(getChild(tree, 0), lvar)
             && checkScope(getChild(tree, 1), lvar);
  default:
    fprintf(stderr, "Erreur! etiquette indefinie: %d\n", tree->op);
    exit(UNEXPECTED);
  }
}


VarP declVar(char *name, TreeP tree, VarP decls) {
  VarP pvar = NEW(1, Var);
  pvar->name = name; pvar->next = NIL(Var);
  checkScope(tree, decls);
  // genCode(tree, decls);
  return addToScope(decls, pvar);
}




/* Constructeur de feuille dont la valeur est une chaine de caracteres
 * Si check vaut Vrai, Verifie si cet identificateur a bien ete declare.
 */
TreeP makeLeafStr(short op, char *str) {
  TreeP tree = makeNode(0, op);
  tree->u.str = str;
  return(tree);
}


/* GENERATION DE CODE
|
|
|
|
|
|
|
|
|
|
|
|
|
|
|
*/


VarP genCodeAff (TreeP tree, VarP decls) {
  if (tree == NIL(Tree) || tree->op != hDCL) {
    exit(UNEXPECTED);
  } else {
    /* Va laisser la valeur de la nouvelle variable en sommet de pile
     * donc rien de special a faire, puisqu'on va lui associer comme rang
     * cette adresse dans la pile
     */
    TreeP fils = getChild(tree, 1);
    return declVar(getChild(tree, 0)->u.str, fils, decls);
  }
}


VarP genCodeDecls (TreeP tree) {
  if (tree == NIL(Tree)) {
    printf("START\n");    /* initialisation des registres internes */
    return NIL(Var);
  }
  else {
    TreeP g, d; VarP res;
    g = getChild(tree, 0);
    d = getChild(tree, 1);
    res = genCodeDecls(g);
    res = genCodeAff(d, res);
    return res;
  }
}


/* generation de code pour un if then else
 * le premier fils represente la condition,
 * les deux autres fils correspondent respectivement aux parties then et else.
 */
int genCodeIf(TreeP tree, VarP decls) {
  char * etiFalse = getLabel();
  char * etiFin = getLabel();
  genCode(getChild(tree, 0), decls);
  printf("JZ %s\n", etiFalse);
  genCode(getChild(tree, 1), decls);
  printf("JUMP %s\n", etiFin);
  printf("%s: ", etiFalse);
  genCode(getChild(tree, 2), decls);
  printf("%s: NOP\n", etiFin);
  return 0;
}


int getLocVar(char *name, VarP decls) {
  VarP l = decls;
  while (l != NIL(Var)) {
    if (! strcmp(name, l->name)) { return l->rank; }
    else { l = l->next; }
  }
  if (errorCode == NO_ERROR) {
    	fprintf(stderr, "Unexpected error: undeclared variable %s\n", name);
	exit(UNEXPECTED);
  } else { return -1; /* code generation will not complete anyway */ }
}


/* generation de code pour une expression.
 * tree: l'AST de l'expression
 * decls: l'environnement, c'est à dire la liste des variables que l'expression
 * a le droit de referencer.
 */
int genCode(TreeP tree, VarP decls) {
  if (tree == NIL(Tree)) { exit(UNEXPECTED); }
  switch (tree->op) {
 case hID:
   printf("PUSHG %d \t-- %s\n", getLocVar(tree->u.str, decls), tree->u.str);
   break;
  case hCST:
    printf("PUSHI %d\n", tree->u.val);
    break;
  case hEQU:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("EQUAL\n");
    break;
  case hDIF:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("EQUAL\nNOT\n");
    break;
  case hSUP:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("SUP\n");
    break;
  case hSEQ:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("SUPEQ\n");
    break;
  case hINF:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("INF\n");
    break;
  case hIEQ:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("INFEQ\n");
    break;
  case hADD:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("ADD\n");
    break;
  case hSUB:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("SUB\n");
    break;
  case hMUL:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("MUL\n");
    break;
  case hDIV:
    genCode(getChild(tree, 0), decls); genCode(getChild(tree, 1), decls);
    printf("DUPN 1\nJZ DIV0\n");
    break;
  case hIF:
    genCodeIf(tree, decls);
    break;
  case hTHE:
  case hELS:
  default:
    fprintf(stderr, "Erreur! etiquette indefinie: %d\n", tree->op);
    exit(UNEXPECTED);
  }
  return 0;
}

/* generation de code pour l'expression finale du programme.
 * tree : expression comprise entre le BEGIN et le END
 * decls : l'environnement forme par les variables declarees
 */
int genCodeMain(TreeP tree, VarP decls) {
  /* verifie les ident utilises dans l'expression finale */
  checkScope(tree, decls);
  if (noEval) {
    fprintf(stderr, "Skipping evaluation step.\n");
  } else {
    if (errorCode == NO_ERROR) {
      /* if errorCode != NO_ERROR then noEval shoud be equals to true */
      genCode(tree, decls);
      printf("PUSHS \"Resultat global: \"\nWRITES\nWRITEI\n");
      printf("PUSHS \"\\n\"\nWRITES\nSTOP\n");
      printf("DIV0: ERR \"Tentative de division par 0\"\n");
    } else { fprintf(stderr, "Code generation stopped because of errors\n"); }
  }
  return errorCode;
}
