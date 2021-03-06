object CptPoint is           /* objet compteur d'instances de Point */
{ var next : Integer;        /* index de la prochaine instance de Point */
  def CptPoint is
   {
    this
    .next
     := -1; }
  def
   incr():
    Integer is 
  { result 
  := this.next;
   this.next := this.next + 1;}
   
  def howMany() : 
  Integer := 
  
  this.next - 1 /* nbre d'instances creees */
}

/* la parametres precedes de var definissent implicitement des attributs */
class Point(var x: Integer, var y: Integer, var name: String) is
{
  var hasClone : Integer;   /* attributs supplementaires */

  /* la valeur du champ ci-dessous est indefinie si hasClone vaut 0.
   * Le probleme est qu'on n'a pas l'equivalent de NULL (du bon type)
   * dans le langage. On ne peut pas utiliser un 'object' pour cela car
   * un objet n'a pas de type defini. Ici on veut pouvoir connaitre le
   * dernier clone d'un meme objet.
  */
  var clone : Point;

  /* L'en-tete du constructeur doit etre identique a celui de la classe.
   * Les parametres précédés de var correspondent implictement à des attributs
   * de meme nom qui seront intialises automatiquement à la valeur du
   * parametre, comme si on avait la ligne suivante en tete du constructeur:
   *  this.x := x; this.y := y; this.nam := name;
   */

  def Point(var x: Integer, var y: Integer, var name: String) is
     { CptPoint.incr(); this.hasClone := 0; }

  def setName(s: String) is { this.name := s; }

  def getx() : Integer := this.x      /* pas de this implicite */

  def gety() : Integer := this.y

  def cloned() : Integer := this.hasClone <> 0

  def move(dx: Integer, dy: Integer, verbose: Integer) : Point is {
      this.x := this.x + dx; this.y := this.y + dy;
      if verbose then { this.print(verbose); } else {}
      /* pseudio variable 'result' dans laquelle, par convention, on laisse
       * la valeur de retour d'une fonction qui a un type de retour.
       */
      result := this;
  }

  def print(verbose : Integer) is {
      if verbose then "Inside Point::print".println(); else { }
      /* Exemples avec l'operateur & de concatenation de chaines */
      this.name.print();
      ( "= (" & this.x.toString() & ", " & this.y.toString() & ")" ).println();
  }

  /* par defaut, on ajoute un ' au nom de l'objet. On pourra le changer
   * apres coup avec setName si on veut
   */
  def clone() : Point is
  { 
    /* On memorise le dernier clone construit à partir de cet objet
     * si on voulait on pourrait ller le chainer a la fin de la liste des clones
    */
    this.clone := new Point(this.x, this.y, this.name & "'");
    this.hasClone := 1;
    result := this.clone;
  }

  /* imprime un objet et ses clones, s'ils existent */
  def allClones () is {
    this.print(0);
    if this.hasClone <> 0 then { this.clone.allClones(); } else { }
  }

  def egal(p: Point) : Integer is {
   /* Ils sont egaux s'ils ont memes coordonnees
    * On n'a pas de booleen donc on se sert des entiers, comme en C.
   */
       b1 : Integer := p.x - this.x;
       b2 : Integer := p.y - this.y;
       is
       if b1 then result := 0; else result := b2 = 0;
  }

  def egal2(p: Point) : Integer is {  /* autre version */
    b1 : Integer := p.x - this.x;
    b2: Integer;
    is
    result := 0;
    if b1 then { return; } else { }
       b2 := p.y - this.y;
       result := b2 = 0;
  }
}

class Couleur(var coul: Integer) is {
   /* 3 couleurs codees par  0, 1 ou 2    */

  def Couleur (var coul: Integer) is {
    if coul < 0 then coul := 0;
    else if coul > 2 then coul := 0; else {}
    this.coul := coul;
  }

  def name(verbose: Integer) : String is {
  aux : String := "Blanc";
  is
  if verbose then "Inside Couleur::couleur".println(); else {}
  if this.coul = 0 then { result := aux; }
  else { /* exemple avec deux blocs emboites qui chacun declare une variable.
          * Ici le aux externe doit etre masque.
          */
         aux : String := "Gris";
         is
         if this.coul = 1 then aux := "Noir"; else { }
         result := aux;
       }
  }

  def estGris() : Integer is {
     "Inside Couleur::estGris".println();
     result := this.coul = 2;
  }

}

 /* simule une classe avec des attributs et des methodes statiques puisqu'il
  * y aura toujours une seule instance (implicite) de l'objet
  */
object CouleurFactory is
{  var theBlanc: Couleur;
   var theNoir : Couleur;
   var theGris : Couleur;

  def CouleurFactory is {
      this.theBlanc := new Couleur(0);
      this.theNoir := new Couleur(1);
      this.theGris := new Couleur(2);
  }

  def blanc() : Couleur := this.theBlanc
  def noir() : Couleur :=  this.theNoir
  def gris() : Couleur :=  this.theGris
}


/* ci-dessous on ne met pas "var" devant x et y sinon ca definirait deux
 * nouveaux champs qui masqueraient ceux herites de Point.
 * On va fournir une valeur par defaut pour le parametre "name" du
 * constructeur de Point, donc on n'a pas besoin de demander sa valeur ici.
 */
class PointColore(x: Integer, y:Integer, var coul: Couleur)
      extends Point is {

  def PointColore (x: Integer, y:Integer, var coul: Couleur)
  
     : Point
     (x, y,
      "P-" & 
      CptPoint.howMany().toString()
       ) is
  { }

  def couleur() : Couleur := this.coul

  def colore() : Integer := this.coul.estGris() <> 0

  /* statiquement on renvoie un Point (pas un PointColore: pas de covariance)
   * mais dynamiquement  on cree bien une instance de PointColore.
   */
  override def clone() : Point is {
  /* ci-dessous x et y sont les champs herites de Point.
   * Helas on ne peut pas juste faire super.clone() car cela creerait une
   * instance de Point, pas de PointColore.
  */
       result := new PointColore(this.x, this.y, this.coul);
       result.setName(this.name & "'");
       this.hasClone := 1;
       this.clone := result;
  }

  def estGris() : Integer := this.coul.estGris()

  /* exemple de methode redefinie */
  override def print(verbose : Integer) is {
     if verbose then "Inside PointColore::print".println(); else { }
      super.print(verbose);                    /* usage de super */
      this.couleur().name(verbose).println();
  }
}


class PointNoir(xc: Integer, yc:Integer)
      extends PointColore is {
  def PointNoir(xc: Integer, yc:Integer) :
        PointColore(xc, yc, CouleurFactory.noir()) is { /* vide */ }

  /* autres exemples de methodes redefinies */
  override def estGris() : Integer := 0 /* false */
  override def colore() : Integer := 1  /* true */
  override def couleur() : Couleur := CouleurFactory.noir()
}


class DefaultPoint() /* Parentheses obligatoires */
      extends PointColore is {
  def DefaultPoint() : PointColore(0, 0, CouleurFactory.blanc()) is {}
  override def estGris() : Integer := 0
  override def couleur() : Couleur := CouleurFactory.blanc()
}

object Test is {
  def Test is {}
  def test(p: Point, p2: PointColore, p3: PointNoir) is {
      c: String; c2: String; c3: String;
      true: Integer := 1;
      false: Integer := 0;
      is
      p.print(true);
      p2.print(true);
      if p2.colore() <> 0 then c := "colore";  else c := "gris";
      if p3.colore() <> 0 then c2 := "colore"; else c2 := "gris";
      if p3.colore() <> 0 then c3 := "colore"; else c3 := "gris";
      "Resultats de test: ".println();
      c.print(); " ".print();
      c2.print(); " ".print();
      c3.print();
      "".println(); /* imprime une ligne vide */
  }

  def test2(p: PointColore) is {
      p.couleur().name(0).print();
  }
}

/* des exemples ad-hoc qui montrent d'autres aspects ... */
class A() is {
  var v : Integer;

  def A() is { this.v := 1 ;}
  def f() is { "A::f()\n".print(); }
  def h(x: Integer, y : Integer) : Integer := x + y
}

class A2() extends A is {
  var v : String; /* masque le champ v de la classe A */

  def A2() : A() is { this.v := "hello"; }

  override def f() is { "A2::f()\n".print(); }

  def g() is {
      this.v := "world"; /* par defaut, il s'agit du champ de A2 */
      /* this etant caste en A, le champ v en question est celui de A */
      (A this).v := 1;
      /* les deux appels doivent fournir la meme chose */
      this.f();
      (A this).f();
  }

  override def h(x : Integer, y: Integer) : Integer := super.h(x, y)
}

{ /* Bloc qui correspond au programme principal */
  true: Integer := 1;
  p1: Point := new Point(1, 5, "p1");
  p2: Point := new Point(2, 3, "p2");
  p3 : Point := new Point(0, 0, "p3");
  o : PointColore := new PointColore(50, 100, CouleurFactory.noir());
  o2: Point;
  pn : PointNoir := new PointNoir(+1, -1);
  dp : DefaultPoint := new DefaultPoint();
is
  p2.move(p1.getx(), p1.gety(), true);
  o.setName("origine");
  p2.move(p1.getx()-2*5-3, p1.gety(), true);
  o.print(true);
  p2.print(true);

  "\nClonage de o:".println();
  o2 := o.clone();
  o2.print(true);
  /* on clone le clone et on le deplace */
  o2.clone().move(54, 36, 0).print(true);
  "\nListe des clones de o:".println();
  o.allClones();
  "Fin de la liste\n".println();
  /* Ne doit pas compiler car clone() renvoie statiquement un Point alors
   * que o est declare comme PointColore
   * o := o.clone();
   */

  "Seconde liste des clones de o:".println();
  o2 := p1.clone();
  o2.move(+2, -3, 0);
  p1.print(0); /* les coordonnees ne sont pas modifiees */
  o2.clone();
  p1.allClones();
  "Fin de la seconde liste\n".println();

  "test(Point, PointColore, PointNoir)".println();
  Test.test(p1, o, pn);
  "test(PointNoir, PointNoir, PointNoir)".println();
  Test.test(pn, pn, pn);
  p1 := pn; /* affectation entre pointeurs ! */
  Test.test2(o);
  Test.test2(pn);
  o := pn;  /* Idem : on doit avoir de la liaison dynamique ci-dessous */

  "test(PointNoir, PointNoir, PointNoir)".println();
  Test.test(p1, o, pn);
  Test.test2(o);
  Test.test2(pn);
  "\nDone".println();
}

