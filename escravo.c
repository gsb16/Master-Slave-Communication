#include <stdio.h>
#include <unistd.h>

#include "mensagens.h"
#include "comandos.h"
#include "conexao.h"
#include "io.h"

unsigned int seq_envia;
unsigned int seq_recebe;

char contexto = 0;

int main(int argc, char *argv[]){
	int socket = ConexaoRawSocket(argv[1]);
    seq_envia = 0;
	seq_recebe = 0;

	mensagem msg_recebe;

    while(1){
		while(!recebeMensagem(socket, &msg_recebe));
        executaOrdem(socket, msg_recebe);
    }

	return 0;
}
