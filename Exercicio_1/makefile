all: tarefa_1

tarefa_1: tarefa_1.o ../libs/smpl.o ../libs/rand.o
	$(MAKE) -C ../libs
	$(LINK.c) -o $@ -Bstatic tarefa_1.o ../libs/smpl.o ../libs/rand.o -lm

tarefa_1.o: tarefa_1.c ../libs/smpl.h
	$(COMPILE.c) -g  tarefa_1.c

clean:
	$(RM) *.o tarefa_1 relat saida

purge:
	$(MAKE) clean -C ../libs
	$(RM) *.o tarefa_1 relat saida