all: tarefa_0

tarefa_0: tarefa_0.o ../libs/smpl.o ../libs/rand.o
	$(MAKE) -C ../libs
	$(LINK.c) -o $@ -Bstatic tarefa_0.o ../libs/smpl.o ../libs/rand.o -lm

tarefa_0.o: tarefa_0.c ../libs/smpl.h
	$(COMPILE.c) -g  tarefa_0.c

clean:
	$(RM) *.o tarefa_0 relat saida

purge:
	$(MAKE) clean -C ../libs
	$(RM) *.o tarefa_0 relat saida