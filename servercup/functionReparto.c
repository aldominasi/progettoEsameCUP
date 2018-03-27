#include "lib.h"

#define PRESTAZIONI_EROGABILI 0 //RICEVE LE PRESTAZIONI EROGABILI DAL REPARTO SELEZIONATO
#define DATE_DISPONIBILI 1 //RICEVE LE DATE PRENOTABILI PER UNA PRESTAZIONE
#define CONFERMA_DATA 2 //RICEVE LA CONFERMA DELLA DATA DAL REPARTO
#define SALVA_PRENOTAZIONE 3 //INVIA AL REPARTO I DATI DELLA PRENOTAZIONE
#define CANCELLA_PRENOTAZIONE 4 //INVIA LA PRENOTAZIONE DA CANCELLARE AL REPARTO
#define INFO_PRENOTAZIONE 5 //RICEVE INFORMAZIONI RIGUARDANTI LA PRENOTAZIONE INVIATA DAL CLIENT
#define LISTA_PRENOTAZIONI_UTENTE 6 //RICEVE LA LISTA DELLE PRENOTAZIONI ATTIVE DELL'UTENTE

void prestazioni_erogabili(int sock, char ***prestazioni, int *count)
{
	int codice_operazione = PRESTAZIONI_EROGABILI,i,length;
	FullWrite(sock,&codice_operazione,sizeof(int));
	while(FullRead(sock,count,sizeof(int)) > 0);
	**prestazioni = (char**)malloc(*count * sizeof(char*));
	for(i=0;i<*count;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		**prestazioni[i] = (char *)malloc(length*sizeof(char));
		while(FullRead(sock,**prestazioni[i],length) > 0);
	}
}

void ricevi_date_disponibili(int sock, struct appuntamento **date,int *count, struct prenotazione prenotazione)
{
	int i,length;
	struct appuntamento appuntamento_ricevuto;
	length = strlen(prenotazione.prestazione)+1;
	FullWrite(sock,&length,sizeof(int));
	Fullwrite(sock,prenotazione.prestazione,length);
	while(FullRead(sock,*count,sizeof(int)) > 0);
	if (*count > 0)
	{
		*date = (struct appuntamento *)malloc(count * sizeof(struct appuntamento));
		for(i=0;i<count;i++)
			while(FullRead(sock,&appuntamento_ricevuto,sizeof(struct appuntamento)) > 0);
	}
}

int conferma_appuntamento(int sock, struct prenotazione prenotazione_da_confermare)
{
	int risposta;
	FullWrite(sock,prenotazione_da_confermare,sizeof(struct prenotazione));
	while(FullRead(sock,&risposta,sizeof(int)) > 0);
	if (risposta == 1)
		return 1;
	else
		return 0;
}

void invia_prenotazione_completa(int sock, struct prenotazione prenotazione)
{
	FullWrite(sock,&prenotazione,sizeof(struct prenotazione));
}
