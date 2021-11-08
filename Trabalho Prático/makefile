all: smpl.o rand.o cisj.o tempo

tempo: tempo.o smpl.o rand.o cisj.o
	$(LINK.c) -o $@ -Bstatic tempo.o smpl.o rand.o cisj.o -lm

tempo.o: tempo.c smpl.h cisj.h
	$(COMPILE.c) -g  tempo.c

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

cisj.o: cisj.c
	$(COMPILE.c) -g cisj.c

clean:
	$(RM) *.o

purge: clean	
	$(RM) tempo relat saida