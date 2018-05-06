#include "mywrapper.h" //file header contenente le firme delle funzioni utilizzate per la gestione della connessione verso e dal server
#include "lib.h" //file header che contiene le firme delle funzioni utilizzate dall'applicazione client
#include "frdwr.h" //file header che contiene le firme delle funzioni che gestiscono le fasi di lettura e scrittura sul canale di comunicazione tra il client e il server
#include <stdio.h>

//Porta utilizzata dal server del cup che si occuperà poi di reindirizzare tutte le scelte dell'utente in base al reparto da lui scelto
#define PORTA 3000

int main(int argc, char *argv[])
{
/*
	Dichiarazioni delle variabili utilizzate dal processo per 
	poter gestire le possibili scelte dell'utente
*/
	int sockfd, count, codice_operazione;
	struct prenotazione *prenotazioni;
	struct sockaddr_in servaddr;
	char *addr = "127.0.0.1"; //localhost
	char data_prenotazioni[DATA], scelta;
	char *reparto;
	//Operazioni che gestiscono la connessione verso il server del cup
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
	scegli_reparto(sockfd,&reparto); //function che prevede la scelta del reparto da parte dell'utente
	codice_operazione = scegli_operazione(sockfd); //function che permette all'utente di scegliere quale operazione vuole effettuare verso il reparto precedentemente scelto
	invia_operazione(sockfd,codice_operazione); //Dopo che l'utente ha scelto quale operazione effettuare, questa viene comunicata al server del cup
//lo switch seguente gestisce singolarmente le possibili operazioni che l'utente può scegliere
	switch(codice_operazione)
	{
		case PRENOTA: //OPERAZIONE DI PRENOTAZIONE
			prenota(sockfd,reparto); //La function prenota gestisce tutta la fase di prenotazione lato client
			invia_operazione(sockfd, EXIT); //invia la comunicazione al server di chiudere la connessione
			close(sockfd); //chiude la connessione col server cup
			exit(1);
			break;
		case CANCELLA_PRENOTAZIONE:
			cancella_prenotazione(sockfd); //la function cancella_prenotazione gestisce tutta la fase di cancellazione lato client
			invia_operazione(sockfd, EXIT);//invia la comunicazione al server di chiudere la connessione
			close(sockfd); //chiude la connessione col server cup
			exit(1);
			break;
		case INFO_PRENOTAZIONE:
			info_prenotazione(sockfd); //la function info_prenotazione gestisce tutta la fase di ricezione delle informazioni riguardanti l'appuntamento lato client
			invia_operazione(sockfd, EXIT);//invia la comunicazione al server di chiudere la connessione
			close(sockfd); //chiude la connessione col server cup
			exit(1);
			break;
		case EXIT:
			invia_operazione(sockfd,EXIT); //invia la comunicazione al server di chiudere la connessione
			close(sockfd); //chiude la connessione col server cup
			exit(1);
			break;
	}
}
