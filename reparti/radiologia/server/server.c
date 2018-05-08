#include "mywrapper.h"
#include "disponibilita.h"
#include "frdwr.h"

#define PORTA 3001 //porta del server del reparto
#define CLIENT_QUEUE 100 //numero max di client in coda

int main(int argc, char *argv[])
{
//dichiarazioni delle variabili utilizzate
	char prestazione_scelta[PRESTAZIONE], data[DATA], cod_prenotazione[CODICE_PRENOTAZIONE];
	int listenfd, connfd, codice_comunicazione = 9, length, enabled = 1, confermato;
	struct prenotazione appuntamento_da_confermare;
	pid_t pid;
	struct sockaddr_in my_addr;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0) //imposta il riutilizzo del socket
	{
		perror("setsockopt");
		exit(1);
	}
	AssegnaIndirizzo(listenfd,my_addr);
	Ascolto(listenfd,CLIENT_QUEUE);
	for(;;)
	{
		connfd = AccettazioneNuovaRichiesta(listenfd); //Accetta la richiesta di connessione del client
		if((pid = fork()) < 0) //Crea il processo figlio
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
				while(FullRead(connfd,&codice_comunicazione,sizeof(int)) > 0); //riceve il codice che identifica l'operazione richiesta
				if(codice_comunicazione == PRESTAZIONI_EROGABILI) //Richiesta della lista delle prenotazioni erogabili
					invia_prestazioni_erogabili(connfd); //invia le prestazioni erogabili al richiedente
				//invia al server del cup le date disponibili per una prestazione
				else if(codice_comunicazione == DATE_DISPONIBILI) //Richiesta delle date disponibili in base alla prestazione scelta
				{
					while(FullRead(connfd,&length,sizeof(int)) > 0); //riceve il numero di char della prestazione
					while(FullRead(connfd,prestazione_scelta,length) > 0); //riceve la prestazione
					fprintf(stdout,"prestazione scelta %s\n",prestazione_scelta);
					invia_date_disponibili(connfd,prestazione_scelta); //invia le date disponibili di una prestazione
				}
				else if(codice_comunicazione == LISTA_PRENOTAZIONI) //Richiesta delle prenotazioni di un singolo giorno
				{
					while(FullRead(connfd,&data,DATA) > 0); //Riceve la data di ricerca
					invia_lista_prenotazioni(connfd,data); //ricerca ed invia le prenotazioni del giorno ricevuto in precedenza
				}
				else if(codice_comunicazione == LISTA_TUTTE_PRENOTAZIONI) //Richiesta di tutte le prenotazioni del reparto
				{
					printf("lista di tutte le prenotazioni\n");
					invia_lista_prenotazioni_senza_data(connfd); //invia tutte le prenotazioni del reparto
				}
				else if(codice_comunicazione == CONFERMA_DATA) //Richiesta della conferma della data di prenotazione
				{
					while(FullRead(connfd,&appuntamento_da_confermare,sizeof(struct prenotazione)) > 0); //riceve l'appuntamento da confermare
					confermato = conferma_appuntamento(connfd, appuntamento_da_confermare); //conferma l'appuntamento
					if(confermato == 1) //se l'operazione di conferma è andata a buon fine
						inserisci_prenotazione_in_agenda(connfd); //inserisce la prenotazione nell'agenda del reparto
				}
				else if(codice_comunicazione == INFO_PRENOTAZIONE) //Richiesta delle informazioni di una prenotazione presente nell'agenda del reparto
					informazioni_prenotazione(connfd); //cerca ed invia le informazioni riguardanti una prenotazione
				else if(codice_comunicazione == CANCELLA_PRENOTAZIONE) //Richiesta di cancellazione di una prenotazione
					cancella_prenotazione(connfd); //cerca e cancella la prenotazione ricevuta
			} while(codice_comunicazione != EXIT); //finchè non riceve il codice che identifica la chiusura della comunicazione
			close(connfd);
			exit(0);
		}
	}
	close(listenfd);
	exit(0);
}
