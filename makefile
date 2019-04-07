CFLAGS = -Wall -g #-DDEBUG
OBJECTS = conexao.o mensagens.o comandos.o io.o
LDLIBS = -lreadline

all: mestre escravo
	make clean

mestre: $(OBJECTS)
escravo: $(OBJECTS)

clean:
	rm -rf *.o

purge: clean
	rm -f mestre escravo

remake: purge
	make
	make clean
