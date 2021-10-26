all: tempo

tempo: tempo.o ../libs/smpl.o ../libs/rand.o
	$(MAKE) -C ../libs
	$(LINK.c) -o $@ -Bstatic tempo.o ../libs/smpl.o ../libs/rand.o -lm

tempo.o: tempo.c ../libs/smpl.h
	$(COMPILE.c) -g  tempo.c

clean:
	$(RM) *.o tempo relat saida

purge:
	$(MAKE) clean -C ../libs
	$(RM) *.o tempo relat saida