/*
Para compilar:
Precisa da bibliteca libsctp-dev
gcc sctpclient.c -o client -lsctp
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <netinet/sctp.h> // Novo include

#define ECHOMAX 1024

int main(int argc, char *argv[]) {

	/* Checagem de parametros. Aborta se número de parametros está errado ou se porta é um string invalido. */
	if (argc != 3 || atoi(argv[2]) == 0) {
		printf("Parametros:<remote_host> <remote_port> \n");
		exit(1);
	}

	/* Variáveis Locais */
	int rem_sockfd;
	char linha[ECHOMAX];

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	struct sockaddr_in rem_addr = {
		.sin_family = AF_INET, /* familia do protocolo*/
		.sin_addr.s_addr = inet_addr(argv[1]), /* endereco IP do servidor */
		.sin_port = htons(atoi(argv[2])), /* porta local  */
		//sin_zero = 0, /* por algum motivo pode se botar isso em 0 usando memset() */
	};


   	/* Cria o socket para enviar e receber fluxos */
	/* parametros(familia, tipo, protocolo) */
	rem_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if (rem_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	printf("> Conectando no servidor '%s:%s'\n", argv[1], argv[2]);

   	/* Estabelece uma conexao remota */
	/* parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) */
	if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("Conectando stream socket");
		exit(1);
	}

	do  {
		//gets(linha);
		fgets (linha, ECHOMAX, stdin);
    	linha[strcspn(linha, "\n")] = 0;

        printf("______________________________________\n");
        printf("Comando executado localmente: %s\n", linha);
		sctp_sendmsg(rem_sockfd, &linha, sizeof(linha), NULL, 0, 0, 0, 0, 0, 0);
        system(linha);
        printf("_______________________________________\n");

		sctp_recvmsg(rem_sockfd, &linha, sizeof(linha), NULL, 0, 0, 0);

	}while(strcmp(linha,"exit"));
	/* fechamento do socket remota */ 	
	close(rem_sockfd);
}