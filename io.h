
#ifndef __IO
#define  __IO
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "mensagens.h"
#include "comandos.h"

#define REMOTO 'R'
#define LOCAL 'L'

extern char contexto;

#define CMD_QUIT 10
#define CMD_REMOTO 20
#define CMD_LOCAL 21
#define CMD_LS 40
#define CMD_CD 41
#define CMD_PUT 42
#define CMD_GET 43
#define CMD_UNK 50

int checaEspaco(long long int tam);

char leComando(char **linha);

void erroComando();

void executaCD(int socket, char *linha);

void executaLS(int socket, char *linha);

FILE *chamaLS(int tipo);

int executaComando(int socket, char tipo, char *linha);

int executaOrdem(int socket, mensagem msg);
#endif
