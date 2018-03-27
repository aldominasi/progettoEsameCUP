#include "frdwr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

void scegli_reparto(int sock, char **reparto)
{
	int i, n, length, codice_scelto, length_reparto_scelto;
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
	while(codice_scelto < 0 && codice_scelto >= i)
		fscanf(stdin,"%d",&codice_scelto);
	FullWrite(sock,&codice_scelto,sizeof(int));
	length_reparto_scelto = ((int)strlen(reparti[codice_scelto])) + 1;
	*reparto =  (char *)malloc(length_reparto_scelto*sizeof(char));
	strcpy(*reparto,reparti[codice_scelto]);
	for(i=0;i<n;i++)
		free(reparti[i]);
	free(reparti);
}

void scegli_prestazioni_erogabili(int sock, char **prestazione)
{
	int i, n, length, codice_scelto = 0;
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
	while(codice_scelto < 0 && codice_scelto >= i)
		fscanf(stdin,"%d",&codice_scelto);
	FullWrite(sock,&codice_scelto,sizeof(int));
	*prestazione = (char *)malloc(strlen(prestazioni[codice_scelto])*sizeof(char));
	strcpy(*prestazione,prestazioni[codice_scelto]);
	for(i=0;i<n;i++)
		free(prestazioni[i]);
	free(prestazioni);
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

int scegli_data_prenotazione(int sock, char **dataprenotazione, char **orarioprenotazione)
{
	int i,length,count,scelta=-1,confermato;
	struct appuntamento *appuntamenti;
	while(FullRead(sock,&count,sizeof(int)) > 0);
	if(count > 0)
	{
		fprintf(stdout,"Digita il codice relativo all'appuntamento che vuoi prenotare\n");
		appuntamenti = (struct appuntamento *)malloc(count * sizeof(struct appuntamento ));
		for(i=0;i<count;i++)
		{
			while(FullRead(sock,&appuntamenti[i],sizeof(struct appuntamento)) > 0);
			fprintf(stdout,"data: %s ora: %s codice: %d\n",appuntamenti[i].data,appuntamenti[i].orario,i);
		}
		fprintf(stdout,"Digita il codice: ");
		while(scelta < 0 && scelta >= i)
			fscanf(stdin,"%d",&scelta);
		FullWrite(sock,&scelta,sizeof(int));
		strcpy(*dataprenotazione,appuntamenti[scelta].data);
		strcpy(*orarioprenotazione,appuntamenti[scelta].orario);
		while(FullRead(sock,&confermato,sizeof(int)) > 0);
		return confermato;
	}
	else
	{
		fprintf(stdout,"Non ci sono date prenotabili per la prestazione che hai scelto\n");
		return -1;
	}
}

//Function utilizzata quando l'utente digita il codice per la prenotazione di una visita
void prenota(int sock, char *reparto)
{
	int conferma = 0, length_ricetta;
	char *data_prenotazione, *orario_prenotazione, *prestazione;
	struct prenotazione datiprenotazione;
	scegli_prestazioni_erogabili(sock,&prestazione);
	strcpy(datiprenotazione.prestazione,prestazione);
	while(conferma == 0)
	{
		if(scegli_data_prenotazione(sock,&data_prenotazione,&orario_prenotazione) > -1) //è possibile continuare la prenotazione
			while(FullRead(sock,&conferma,sizeof(int)) > 0);
		else
			conferma = -1;
	}
	if(conferma != -1)
	{
		strcpy(datiprenotazione.data_appuntamento,data_prenotazione);
		strcpy(datiprenotazione.orario_appuntamento,orario_prenotazione);
		fprintf(stdout,"Digita il nome dell'assistito: ");
		fscanf(stdin,"%s",datiprenotazione.assistito.nome);
		fprintf(stdout,"Digita il cognome dell'assistito: ");
		fscanf(stdin,"%s",datiprenotazione.assistito.cognome);
		do
		{
			fprintf(stdout,"Digita il codice dell'impegnativa: ");
			fscanf(stdin,"%s",datiprenotazione.codice_ricetta);
		} while(strlen(datiprenotazione.codice_ricetta) == (RICETTA - 1));
		FullWrite(sock,&datiprenotazione,sizeof(struct prenotazione));
		while(FullRead(sock,&datiprenotazione,sizeof(struct prenotazione)) > 0);
		fprintf(stdout,"Prenotazione avvenuta con successo\n");
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