#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mensagens.h"
#include "comandos.h"
#include "conexao.h"
#include "io.h"

unsigned int seq_envia;
unsigned int seq_recebe;

char contexto = LOCAL;

int main(int argc, char *argv[]){
	int socket = ConexaoRawSocket(argv[1]);
	seq_envia = 0;
	seq_recebe = 0;
    char *linha;

    while(1){
        int cmd = leComando(&linha);
        executaComando(socket, cmd, linha);
    }

	return 0;
}
