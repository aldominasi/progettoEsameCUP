#include "frdwr.h"
#include "prenotazioni.h"
#include <stdio.h>
#include <string.h>

void invia_data(int sock)
{
	char data_prenotazioni[DATA];
	fprintf(stdout,"Inserisci la data per la ricerca di prenotazioni (DD/MM/YYYY): ");
	fscanf(stdin,"%s",data_prenotazioni);
	data_prenotazioni[10] = '\0';
	FullWrite(sockfd,data_prenotazioni,DATA);
}

void ricevi_prenotazioni(int sock, struct prenotazioni **lista_prenotazioni, int *count)
{
	int n,i;
	while(FullRead(sockfd,&n,sizeof(int)) > 0);
	*count = n;
	if(n > 0)
	{
		*lista_prenotazioni = (struct prenotazioni *)malloc(n*sizeof(struct prenotazioni));
		for(i=0;i<n;i++)
			while(FullRead(sock,&lista_prenotazioni[i],sizeof(struct prenotazioni)) > 0);
		
	}
}

void ordina_per_orario(struct prenotazioni **lista_prenostazioni, int n)
{

}

void ordina_per_giorno_e_orario(struct prenotazioni **lista_prenotazioni, int n)
{

}

void visualizza_lista_prenotazioni(struct prenotazioni *lista_prenotazioni, int count)
{
	for(i=0;i<count;i++)
			fprintf(stdout,"%s %s %s %s\n", lista_prenotazioni[i].codice_prenotazione, lista_prenotazioni[i].orario_appuntamento, lista_prenotazioni[i].assistito.nome,lista_prenotazioni[i].assistito.cognome);
}

void invia_operazione(int sock, int operazione)
{
	FullWrite(sock,&operazione,sizeof(int));
}
