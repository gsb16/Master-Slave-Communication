#include "io.h"
#include <errno.h>
#include <sys/statvfs.h>

int checaEspaco(long long int tam){
    struct statvfs stat;
    statvfs("./", &stat);

    return (tam < (stat.f_bavail * stat.f_bsize));
}

char leComando(char **linha){
	char prompt[4] = {'X', '>', '>', '\0'};
	prompt[0] = contexto;
	char *string = (char *) malloc(sizeof(char)*100);
	*linha = readline(prompt);
	sscanf(*linha, "%s", string);

	if(strcmp(string, "ls") == 0){
		return CMD_LS;
	} else if(strcmp(string, "cd") == 0){
		return CMD_CD;
	} else if(strcmp(string, "put") == 0){
		return CMD_PUT;
	}else if(strcmp(string, "get") == 0){
		return CMD_GET;
	}else if(strcmp(string, "R") == 0){
		return CMD_REMOTO;
	}else if(strcmp(string, "L") == 0){
		return CMD_LOCAL;
	}else if(strcmp(string, "quit") == 0){
		return CMD_QUIT;
	} else {
		return CMD_UNK;
	}
}

void erroComando(){
	printf("comando inválido, não entendi :(\n");
}

void executaCD(int socket, char *linha){
	char str1[100];
	char str2[100];
	char str3[100];
	int i = sscanf(linha, "%s %s %s", str1, str2, str3);
	if(i == 2){
		if(contexto == LOCAL){
			if(chdir(str2) == -1){
                switch(errno){
                    case ENOENT:
                        printf("dir não existe\n");
                        break;
                    case ENOTDIR:
                        printf("dir não existe\n");
                        break;
                    case EACCES:
                        printf("acesso negado\n");
                        break;
                    default:
                        printf("erro desconhecido\n");
                        break;
                }
            }
		} else if(contexto == REMOTO){
			mestreCD(socket, str2);
		}
	} else {
		erroComando();
	}
}

FILE *chamaLS(int tipo){
	FILE *fp;
	switch (tipo) {
		case 0:
			fp = popen("ls", "r");
			break;
		case 1:
			fp = popen("ls -l", "r");
			break;
		case 2:
			fp = popen("ls -a", "r");
			break;
		case 3:
			fp = popen("ls -l -a", "r");
			break;
	}
	return fp;
}

void executaLS(int socket, char *linha){
	char str[4][100];
	int tipo = 0;
	int i = sscanf(linha, "%s %s %s %s", str[0], str[1], str[2], str[3]);
	if((i < 4) && (i > 0)){
		for(int k = 0; k < 4; k++){
			if(strcmp(str[k],"-l") == 0){
				tipo += 1;
			}
			if(strcmp(str[k],"-a") == 0){
				tipo += 2;
			}
		}
		if(contexto == LOCAL){
			FILE *fp = chamaLS(tipo);

            char c;
			while ((c = fgetc(fp)) != EOF){
				printf("%c", c);
			}
		} else if(contexto == REMOTO){
			mestreLS(socket, tipo);
		}
	} else {
		erroComando();
	}
}

int executaComando(int socket, char tipo, char *linha){
    char nome_arq[TAM_DADOS];
    switch(tipo){
		case CMD_CD:
			executaCD(socket, linha);
			break;
		case CMD_LS:
			executaLS(socket, linha);
			break;
        case CMD_PUT:
            sscanf(linha, "%s %s\n", nome_arq, nome_arq);
            mestrePUT(socket, nome_arq);
            break;
        case CMD_GET:
            sscanf(linha, "%s %s\n", nome_arq, nome_arq);
            mestreGET(socket, nome_arq);
            break;
		case CMD_LOCAL:
			contexto = LOCAL;
			break;
		case CMD_REMOTO:
			contexto = REMOTO;
			break;
		case CMD_QUIT:
            seq_envia++;
            enviaQUIT(socket);
			exit(-1);
		case CMD_UNK:
			erroComando();
			break;
	}

	return 0;
}

int executaOrdem(int socket, mensagem msg){
    switch(msg.tipo){
		case TIPO_CD:
			escravoCD(socket, (char *) msg.dados);
			break;
		case TIPO_LS:
			escravoLS(socket, msg.dados[0]);
			break;
        case TIPO_PUT:
            escravoPUT(socket, (char *) msg.dados);
            break;
        case TIPO_GET:
            escravoGET(socket, (char *) msg.dados);
            break;
        case TIPO_QUIT:
            exit(-1);
	}

	return 0;
}
