CC := gcc
LIBS := -lm
COMPILEDIR := ./compile
build:
	rm -rf $(COMPILEDIR)/picgen
	$(CC) -O2 -o $(COMPILEDIR)/picgen picgen.c $(LIBS)

debug:
	rm -rf $(COMPILEDIR)/picgen_debug
	$(CC) -g3 -DDEBUG -o $(COMPILEDIR)/picgen_debug picgen.c $(LIBS)
