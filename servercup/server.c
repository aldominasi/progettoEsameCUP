#include "mywrapper.h"
#include "frdwr.h"
#include "lib.h"
#include <time.h>
#include <string.h>

#define PORTA 3000
#define CLIENT_QUEUE 100

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
		connfd = AccettazioneNuovaRichiesta(listenfd);
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
			reparto = scelta_reparto(connfd);
			sockreparto = Socket(AF_INET,SOCK_STREAM,0);
			ImpostaIndirizzoClient(AF_INET,"127.0.0.1",reparto,&addr_reparto);
			Connessione(sockreparto,addr_reparto);
			srand((unsigned)time(NULL));
			do
			{
				while(FullRead(connfd,&operazione,sizeof(int)) > 0);
				switch(operazione)
				{
					case PRENOTA:
						prestazioni_erogabili(sockreparto,&prestazioni,&n_prestazioni);
						scelta_prestazione = invia_prestazioni_erogabili(connfd,prestazioni,n_prestazioni);
						snprintf(prenotazione.prestazione,sizeof(prenotazione.prestazione),"%s",prestazioni[scelta_prestazione]);
						ricevi_date_disponibili(sockreparto,&appuntamenti,&n_appuntamenti, prenotazione);
						if(n_appuntamenti > 0)
						{
							scelta_appuntamento = scelta_data_orario_disponibile(connfd,appuntamenti,n_appuntamenti);
							strcpy(prenotazione.data_appuntamento,appuntamenti[scelta_appuntamento].data);
							strcpy(prenotazione.orario_appuntamento,appuntamenti[scelta_appuntamento].orario);
							appuntamento_confermato = conferma_appuntamento(sockreparto,prenotazione);
							invia_conferma_data(connfd,appuntamento_confermato);
							while(FullRead(connfd,&dati_prenotazione_client,sizeof(struct prenotazione)) > 0);
							for(i=0;i<CODICE_PRENOTAZIONE;i+=2)
								prenotazione.codice_prenotazione[i] = (char)((rand()%10)+48);
							for(i=1;i<CODICE_PRENOTAZIONE;i+=2)
								prenotazione.codice_prenotazione[i] = (char)((rand()%26)+65);
							prenotazione.codice_prenotazione[CODICE_PRENOTAZIONE-1] = '\0';
							snprintf(prenotazione.assistito.nome,sizeof(prenotazione.assistito.nome),"%s",dati_prenotazione_client.assistito.nome);
							snprintf(prenotazione.assistito.cognome,sizeof(prenotazione.assistito.cognome),"%s",dati_prenotazione_client.assistito.cognome);
							snprintf(prenotazione.codice_ricetta,sizeof(prenotazione.codice_ricetta),"%s",dati_prenotazione_client.codice_ricetta);
							FullWrite(connfd,&prenotazione,sizeof(struct prenotazione));
							FullWrite(sockreparto,&prenotazione,sizeof(struct prenotazione));
						}
						else
							FullWrite(connfd,&n_appuntamenti,sizeof(int));
						break;
					case INFO_PRENOTAZIONE_UTENTE:
						while(FullRead(connfd,codice_prenotazione,CODICE_PRENOTAZIONE) > 0);
						op = INFO_PRENOTAZIONE_REPARTO;
						FullWrite(sockreparto,&op,sizeof(int));
						ricevi_info_prenotazione(sockreparto, codice_prenotazione, &prenotazione, &trovato);
						FullWrite(connfd,&trovato,sizeof(int));
						if(trovato == 1)
							FullWrite(connfd,&prenotazione,sizeof(struct prenotazione));
						break;
					case CANCELLA_PRENOTAZIONE_UTENTE:
						while(FullRead(connfd,codice_prenotazione,CODICE_PRENOTAZIONE) > 0);
						op = INFO_PRENOTAZIONE_REPARTO;
						FullWrite(sockreparto,&op,sizeof(int));
						ricevi_info_prenotazione(sockreparto, codice_prenotazione, &prenotazione,&trovato);
						FullWrite(connfd,&trovato,sizeof(int));
						if(trovato == 1)
						{
							FullWrite(connfd,&prenotazione,sizeof(struct prenotazione));
							while(FullRead(connfd,&conferma_cancellazione_utente,sizeof(int)) > 0);
							if(conferma_cancellazione_utente == 1)
							{
								op = CANCELLA_PRENOTAZIONE_REPARTO;
								FullWrite(sockreparto,&op,sizeof(int));
								conferma_cancellazione_reparto = cancella_prenotazione(sockreparto,prenotazione);
								FullWrite(connfd,&conferma_cancellazione_reparto,sizeof(int));
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
