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
	FullWrite(sock,data_prenotazioni,DATA);
}

void ricevi_prenotazioni(int sock, struct prenotazione **lista_prenotazioni, int *count)
{
	int n,i;
	struct prenotazione value;
	while(FullRead(sock,&n,sizeof(int)) > 0);
	*count = n;
	if(n > 0)
	{
		*lista_prenotazioni = (struct prenotazione *)malloc(n*sizeof(struct prenotazione));
		for(i=0;i<n;i++)
		{
			while(FullRead(sock,&value,sizeof(struct prenotazione)) > 0);
			assegna_prenotazione(&(*lista_prenotazioni[i]),value);
		}
		
	}
}

void ordina_per_orario(struct prenotazione **lista_prenotazioni, int n)
{
	struct prenotazione temp;
	int i,j;
	for(i=1;i<n;i++)
	{
		assegna_prenotazione(&temp,*lista_prenotazioni[i]);
		j=i-1;
		while(j>=-1 && lista_prenotazioni[j]->orario_appuntamento > lista_prenotazioni[i]->orario_appuntamento)
		{
			assegna_prenotazione(&(*lista_prenotazioni[j+1]),*lista_prenotazioni[j]);
			j -= 1;
		}
		assegna_prenotazione(&(*lista_prenotazioni[j+1]),temp);
	}
}

void ordina_per_giorno_e_orario(struct prenotazione **lista_prenotazioni, int n)
{
	struct prenotazione temp;
	int i,j;
	for(i=1;i<n;i++)
	{
		assegna_prenotazione(&temp,*lista_prenotazioni[i]);
		j=i-1;
		while(j>=-1 && (confronta_giorno(lista_prenotazioni[j]->orario_appuntamento, lista_prenotazioni[i]->orario_appuntamento) == 2)) 
		{
			assegna_prenotazione(&(*lista_prenotazioni[j+1]),*lista_prenotazioni[j]);
			j -= 1;
		}
		assegna_prenotazione(&(*lista_prenotazioni[j+1]),temp);
	}
}

int confronta_giorno(char *prenotazioneA,char *prenotazioneB)
{
	int giornoA,giornoB,meseA,meseB,annoA,annoB;
	converti_data(&giornoA,&meseA,&annoA,prenotazioneA);
	converti_data(&giornoB,&meseB,&annoB,prenotazioneB);
	if(annoA < annoB)
		return 1;
	else if(annoA > annoB)
		return 2;
	if(meseA < meseB)
		return 1;
	else if(meseA > meseB)
		return 2;
	if(giornoA < giornoB)
		return 1;
	else if(giornoA > giornoB)
		return 2;
	return 0;
}

void converti_data(int *giorno,int *mese, int *anno, char *data)
{
	char day[3],month[3],year[5];
	if(data[0] == '0')
		*giorno = (int)(data[1] - '0');
	else
	{
		memcpy(day,&data[0],2);
		day[2] = '\0';
		*giorno = atoi(day);
	}
	if(data[3] == '0')
		*mese = (int)(data[4] - '0');
	else
	{
		memcpy(month,&data[3],2);
		month[2] = '\0';
		*mese = atoi(month);
	}
	memcpy(year,&data[6],4);
	year[4] = '\0';
	*anno = atoi(year);
}

void visualizza_lista_prenotazioni(struct prenotazione *lista_prenotazioni, int count)
{
	int i;
	fprintf(stdout,"***** LISTA PRENOTAZIONI *****\n");
	for(i=0;i<count;i++)
			fprintf(stdout,"%s %s %s %s %s\n", lista_prenotazioni[i].codice_prenotazione, lista_prenotazioni[i].orario_appuntamento, lista_prenotazioni[i].assistito.nome,lista_prenotazioni[i].assistito.cognome, lista_prenotazioni[i].prestazione);
}

void invia_operazione(int sock, int operazione)
{
	FullWrite(sock,&operazione,sizeof(int));
}

void assegna_prenotazione(struct prenotazione *prenotazioneA,struct prenotazione prenotazioneB)
{
	strcpy(prenotazioneA->assistito.nome, prenotazioneB.assistito.nome);
	strcpy(prenotazioneA->assistito.cognome, prenotazioneB.assistito.cognome);
	strcpy(prenotazioneA->prestazione, prenotazioneB.prestazione);
	strcpy(prenotazioneA->data_appuntamento, prenotazioneB.data_appuntamento);
	strcpy(prenotazioneA->orario_appuntamento, prenotazioneB.orario_appuntamento);
	strcpy(prenotazioneA->codice_ricetta, prenotazioneB.codice_ricetta);
	strcpy(prenotazioneA->codice_prenotazione, prenotazioneB.codice_prenotazione);
}
