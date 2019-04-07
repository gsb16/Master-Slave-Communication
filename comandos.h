#ifndef __COMANDOS
#define __COMANDOS
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mensagens.h"

void leArquivo(int socket, char *nome_arquivo);

void enviaArquivo(int socket, char *nome_arquivo);

void mestrePUT(int socket, char *nome_arq);

void escravoPUT(int socket, char *nome_arq);

void mestreGET(int socket, char *nome_arq);

void escravoGET(int socket, char *nome_arq);

void mestreLS(int socket, int args);

void escravoLS(int socket, int args);

void mestreCD(int socket, char *nome_dir);

void escravoCD(int socket, char *nome_dir);

#endif
