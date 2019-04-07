#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "mensagens.h"
#include "conexao.h"

struct timeval tempo_to;

extern char contexto;

void montaMensagem(mensagem *msg, unsigned int tipo, unsigned int tam, char *dados){
	memset(msg->dados, 0, TAM_DADOS);
	msg->tipo = tipo;
	msg->tam = tam; //calculado antes, passado como arg
	if(tam > 0){
		for(unsigned int i = 0; i < tam; i++){
			msg->dados[i] = dados[i];
		}
	}
}

unsigned char calculaParidade(mensagem *msg){
	char paridade = 0;
	char *msg_char = (char *)msg;
	for(unsigned int i = 1; i < 4+TAM_DADOS-1; i++){
		paridade ^= msg_char[i];
	}

	return paridade;
}

int checaParidade(mensagem *msg){
	unsigned char paridade = calculaParidade(msg);
	#ifdef DEBUG
        printf("%d == %d :: %d\n", paridade, msg->paridade, (paridade == msg->paridade));
	#endif
	return (paridade == msg->paridade);
}

int enviaMensagem(int socket, mensagem *msg){
    mensagem recebe;
	msg->inicio = PREAMBULO;
	msg->seq = seq_envia; //atualizar antes, fora do loop de envia
	msg->paridade = calculaParidade(msg);
    tempo_to.tv_sec = 1;
	tempo_to.tv_usec = 0;
    int cont_timeout = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tempo_to, sizeof(tempo_to));

    do{
        #ifdef DEBUG
            printf(">>>>Enviando:\n");
            imprimeMensagem(msg);
        #endif
        send(socket, msg, SIZE_MSG, 0);
        cont_timeout++;
        if(cont_timeout > 99){
            if(contexto){
                printf("TIMEOUT - MESTRE ENCERRADO, SEM CONEXÃO COM ESCRAVO (STATUS DESCONHECIDO)\n");
            } else {
                printf("CONEXÃO COM MESTRE PERDIDA - ENCERRANDO EXECUÇÃO\n");
            }
            exit(-1);
        }
    } while(recebeMensagem(socket, &recebe) != TIPO_ACK);

    tempo_to.tv_sec = 0;
    tempo_to.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tempo_to, sizeof(tempo_to));

    return 0;
}

int enviaACK(int socket){
    mensagem msg;
	msg.inicio = PREAMBULO;
    msg.tipo = TIPO_ACK;
    msg.seq = seq_envia;
    msg.paridade = calculaParidade(&msg);

    #ifdef DEBUG
        printf(">>>>Enviando:\n");
        imprimeMensagem(&msg);
    #endif
    send(socket, &msg, SIZE_MSG, 0);
	return 0;
}

int enviaNACK(int socket){
    mensagem msg;
	msg.inicio = PREAMBULO;
    msg.tipo = TIPO_NACK;
    msg.seq = seq_envia;
    msg.paridade = calculaParidade(&msg);

    #ifdef DEBUG
        printf(">>>>Enviando:\n");
        imprimeMensagem(&msg);
    #endif
    send(socket, &msg, SIZE_MSG, 0);
	return 0;
}

int enviaOK(int socket){
    mensagem msg;
    mensagem recebe;
	msg.inicio = PREAMBULO;
    msg.tipo = TIPO_OK;
    msg.seq = seq_envia;
    msg.paridade = calculaParidade(&msg);

    do{
        #ifdef DEBUG
            printf(">>>>Enviando:\n");
            imprimeMensagem(&msg);
        #endif
        send(socket, &msg, SIZE_MSG, 0);
    } while(recebeMensagem(socket, &recebe) != TIPO_ACK);
	return 0;
}

int enviaQUIT(int socket){
    mensagem msg;
    mensagem recebe;
	msg.inicio = PREAMBULO;
    msg.tipo = TIPO_QUIT;
    msg.seq = seq_envia;
    msg.paridade = calculaParidade(&msg);

    do{
        #ifdef DEBUG
            printf(">>>>Enviando:\n");
            imprimeMensagem(&msg);
        #endif
        send(socket, &msg, SIZE_MSG, 0);
    } while(recebeMensagem(socket, &recebe) != TIPO_ACK);
	return 0;
}

int recebeMensagem(int socket, mensagem *msg){
	int sequencia = -1;
	recv(socket, msg, SIZE_MSG, 0);

	if(msg->inicio == PREAMBULO){
        #ifdef DEBUG
    		printf(">>>>Recebendo:\n");
    		imprimeMensagem(msg);
        #endif
		sequencia = checaSequencia(msg, &seq_recebe);
		if(checaParidade(msg)){
			if((sequencia == SEQ_OK) || (msg->tipo == TIPO_ACK)){
                if(msg->tipo != TIPO_ACK){
                    enviaACK(socket);
                }
				return msg->tipo;
			} else if((sequencia == SEQ_REPET) && (msg->tipo != TIPO_ACK)){
				if(msg->tipo != TIPO_NACK){
					enviaACK(socket);
					return SEQ_REPET;
				}
			}
		} else {
            enviaNACK(socket);
            seq_recebe--;
			return PARIDADE_NOTOK;
		}
	}

	return sequencia;
}

void imprimeMensagem(mensagem *msg){
	// printf("Início: %d\n", msg->inicio);
	printf("Tamanho: %d\n", msg->tam);
	printf("Sequência: %d\n", msg->seq);
	printf("Tipo: %d\n", msg->tipo);
	printf("Dados:");
	for (int i = 0; i < TAM_DADOS; ++i) {
		printf(" %x", msg->dados[i]);
	}
	printf("\n");
	printf("Paridade: %d\n", msg->paridade);
}

// mudou o valor de retorno
int checaSequencia(mensagem *msg, unsigned int *seq){
	int prox_seq = (*seq+1) % 64;
	if(msg->seq == prox_seq){
		*seq = prox_seq;
		return SEQ_OK;
	} else if (msg->seq == *seq){
		return SEQ_REPET;
	} else {
		return SEQ_NOTOK;
	}
}
