all: smpl.o rand.o

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

clean:
	$(RM) *.o