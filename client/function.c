#include "frdwr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

/*
	La funzione scegli_reparto permette all'utente di scegliere il reparto su cui effettuare 
	un operazione. la lista dei reparti viene ricevuta dal server del cup a cui si affida un codice
*/
void scegli_reparto(int sock, char **reparto)
{
	int i, n, length, codice_scelto=-1, length_reparto_scelto;
	char **reparti;
	while(FullRead(sock,&n,sizeof(int)) > 0); //Legge il numero di reparti gestiti dal cup
	reparti = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0); //riceve la lunghezza dell'array di char che descrive il nome del reparto
		reparti[i] = (char *)malloc(length * sizeof(char)); //alloca memoria per contenere il nome del reparto
		while(FullRead(sock,reparti[i],length) > 0); //inserisce il nome del reparto nell'array
		fprintf(stdout,"codice: %d reparto: %s\n",i+20,reparti[i]); //visualizza i reparti da scegliere mostrando il codice da digitare (la posizione nell'array + 20)
	}
	fprintf(stdout,"Inserire il codice relativo al reparto a cui si intende far riferimento: ");
	while(codice_scelto < 0 || codice_scelto >= (i+20))
		fscanf(stdin,"%d",&codice_scelto); //salva la scelta dell'utente nella variabile codice_scelto
//viene ricavato l'indice del reparto scelto all'interno dell'array e viene inviato al server	
	codice_scelto -= 20;
	FullWrite(sock,&codice_scelto,sizeof(int));
	length_reparto_scelto = ((int)strlen(reparti[codice_scelto])) + 1;
	*reparto =  (char *)malloc(length_reparto_scelto*sizeof(char));
	strcpy(*reparto,reparti[codice_scelto]);
	for(i=0;i<n;i++)
		free(reparti[i]);
	free(reparti);
}

/*
	la funzione scegli_prestazioni_erogabili viene utilizzata quando l'utente
	vuole effettuare una prenotazione di un appuntamento e gestisce la scelta dell'utente
	sulle possibili prestazioni che può prenotare.
*/
void scegli_prestazioni_erogabili(int sock, char **prestazione)
{
	int i, n, length, codice_scelto = 0;
	char **prestazioni;
	while(FullRead(sock,&n,sizeof(int)) > 0); //riceve il numero di prestazioni erogabili dal reparto
	prestazioni = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0); //riceve la lunghezza del nome della prestazione
		prestazioni[i] = (char *)malloc(length*sizeof(char)); //alloca memoria abbastanza grande da contenere la prestazione
		while(FullRead(sock,prestazioni[i],length) > 0); //riceve la prestazione erogabile
		fprintf(stdout,"codice: %d prestazione: %s\n",i+40,prestazioni[i]); //visualizza all'utente la prestazione con l'identificativo numerico (indice della posizione nell'array + 40)
	}
	fprintf(stdout,"Inserire il codice relativo alla prestazione a cui si intende far riferimento: ");
	while(codice_scelto < 40 || codice_scelto >= (40+i))
		fscanf(stdin,"%d",&codice_scelto);
	codice_scelto -= 40; //recupera l'indice della prestazione scelta dall'utente nell'array
	FullWrite(sock,&codice_scelto,sizeof(int)); //invia al server l'indice della prestazione scelta dall'utente
	*prestazione = prestazioni[codice_scelto];
	for(i=0;i<n;i++)
		free(prestazioni[i]);
	free(prestazioni);
}

/*
	Questa funzione viene utilizzata per comunicare al server le operazioni
	scelte durante il processo da parte dell'utente verso il client. E' utilizzata
	soprattutto per inviare i codici delle operazioni identificati dalle LABEL definite nel
	file header lib.h
*/
void invia_operazione(int sock,int operazione)
{
	FullWrite(sock,&operazione,sizeof(int));
}
/*
	La funzione scegli_operazione visualizza all'utente quali operazioni può effettuare verso un reparto
	e gestisce tale scelta. La funzione ritorna come valore l'intero che identifica l'operazione da eseguire
*/
int scegli_operazione(int sock)
{
	int scelta=0;
	while(scelta != PRENOTA && scelta != CANCELLA_PRENOTAZIONE && scelta != INFO_PRENOTAZIONE && scelta != EXIT)
	{
		fprintf(stdout,"Digita il codice dell'operazione che vuoi effettuare\n");
		fprintf(stdout,"PRENOTA (10)\nCANCELLA PRENOTAZIONE (11)\nINFO PRENOTAZIONE (12)\nESCI (-1)\nDigita il codice: ");
		fscanf(stdin,"%d",&scelta);
	}
	return scelta;
}

/*
	La funzione scegli_data_prenotazione permette all'utente, una volta ricevute 
	le date e gli orari degli appuntamenti prenotabili, di scegliere la data e l'ora dell'appuntamento
	che l'utente vuole prenotare.
*/
int scegli_data_prenotazione(int sock,struct appuntamento *dataprenotazione)
{
	int i,length,count,scelta=-1,confermato;
	struct appuntamento *appuntamenti;
	while(FullRead(sock,&count,sizeof(int)) > 0); //riceve il numero di appuntamenti disponibili
	if(count > 0) //se ci sono appuntamenti disponibili
	{
		fprintf(stdout,"Digita il codice relativo all'appuntamento che vuoi prenotare\n");
		appuntamenti = (struct appuntamento *)malloc(count * sizeof(struct appuntamento )); //alloca memoria per contenere la lista degli appuntamenti disponibili
		for(i=0;i<count;i++)
		{
			while(FullRead(sock,&appuntamenti[i],sizeof(struct appuntamento)) > 0); //riceve l'appuntamento
			fprintf(stdout,"data: %s ora: %s codice: %d\n",appuntamenti[i].data,appuntamenti[i].orario,i); //visualizza l'appuntamento con l'indice dello stesso all'interno dell'array
		}
		fprintf(stdout,"Digita il codice: ");
		while(scelta < 0 || scelta >= i)
			fscanf(stdin,"%d",&scelta); //salva la scelta dell'utente
		FullWrite(sock,&scelta,sizeof(int)); //invia al server la scelta dell'utente
		strcpy(dataprenotazione->data,appuntamenti[scelta].data);
		strcpy(dataprenotazione->orario,appuntamenti[scelta].orario);
		while(FullRead(sock,&confermato,sizeof(int)) > 0); //riceve la conferma della prenotazione dal server
		return confermato;
	}
	else
	{
		fprintf(stdout,"Non ci sono date prenotabili per la prestazione che hai scelto\n");
		return -1;
	}
}

/*
	Function utilizzata quando l'utente digita il codice che identifica l'operazione 
	di prenotazione per una visita. La funzione gestisce l'intera fase.
*/
void prenota(int sock, char *reparto)
{
	int conferma = 0, length_ricetta;
	char *prestazione;
	struct prenotazione datiprenotazione;
	struct appuntamento datiappuntamento;
	scegli_prestazioni_erogabili(sock,&prestazione); //chiama la funzione per scelta della prestazione da prenotare
	strcpy(datiprenotazione.prestazione,prestazione);
	while(conferma == 0)
		conferma = scegli_data_prenotazione(sock,&datiappuntamento); //chiama la funzione per la scelta della data relativa alla prentoazione
	if(conferma == 1) //è stata scelta la data
	{
/*
	In questa fase l'utente inserisce ed invia al server: nome e cognome dell'utente e il codice dell'impegnativa
	che è costituita da 15 caratteri
*/
		strcpy(datiprenotazione.data_appuntamento,datiappuntamento.data);
		strcpy(datiprenotazione.orario_appuntamento,datiappuntamento.orario);
		fprintf(stdout,"Digita il nome dell'assistito: ");
		fscanf(stdin,"%s",datiprenotazione.assistito.nome);
		fprintf(stdout,"Digita il cognome dell'assistito: ");
		fscanf(stdin,"%s",datiprenotazione.assistito.cognome);
		do
		{
			fprintf(stdout,"Digita il codice dell'impegnativa: ");
			fscanf(stdin,"%s",datiprenotazione.codice_ricetta);
		} while(strlen(datiprenotazione.codice_ricetta) != (RICETTA - 1));
		FullWrite(sock,&datiprenotazione,sizeof(struct prenotazione)); //Invia i dati della prenotazione al server
		while(FullRead(sock,&datiprenotazione,sizeof(struct prenotazione)) > 0); //riceve i dati definitivi della prenotazione
		fprintf(stdout,"Prenotazione avvenuta con successo\n I dati relativi alla prenotazione sono i seguenti\n");
		//visualizza data e ora dell'appuntamento prenotato e il relativo codice di prenotazione da conservare
		fprintf(stdout,"data appuntamento: %s ora appuntamento: %s codice prenotazione: %s\n",datiprenotazione.data_appuntamento,datiprenotazione.orario_appuntamento,datiprenotazione.codice_prenotazione);
	}
	else
		fprintf(stdout,"La prenotazione non è stata completata\n");
}

/*
	Function utilizzata quando l'utente digita il codice per la cancellazione di un appuntamento prenotato.
	La cancellazione avviene se l'utente possiede ed inserisce il codice prenotazione relativo all'appuntamento
	che intende cancellare.
*/
void cancella_prenotazione(int sock)
{
	char codice_prenotazione[CODICE_PRENOTAZIONE], risposta='c';
	int trovato, operazione = CANCELLA_PRENOTAZIONE, elimina=0, operazione_completata;
	struct prenotazione prenotazione;
	//L'utente inserisce il codice prenotazione relativo all'appuntamento che desidera cancellare
	do
	{
		printf("Inserisci il codice di prenotazione (10 caratteri): ");
		scanf("%s",codice_prenotazione);
	} while(strlen(codice_prenotazione) != 10);
	codice_prenotazione[CODICE_PRENOTAZIONE-1]='\0';
	FullWrite(sock,codice_prenotazione,CODICE_PRENOTAZIONE); //invia al server il codice di prenotazione relativo all'appuntamento
	while(FullRead(sock,&trovato,sizeof(int)) > 0); //riceve dal server se è stato trovato l'appuntamento identificato dal codice prenotazione inviato precedentemente
	if(trovato == 1) //L'appuntamento è stato trovato
	{
		while(FullRead(sock,&prenotazione,sizeof(struct prenotazione)) > 0); //riceve i dati dell'appuntamento
		while(getchar() != '\n');
		//visualizza in dettaglio l'appuntamento e chiede all'utente se è sicuro di voler cancellare l'appuntamento
		do
		{
			fprintf(stdout,"Sei sicuro di voler cancella la seguente prenotazione? (y/n)\n%s %s %s\n",prenotazione.prestazione,prenotazione.data_appuntamento,prenotazione.orario_appuntamento);
			scanf("%c",&risposta);
		} while(risposta != 'y' && risposta != 'n');
		if(risposta == 'y') //l'utente ha confermato la cancellazione dell'appuntamento
		{
			elimina = 1;
			FullWrite(sock,&elimina,sizeof(int));  //invia la risposta al server
			while(FullRead(sock,&operazione_completata,sizeof(int)) > 0); //riceve l'esito della prenotazione
			if(operazione_completata  == 1) //operazione avvenuta
				fprintf(stdout,"La prenotazione è stata cancellata\n");
			else
				fprintf(stdout,"La prenotazione non è stata canncellata\n");
		}
		else //l'utente non ha confermato la cancellazione dell'appuntamento
		{
			FullWrite(sock,&elimina,sizeof(int)); //invia la risposta al server
			fprintf(stdout,"Hai annullato l'operazione\n");
		}
	}
	else //l'appuntamento non è stato trovato
		fprintf(stdout,"La prenotazione relativa al codice di prenotazione non è stata trovata\n");
}

/*
	Function utilizzata quando l'utente digita il codice per ricevere le informazioni relative
	ad un appuntamento prenotato in precedenza. Tale operazione può essere avviata solo se l'utente
	è a conoscenza del codice di prenotazione dell'appuntamento.
*/
void info_prenotazione(int sock)
{
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	int trovato, operazione = INFO_PRENOTAZIONE;
	struct prenotazione prenotazione;
	//l'utente inserisce il codice di prenotazione relativo all'appuntamento
	do
	{
		printf("Inserisci il codice di prenotazione (%d caratteri): ",CODICE_PRENOTAZIONE-1);
		scanf("%s",codice_prenotazione);
	} while(strlen(codice_prenotazione) != CODICE_PRENOTAZIONE - 1);
	codice_prenotazione[CODICE_PRENOTAZIONE-1] = '\0';
	FullWrite(sock,codice_prenotazione,CODICE_PRENOTAZIONE); //invia il codice di prenotazione al server
	while(FullRead(sock,&trovato,sizeof(int)) > 0); //riceve un valore che indica se l'appuntamento è stato trovato
	if(trovato == 1) //appuntamento trovato
	{
		while(FullRead(sock,&prenotazione,sizeof(struct prenotazione)) > 0); //riceve i dati dell'appuntamento
		fprintf(stdout,"%s %s %s %s %s %s\n", prenotazione.codice_prenotazione, prenotazione.data_appuntamento,prenotazione.orario_appuntamento, prenotazione.assistito.nome,prenotazione.assistito.cognome, prenotazione.prestazione);
	}
	else //appuntamento non trovato
		printf("Non è stata trovata alcuna prenotazione\n");
}
