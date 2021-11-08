all: smpl.o rand.o cisj.o

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

cisj.o: cisj.c
	$(COMPILE.c) -g cisj.c

clean:
	$(RM) *.o