#include "mywrapper.h"
#include "frdrwr.h"
#include <time.h>

#define PORTA 3000
#define CLIENT_QUEUE 100

int main(int argc, char *argv[])
{
	int listenfd, connfd, enabled = 1, operazione, reparto,scelta_prestazione,n_prestazioni;
	int n_appuntamenti, scelta_appuntamento, appuntamento_confermato;
	char **prestazioni,n_prestazioni;
	struct appuntamento *appuntamenti;
	struct prenotazione prenotazione;
	pid_t pid;
	struct sockaddr_in my_addr;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	srand((unsigned)time(NULL));
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
	{
		perror("setsockop);
		exit(1);
	}
	AssegnaIndirizzo(listenfd,my_addr);
	Ascolto(listenfd,CLIENT_QUEUE);
	for(;;)
	{
		connfd = AccettazioneNuovaRichiesta(listenfd);
		if((pid = fork()) < 0)
		{
			perror("fork");
			close(connfd);
			exit(1);
		}
		else if(pid > 0) //Padre
			close(listenfd);
		else //Figlio
		{
			close(listenfd);
			reparto = scelta_reparto(connfd);
			while(FullRead(connfd,&operazione,sizeof(int)) > 0);
			switch(operazione)
			{
				case PRENOTA:
					//prestazioni_erogabili(sockreparto,&prestazioni,&n_prestazioni);
					scelta_prestazione = invia_prestazioni_erogabili(connfd,prestazioni,n_prestazioni);
					//ricevi_date_disponibili(sockreparto,&appuntamenti,&n_appuntamenti, prenotazione);
					scelta_appuntamento = scelta_data_orario_disponibile(connfd,appuntamenti,n_appuntamenti);
					//appuntamento_confermato = conferma_appuntamento(sockreparto,prenotazione);
					invia_conferma_data(connfd,appuntamento_confermato);
					while(FullRead(connfd,&prenotazione,sizeof(struct prenotazione)) > 0);
					
			}
			close(connfd);
			exit(0);
		}
	}
	exit(0);
}
