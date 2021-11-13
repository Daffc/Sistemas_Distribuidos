all: tarefa_3

tarefa_3: tarefa_3.o ../libs/smpl.o ../libs/rand.o
	$(MAKE) -C ../libs
	$(LINK.c) -o $@ -Bstatic tarefa_3.o ../libs/smpl.o ../libs/rand.o -lm

tarefa_3.o: tarefa_3.c ../libs/smpl.h
	$(COMPILE.c) -g  tarefa_3.c

clean:
	$(RM) *.o tarefa_3 relat saida

purge:
	$(MAKE) clean -C ../libs
	$(RM) *.o tarefa_3 relat saida