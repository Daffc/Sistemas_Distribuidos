all: tarefa_4

tarefa_4: tarefa_4.o ../libs/smpl.o ../libs/rand.o
	$(MAKE) -C ../libs
	$(LINK.c) -o $@ -Bstatic tarefa_4.o ../libs/smpl.o ../libs/rand.o -lm

tarefa_4.o: tarefa_4.c ../libs/smpl.h
	$(COMPILE.c) -g  tarefa_4.c

clean:
	$(RM) *.o tarefa_4 relat saida

purge:
	$(MAKE) clean -C ../libs
	$(RM) *.o tarefa_4 relat saida