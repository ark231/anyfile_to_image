CC := gcc
LIBS := -lm
build:
	rm -rf picgen
	$(CC) -O2 -o picgen picgen.c $(LIBS)

debug:
	rm -rf picgen
	$(CC) -g3 -DDEBUG -o picgen picgen.c $(LIBS)
