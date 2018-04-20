#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#define PRESTAZIONE 51 //MAX CHAR PER DESCRIVERE LA PRESTAZIONE (50)
#define DATA 11 //MAX CHAR DATA (10)
#define PRENOTA 10 //INVIA I DATI PER LA PRENOTAZIONE AL CUP
#define CANCELLA_PRENOTAZIONE 11 //CANCELLA LA PRENOTAZIONE INVIATA AL CUP
#define INFO_PRENOTAZIONE 12 //RICEVE INFORMAZIONI RIGUARDANTI LA PRENOTAZIONE INVIATA AL CUP
#define LISTA_PRENOTAZIONI_UTENTE 13 //RICEVE LA LISTA DELLE PRENOTAZIONI ATTIVE
#define LISTA_REPARTI 14 //RICEVE LA LISTA DEI REPARTI
#define EXIT -1 //INVIA AL CUP CHE L'APPLICAZIONE HA TERMINATO LE OPERAZIONI DA ESEGUIRE
#define NOME 21 //MAX CHAR NOME (20)
#define COGNOME 31 //MAX CHAR COGNOME (30)
#define RICETTA 16 //MAX CHAR CODICE_RICETTA (15)
#define CODICE_PRENOTAZIONE 11 //MAX CHAR CODICE_PRENOTAZIONE (5)
#define ORARIO 3 //MAX CHAR ORARIO APPUNTAMENTO/DISPONIBILITA (2)

struct assistito
{
	char nome[NOME];
	char cognome[COGNOME];
};

struct prenotazione
{
	struct assistito assistito;
	char prestazione[PRESTAZIONE];
	char data_appuntamento[DATA];
	char orario_appuntamento[ORARIO];
	char codice_ricetta[RICETTA];
	char codice_prenotazione[CODICE_PRENOTAZIONE];
};

struct appuntamento
{
	char data[DATA];
	char orario[ORARIO];
};

void scegli_reparto(int, char **);
void scegli_prestazioni_erogabili(int , char **);
void invia_operazione(int ,int );
int scegli_operazione(int);
void prenota(int , char *);
void cancella_prenotazione(int );
void info_prenotazione(int );
void lista_prenotazioni(int );
int scegli_data_prenotazione(int , struct appuntamento *);
