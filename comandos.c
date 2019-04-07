#include "comandos.h"
#include "io.h"
#include <errno.h>

void leArquivo(int socket, char *nome_arquivo){
    int arquivo = open(nome_arquivo, O_RDWR | O_CREAT | O_TRUNC, 0777);
    mensagem msg_recebe;
    int fimarquivo = 0;
    do {
        int tipo = recebeMensagem(socket, &msg_recebe);
        if(tipo == TIPO_DADOS){
            write(arquivo, msg_recebe.dados, msg_recebe.tam);
        } else if(tipo == TIPO_FIM){
            fimarquivo = 1;
        }
    } while(!fimarquivo);
    close(arquivo);
}

void leLS(int socket){
    mensagem msg_recebe;
    int fimarquivo = 0;
    do {
        int tipo = recebeMensagem(socket, &msg_recebe);
        if(tipo == TIPO_MTELA){
            for (int i = 0; i < msg_recebe.tam; i++) printf("%c", msg_recebe.dados[i]);
        } else if(tipo == TIPO_FIM){
            fimarquivo = 1;
        }
    } while(!fimarquivo);
}

void enviaArquivo(int socket, char *nome_arquivo){
    struct stat dados_arq;
    long long int total_lidos = 0;
    int c_lidos = 0;
    char c;
    char dados[TAM_DADOS];
    mensagem msg_envia;
    int arquivo = open(nome_arquivo, O_RDONLY);
    fstat(arquivo, &dados_arq);

    while(total_lidos < dados_arq.st_size){
        c_lidos = 0;
        while((total_lidos < dados_arq.st_size) && (c_lidos < TAM_DADOS)){
            read(arquivo, &c, 1);
            dados[c_lidos] = c;
            c_lidos++;
            total_lidos++;
        }

        seq_envia++;
        montaMensagem(&msg_envia, TIPO_DADOS, c_lidos, dados);
        enviaMensagem(socket, &msg_envia);
    }

    seq_envia++;
    montaMensagem(&msg_envia, TIPO_FIM, sizeof(dados), dados);
    enviaMensagem(socket, &msg_envia);
}

void mestrePUT(int socket, char *nome_arq){
    int arquivo = open(nome_arq, O_RDONLY);

    if(arquivo == -1){
        perror("acesso ao arquivo local");
        return;
    }

    mensagem msg_recebe;
    mensagem msg_envia;
    char dados[TAM_DADOS];
    seq_envia++;
    montaMensagem(&msg_envia, TIPO_PUT, strlen(nome_arq), nome_arq);
    enviaMensagem(socket, &msg_envia);

    while(recebeMensagem(socket, &msg_recebe) != TIPO_OK);

    seq_envia++;
    struct stat starq;
    fstat(arquivo, &starq);
    close(arquivo);
    sprintf(dados, "%ld", starq.st_size);
    montaMensagem(&msg_envia, TIPO_TAM, strlen(dados), dados);
    enviaMensagem(socket, &msg_envia);

    int tipo;
    do {
        tipo = recebeMensagem(socket, &msg_recebe);
    } while((tipo != TIPO_OK) && (tipo != TIPO_ERRO));

    if(tipo == TIPO_ERRO){
        if(msg_recebe.dados[0] == 3){
            printf("espaço não disponível\n");
        } else {
            printf("erro desconhecido\n");
        }
    } else {
        enviaArquivo(socket, nome_arq);
    }

}

void escravoPUT(int socket, char *nome_arq){
    //checa permissão??
    mensagem msg_recebe;
    mensagem msg_envia;
    seq_envia++;
    enviaOK(socket);

    while(recebeMensagem(socket, &msg_recebe) != TIPO_TAM);
    seq_envia++;

    unsigned char dados[1];
    long long int tam;
    sscanf(msg_recebe.dados, "%lld", &tam);
    if(checaEspaco(tam)){
        enviaOK(socket);
        leArquivo(socket, nome_arq);
    } else {
        dados[0] = 3;
        montaMensagem(&msg_envia, TIPO_ERRO, 1, (char *) dados);
        enviaMensagem(socket, &msg_envia);
    }

}

void mestreGET(int socket, char *nome_arq){
    mensagem msg_recebe;
    mensagem msg_envia;
    seq_envia++;
    montaMensagem(&msg_envia, TIPO_GET, strlen(nome_arq), nome_arq);
    enviaMensagem(socket, &msg_envia);

    int tipo;
    do {
        tipo = recebeMensagem(socket, &msg_recebe);
    } while((tipo != TIPO_OK) && (tipo != TIPO_ERRO));

    if(tipo == TIPO_ERRO){
        if(msg_recebe.dados[0] == 1){
            printf("arquivo não existe\n");
        } else {
            printf("erro desconhecido\n");
        }
        return;
    }

    while(recebeMensagem(socket, &msg_recebe) != TIPO_TAM);

    seq_envia++;

    unsigned char dados[1];
    long long int tam;

    sscanf(msg_recebe.dados, "%lld", &tam);
    if(checaEspaco(tam)){
        enviaOK(socket);
        leArquivo(socket, nome_arq);
    } else {
        dados[0] = 3;
        montaMensagem(&msg_envia, TIPO_ERRO, 1, (char *) dados);
        enviaMensagem(socket, &msg_envia);
        printf("espaço insuficiente\n");
    }
}

void escravoGET(int socket, char *nome_arq){
    mensagem msg_recebe;
    mensagem msg_envia;
    char dados[TAM_DADOS];

    int arquivo = open(nome_arq, O_RDONLY);

    if(arquivo == -1){
        if(errno == ENOENT){
            dados[0] = 1;
            montaMensagem(&msg_envia, TIPO_ERRO, 1, dados);
        } else {
            dados[0] = 0;
            montaMensagem(&msg_envia, TIPO_ERRO, 1, dados);
        }
        seq_envia++;
        enviaMensagem(socket, &msg_envia);
        return;
    }

    seq_envia++;
    enviaOK(socket);

    seq_envia++;
    struct stat starq;
    fstat(arquivo, &starq);
    close(arquivo);

    sprintf(dados, "%ld", starq.st_size);
    montaMensagem(&msg_envia, TIPO_TAM, strlen(dados), dados);
    enviaMensagem(socket, &msg_envia);

    int tipo;
    do {
        tipo = recebeMensagem(socket, &msg_recebe);
    } while((tipo != TIPO_OK) && (tipo != TIPO_ERRO));

    if(tipo == TIPO_OK){
        enviaArquivo(socket, nome_arq);
    }

}

void mestreLS(int socket, int args){
    mensagem msg_envia;
    seq_envia++;
    char dados[TAM_DADOS];
    dados[0] = 1;
    sprintf(dados, "%d", args);
    montaMensagem(&msg_envia, TIPO_LS, 1, dados);
    enviaMensagem(socket, &msg_envia);

    leLS(socket);
}

void escravoLS(int socket, int args){
    int c_lidos = 0;
    FILE *arquivo = chamaLS(args - '0'); //gambiarra esquista
    char c;
    char dados[TAM_DADOS];
    mensagem msg_envia;
    c = fgetc(arquivo);

    while(c != EOF){
        c_lidos = 0;
        while((c_lidos < TAM_DADOS) && (c != EOF)){
            dados[c_lidos] = c;
            c_lidos++;
            c = fgetc(arquivo);
        }

        seq_envia++;
        montaMensagem(&msg_envia, TIPO_MTELA, c_lidos, dados);
        enviaMensagem(socket, &msg_envia);
    }

    seq_envia++;
    montaMensagem(&msg_envia, TIPO_FIM, sizeof(dados), dados);
    enviaMensagem(socket, &msg_envia);

    fclose(arquivo);
}

void mestreCD(int socket, char *nome_dir){
    mensagem msg_envia;
    mensagem msg_recebe;
    seq_envia++;
    montaMensagem(&msg_envia, TIPO_CD, strlen(nome_dir), nome_dir);
    enviaMensagem(socket, &msg_envia);

    int tipo;
    do {
        tipo = recebeMensagem(socket, &msg_recebe);
    } while((tipo != TIPO_OK) && (tipo != TIPO_ERRO));

    if(tipo == TIPO_ERRO){
        if(msg_recebe.dados[0] == 1){
            printf("diretório não existe\n");
        } else if (msg_recebe.dados[0] == 2){
            printf("acesso negado\n");
        } else {
            printf("erro desconhecido\n");
        }
    }
}

void escravoCD(int socket, char *nome_dir){
    mensagem msg_envia;

    if(chdir(nome_dir) == -1){
        seq_envia++;
        unsigned char dados[1];
        switch(errno){
            case ENOENT:
                dados[0] = 1;
                montaMensagem(&msg_envia, TIPO_ERRO, 1, (char *) dados);
                break;
            case ENOTDIR:
                dados[0] = 1;
                montaMensagem(&msg_envia, TIPO_ERRO, 1, (char *) dados);
                break;
            case EACCES:
                dados[0] = 2;
                montaMensagem(&msg_envia, TIPO_ERRO, 1, (char *) dados);
                break;
        }
        enviaMensagem(socket, &msg_envia);
    } else {
        seq_envia++;
        montaMensagem(&msg_envia, TIPO_OK, 0, (char *) msg_envia.dados);
        montaMensagem(&msg_envia, TIPO_OK, 0, (char *) msg_envia.dados);
        enviaMensagem(socket, &msg_envia);
    }
}
