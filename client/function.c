#include "frdwr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

void scegli_reparto(int sock)
{
	int i, n, length, codice_scelto;
	char **reparti;
	while(FullRead(sock,&n,sizeof(int)) > 0);
	reparti = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		reparti[i] = (char *)malloc(length * sizeof(char));
		while(FullRead(sock,&reparti[i],length) > 0);
		fprintf(stdout,"codice: %d reparto: %s\n",i+20,reparti[i]);
	}
	fprintf(stdout,"Inserire il codice relativo al reparto a cui si intende far riferimento: ");
	fscanf(stdin, "%d",&codice_scelto);
	FullWrite(sock,&codice_scelto,sizeof(int));
}

void scegli_prestazioni_erogabili(int sock)
{
	int i, n, length, codice_scelto;
	char **prestazioni;
	while(FullRead(sock,&n,sizeof(int)) > 0);
	prestazioni = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		prestazioni[i] = (char *)malloc(length*sizeof(char));
		while(FullRead(sock,&prestazioni[i],length) > 0);
		fprintf(stdout,"codice: %d reparto: %s\n",i+40,prestazioni[i]);
	}
	fprintf(stdout,"Inserire il codice relativo alla prestazione a cui si intende far riferimento: ");
	fscanf(stdin,"%d",&codice_scelto);
	FullWrite(sock,&codice_scelto,sizeof(int));
}

void invia_operazione(int sock,int operazione)
{
	FullWrite(sock,&operazione,sizeof(int));
}

int scegli_operazione(int sock)
{
	int scelta=0;
	while(scelta != PRENOTA && scelta != CANCELLA_PRENOTAZIONE && scelta != INFO_PRENOTAZIONE && scelta != LISTA_PRENOTAZIONI_UTENTE && scelta != EXIT)
	{
		fprintf(stdout,"Digita il codice dell'operazione che vuoi effettuare\n");
		fprintf(stdout,"PRENOTA (10)\nCANCELLA PRENOTAZIONE (11)\nINFO PRENOTAZIONE (12)\nLISTA PRENOTAZIONI (13)\nESCI (-1)\nDigita il codice: ");
		fscanf(stdin,"%d",&scelta);
	}
	return scelta;
}

int scegli_data_prenotazione(int sock)
{
	int i,length,count,scelta=-1;
	char **date_prenotazioni;
	while(FullRead(sock,&count,sizeof(int)) > 0);
	if(count > 0)
	{
		fprintf(stdout,"Digita il codice della data che vuoi prenotare\n");
		date_prenotazioni = (char **)malloc(count * sizeof(char));
		for(i=0;i<count;i++)
		{
			while(FullRead(sock,&length,sizeof(int)) > 0);
			date_prenotazioni[i] = (char *)malloc(length * sizeof(char));
			while(FullRead(sock,&date_prenotazioni[i],length) > 0);
			fprintf(stdout,"%s codice: %d\n",date_prenotazioni[i],i);
		}
		fprintf(stdout,"Digita il codice: ");
		while(scelta < 0 && scelta > i)
			fscanf(stdin,"%d",&scelta);
		FullWrite(sock,&scelta,sizeof(int));
		return 0;
	}
	else
	{
		fprintf(stdout,"Non ci sono date prenotabili per la prestazione che hai scelto\n");
		return -1;
	}
}

//Function utilizzata quando l'utente digita il codice per la prenotazione di una visita
void prenota(int sock)
{
	scegli_prestazioni_erogabili(sock);
	if(scegli_data_prenotazione(sock) > -1) //è possibile continuare la prenotazione
	{
		
	}
}

//Function utilizzata quando l'utente digita il codice per la cancellazione di una visita
void cancella_prenotazione(int sock)
{

}

//Function utilizzata quando l'utente digita il codice per le informazioni di una visita
void info_prenotazione(int sock)
{

}

//Function utilizzata quando l'utente digita il codice per la lista delle prenotazioni
void lista_prenotazioni(int sock)
{

}
