all: tarefa_2

tarefa_2: tarefa_2.o ../libs/smpl.o ../libs/rand.o
	$(MAKE) -C ../libs
	$(LINK.c) -o $@ -Bstatic tarefa_2.o ../libs/smpl.o ../libs/rand.o -lm

tarefa_2.o: tarefa_2.c ../libs/smpl.h
	$(COMPILE.c) -g  tarefa_2.c

clean:
	$(RM) *.o tarefa_2 relat saida

purge:
	$(MAKE) clean -C ../libs
	$(RM) *.o tarefa_2 relat saida