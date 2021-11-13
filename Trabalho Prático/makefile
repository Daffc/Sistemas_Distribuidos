# Autor: Douglas Affonso Clementino
# Data da última modificação: 12/11/2021

all: smpl.o rand.o cisj.o trabPrat

trabPrat: trabPrat.o smpl.o rand.o cisj.o evento.o 
	$(LINK.c) -o $@ -Bstatic trabPrat.o smpl.o rand.o cisj.o evento.o -lm

trabPrat.o: trabPrat.c smpl.h cisj.h nodo.h evento.h
	$(COMPILE.c) -g  trabPrat.c

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

cisj.o: cisj.c
	$(COMPILE.c) -g cisj.c

evento.o: evento.c evento.c nodo.h
	$(COMPILE.c) -g evento.c

clean:
	$(RM) *.o

purge: clean	
	$(RM) trabPrat relat saida