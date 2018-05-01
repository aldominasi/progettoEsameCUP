#include "mywrapper.h"
#include "disponibilita.h"
#include "frdwr.h"

#define PORTA 3001
#define CLIENT_QUEUE 100

int main(int argc, char *argv[])
{
	char prestazione_scelta[PRESTAZIONE], data[DATA], cod_prenotazione[CODICE_PRENOTAZIONE];
	int listenfd, connfd, codice_comunicazione = 9, length, enabled = 1, confermato;
	struct prenotazione appuntamento_da_confermare;
	pid_t pid;
	struct sockaddr_in my_addr;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
	{
		perror("setsockopt");
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
			close(connfd);
		else //figlio
		{
			close(listenfd);
			do
			{
				while(FullRead(connfd,&codice_comunicazione,sizeof(int)) > 0);
				//scelta dell'operazione in base al codice_comunicazione
				//invia al server del cup la lista delle prestazioni erogabili
				if(codice_comunicazione == PRESTAZIONI_EROGABILI)
					invia_prestazioni_erogabili(connfd);
				//invia al server del cup le date disponibili per una prestazione
				else if(codice_comunicazione == DATE_DISPONIBILI)
				{
					while(FullRead(connfd,&length,sizeof(int)) > 0);
					while(FullRead(connfd,prestazione_scelta,length) > 0);
					fprintf(stdout,"prestazione scelta %s\n",prestazione_scelta);
					invia_date_disponibili(connfd,prestazione_scelta);
				}
				else if(codice_comunicazione == LISTA_PRENOTAZIONI)
				{
					while(FullRead(connfd,&data,DATA) > 0);
					invia_lista_prenotazioni(connfd,data);
				}
				else if(codice_comunicazione == LISTA_TUTTE_PRENOTAZIONI)
				{
					printf("lista di tutte le prenotazioni\n");
					invia_lista_prenotazioni_senza_data(connfd);
				}
				else if(codice_comunicazione == CONFERMA_DATA)
				{
					while(FullRead(connfd,&appuntamento_da_confermare,sizeof(struct prenotazione)) > 0);
					confermato = conferma_appuntamento(connfd, appuntamento_da_confermare);
					if(confermato == 1)
						inserisci_prenotazione_in_agenda(connfd);
				}
				else if(codice_comunicazione == INFO_PRENOTAZIONE)
					informazioni_prenotazione(connfd);
				else if(codice_comunicazione == CANCELLA_PRENOTAZIONE)
					cancella_prenotazione(connfd);
			} while(codice_comunicazione != EXIT);
			printf("EXIT\n");
			close(connfd);
			exit(0);
		}
	}
	close(listenfd);
	exit(0);
}
