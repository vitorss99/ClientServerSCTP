/*
Para compilar:
Precisa da bibliteca libsctp-dev
gcc sctpserver.c -o server -lsctp
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <netinet/sctp.h> // Novos includes
#include <sys/types.h>

#define ECHOMAX 1024


int main(int argc, char *argv[]) {

	/* Checagem de parametros. */
	if (argc != 2) {
		printf("Parametros: <local_port> \n");
		exit(1);
	}

	/* Variaveis Locais */
	int loc_sockfd, loc_newsockfd, tamanho;
	char linha[ECHOMAX];

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	struct sockaddr_in loc_addr = {
		.sin_family = AF_INET, /* familia do protocolo */
		.sin_addr.s_addr = INADDR_ANY, /* endereco IP local */
		.sin_port = htons(atoi(argv[1])), /* porta local */
		//.sin_zero = 0, /* por algum motivo pode se botar isso em 0 usando memset() */
	};

	/* Novas Estrutura: sctp_initmsg, comtém informações para a inicialização de associação*/
	struct sctp_initmsg initmsg = {
		.sinit_num_ostreams = 5, /* Número de streams que se deseja mandar. */
  		.sinit_max_instreams = 5, /* Número máximo de streams se deseja receber. */
  		.sinit_max_attempts = 4, /* Número de tentativas até remandar INIT. */
  		/*.sinit_max_init_timeo = 60000, Tempo máximo em milissegundos para mandar INIT antes de abortar. Default 60 segundos.*/
	};

	/* Usado para definir quais eventos se deseja verificar. Se recebe usando a estrutura sctp_sndrcvinfo.
	Não foi usado pois acho que é complicado. 
	struct sctp_event_subscribe eventos = {};
	*/


   	/* Cria o socket para enviar e receber datagramas */
	/* parametros(familia, tipo, protocolo) */
	loc_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP); // Mudança protocolo '0' => 'IPPROTO_SCTP'
	
	if (loc_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

   	/* Bind para o endereco local*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}
	
	/* SCTP necessita usar setsockopt, */
	/* parametros(descritor socket, protocolo, tipo de opção(INIT, EVENTS, etc.), opções, tamanho das opções) */
  	if (setsockopt (loc_sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof (initmsg)) < 0){
		perror("setsockopt(initmsg)");
		exit(1);
  	}

	/* parametros(descritor socket,	numeros de conexoes em espera sem serem aceites pelo accept)*/
	listen(loc_sockfd, initmsg.sinit_max_instreams); // Mudado para initmsg.sinit_max_instreams.
	printf("> aguardando conexao\n");

   	/* Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco)*/
	tamanho = sizeof(struct sockaddr_in);
    loc_newsockfd = accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);

	do  {
		sctp_recvmsg(loc_newsockfd, &linha, sizeof(linha), NULL, 0, 0, 0);

        printf("Comando recebido: %s", linha);
        system(linha);

	}while(strcmp(linha,"exit"));
	/* fechamento do socket local */ 
	close(loc_sockfd);
	close(loc_newsockfd);
}

