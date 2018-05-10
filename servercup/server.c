#include "mywrapper.h"
#include "frdwr.h"
#include "lib.h"
#include <time.h>
#include <string.h>

#define PORTA 3000 //porta su cui il server è in ascolto
#define CLIENT_QUEUE 100 //numero max di client in ascolto

int main(int argc, char *argv[])
{
	int listenfd, connfd, enabled = 1, operazione, reparto,scelta_prestazione,n_prestazioni, sockreparto, i, trovato, op;
	int n_appuntamenti, scelta_appuntamento, appuntamento_confermato, conferma_cancellazione_reparto, conferma_cancellazione_utente;
	char **prestazioni, codice_prenotazione[CODICE_PRENOTAZIONE];
	struct appuntamento *appuntamenti;
	struct prenotazione prenotazione, dati_prenotazione_client;
	pid_t pid;
	struct sockaddr_in my_addr, addr_reparto;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
	{
		perror("setsockop");
		exit(1);
	}
	AssegnaIndirizzo(listenfd,my_addr);
	Ascolto(listenfd,CLIENT_QUEUE);
	for(;;)
	{
		connfd = AccettazioneNuovaRichiesta(listenfd); //Accetta nuove richieste di connessione
		if((pid = fork()) < 0)
		{
			perror("fork");
			close(connfd);
			exit(1);
		}
		else if(pid > 0) //Padre
			close(connfd);
		else //Figlio
		{
			close(listenfd);
			reparto = scelta_reparto(connfd); //invia al client i reparti disponibili
		//Imposta la connessione verso il reparto scelto
			sockreparto = Socket(AF_INET,SOCK_STREAM,0);
			ImpostaIndirizzoClient(AF_INET,"127.0.0.1",reparto,&addr_reparto);
			Connessione(sockreparto,addr_reparto);
			srand((unsigned)time(NULL)); //imposta il seed per l'utilizzo della function rand per la generazione del codice di una prenotazione
			do
			{
				while(FullRead(connfd,&operazione,sizeof(int)) > 0); //riceve il codice che identifica l'operazione che si deve eseguire
				switch(operazione)
				{
					case PRENOTA: //prenotazione di un appuntamento
						prestazioni_erogabili(sockreparto,&prestazioni,&n_prestazioni); //riceve dal reparto le prestazioni disponibili
						scelta_prestazione = invia_prestazioni_erogabili(connfd,prestazioni,n_prestazioni); //invia le prestazioni disponibili e attende la risposta
						snprintf(prenotazione.prestazione,sizeof(prenotazione.prestazione),"%s",prestazioni[scelta_prestazione]);
						ricevi_date_disponibili(sockreparto,&appuntamenti,&n_appuntamenti, prenotazione); //riceve le date disponibili dal reparto per la prestazione scelta
						if(n_appuntamenti > 0) //se ci sono date disponibili
						{
							scelta_appuntamento = scelta_data_orario_disponibile(connfd,appuntamenti,n_appuntamenti); //invia le date disponibili al client e attende la risposta
							strcpy(prenotazione.data_appuntamento,appuntamenti[scelta_appuntamento].data);
							strcpy(prenotazione.orario_appuntamento,appuntamenti[scelta_appuntamento].orario);
							appuntamento_confermato = conferma_appuntamento(sockreparto,prenotazione); //attende che il reparto confermi la prenotazione
							FullWrite(connfd,&appuntamento_confermato,sizeof(int)); //notifica al client la conferma della prenotazione
							if(appuntamento_confermato == 1)
							{
								while(FullRead(connfd,&dati_prenotazione_client,sizeof(struct prenotazione)) > 0); //riceve informazioni dell'assistito
							//genera il codice della prenotazione
								for(i=0;i<CODICE_PRENOTAZIONE;i+=2)
									prenotazione.codice_prenotazione[i] = (char)((rand()%10)+48);
								for(i=1;i<CODICE_PRENOTAZIONE;i+=2)
									prenotazione.codice_prenotazione[i] = (char)((rand()%26)+65);
								prenotazione.codice_prenotazione[CODICE_PRENOTAZIONE-1] = '\0';
								snprintf(prenotazione.assistito.nome,sizeof(prenotazione.assistito.nome),"%s",dati_prenotazione_client.assistito.nome);
								snprintf(prenotazione.assistito.cognome,sizeof(prenotazione.assistito.cognome),"%s",dati_prenotazione_client.assistito.cognome);
								snprintf(prenotazione.codice_ricetta,sizeof(prenotazione.codice_ricetta),"%s",dati_prenotazione_client.codice_ricetta);
								FullWrite(connfd,&prenotazione,sizeof(struct prenotazione)); //invia i dati della prenotazione con il codice prenotazione al reparto
								FullWrite(sockreparto,&prenotazione,sizeof(struct prenotazione)); //invia i dati della prenotazione con il codice prenotazione al client
							}
						}
						else  //se non ci sono date disponibili
							FullWrite(connfd,&n_appuntamenti,sizeof(int));
						break;
					case INFO_PRENOTAZIONE_UTENTE: //invia al client le info di una prenotazione identificata dal codice prenotazione ricevuto
						while(FullRead(connfd,codice_prenotazione,CODICE_PRENOTAZIONE) > 0); //riceve il codice della prenotazione
						op = INFO_PRENOTAZIONE_REPARTO;
						FullWrite(sockreparto,&op,sizeof(int)); //richiede al reparto di eseguire l'operazione di info prenotazione
						ricevi_info_prenotazione(sockreparto, codice_prenotazione, &prenotazione, &trovato); //riceve l'info della prenotazione
						FullWrite(connfd,&trovato,sizeof(int)); //invia l'esito dell'operazione
						if(trovato == 1) //se la prenotazione è stata trovata
							FullWrite(connfd,&prenotazione,sizeof(struct prenotazione)); //invia le info della prenotazione al client
						break;
					case CANCELLA_PRENOTAZIONE_UTENTE: //cancella la prenotazione su richiesta del client
						while(FullRead(connfd,codice_prenotazione,CODICE_PRENOTAZIONE) > 0); //riceve il codice della prenotazione
						op = INFO_PRENOTAZIONE_REPARTO;
						FullWrite(sockreparto,&op,sizeof(int));
						ricevi_info_prenotazione(sockreparto, codice_prenotazione, &prenotazione,&trovato);
						FullWrite(connfd,&trovato,sizeof(int));
						if(trovato == 1) //Se la prenotazione è stata trovata
						{
							FullWrite(connfd,&prenotazione,sizeof(struct prenotazione)); //invia la prenotazione al client
							while(FullRead(connfd,&conferma_cancellazione_utente,sizeof(int)) > 0); //attende di sapere se effettivamente l'utente ha intenzione di proseguire con l'operazione
							if(conferma_cancellazione_utente == 1) //l'utente ha confermato di voler proseguire con la cancellazione
							{
								op = CANCELLA_PRENOTAZIONE_REPARTO;
								FullWrite(sockreparto,&op,sizeof(int)); //richiede al reparto di effettuare la cancellazione
								conferma_cancellazione_reparto = cancella_prenotazione(sockreparto,prenotazione); //invia il codice della prenotazione ed attende la conferma dal reparto
								FullWrite(connfd,&conferma_cancellazione_reparto,sizeof(int)); //invia al client l'esito dell'operazione
							}
						}
						break;
				}
			} while(operazione != EXIT);
			FullWrite(sockreparto,&operazione,sizeof(int));
			printf("EXIT\n");
			close(sockreparto);
			close(connfd);
			exit(0);
		}
	}
	exit(0);
}
