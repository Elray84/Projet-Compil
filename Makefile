OBJ=proj.o lex_l.o gram_y.o
CC=gcc
CFLAGS=-Wall -std=c99 -I./ -g
LDFLAGS= -g -lfl
proj : $(OBJ)
	$(CC) -o proj $(OBJ) $(LDFLAGS)

test_lex: gram_y.h test_lex.o lex_l.o
	$(CC) -o test_lex  test_lex.o lex_l.o $(LDFLAGS)

#bison: gram_y.o lex_l.o
#    echo "on y est"
#    $(CC) -o bison gram_y.o lex_l.o $(LDFLAGS)

# Si absent lance yacc et fait "mv y.tab.c tp.c" ce qui ecrase notre fichier.
proj.c :
	echo ''

proj.o: proj.c gram_y.h proj.h
	$(CC) $(CFLAGS) -c proj.c

lex_l.o: lex_l.c gram_y.h
	$(CC) $(CFLAGS) -Wno-unused-function -Wno-implicit-function-declaration -c lex_l.c

gram_y.o : gram_y.c
	$(CC) $(CFLAGS) -c gram_y.c

# bison
gram_y.h gram_y.c : gram.y proj.h
	bison -v -d -o gram_y.c gram.y

test_lex.o : test_lex.c proj.h gram_y.h
	$(CC) $(CFLAGS) -c test_lex.c

lex_l.c : gram_y.h proj.h anal_lex.l
	flex --yylineno -olex_l.c anal_lex.l

.Phony: clean
# commande pour tester grammaire : bison -v -b gram_y -d gram.y
clean:
	rm -f *~ tp.exe* ./tp *.o lex_l.o gram_y.* test_lex lex_l.c gram_y.output
