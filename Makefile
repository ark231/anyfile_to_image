CC := gcc
LIBS := -lm
COMPILEDIR := ./compile
build:
	rm -rf $(COMPILEDIR)/picgen
	$(CC) -O2 -o $(COMPILEDIR)/picgen picgen.c $(LIBS)

debug:
	rm -rf $(COMPILEDIR)/picgen
	$(CC) -g3 -DDEBUG -o $(COMPILEDIR)/picgen picgen.c $(LIBS)
