#ifndef __MSGS
#define  __MSGS
#include <stdio.h>

#include "conexao.h"

#define PREAMBULO 126 // (01111110)b2
#define TAM_DADOS 31
#define SIZE_MSG 64

#define TIPO_ACK 0
#define TIPO_VAZIO 1
#define TIPO_TAM 2
#define TIPO_OK 3
#define TIPO_CD 6
#define TIPO_LS 7
#define TIPO_GET 8
#define TIPO_PUT 9
#define TIPO_FIM 10
#define TIPO_QUIT 11
#define TIPO_MTELA 12
#define TIPO_DADOS 13
#define TIPO_ERRO 14
#define TIPO_NACK 15

#define PARIDADE_OK 'k'
#define PARIDADE_NOTOK 'n'
#define SEQ_OK 'S'
#define SEQ_REPET 'R'
#define SEQ_NOTOK 'N'

extern unsigned int seq_envia;
extern unsigned int seq_recebe;

typedef struct {
	unsigned char inicio;
	unsigned int tam:5;
	unsigned int seq:6;
	unsigned int tipo:5;
	unsigned char dados[60];
	unsigned char paridade;
} mensagem;

void montaMensagem(mensagem *msg, unsigned int tipo, unsigned int tam, char *dados);

unsigned char calculaParidade(mensagem *msg);

int checaParidade(mensagem *msg);

int enviaMensagem(int socket, mensagem *msg);

int enviaOK(int socket);

int enviaQUIT(int socket);

int recebeMensagem(int socket, mensagem *msg);

void imprimeMensagem(mensagem *msg);

int checaSequencia(mensagem *msg, unsigned int *seq);
#endif
