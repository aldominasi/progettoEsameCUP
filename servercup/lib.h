#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

//LABEL DI COMUNICAZIONE CON IL CLIENT
#define PRENOTA 10 //INVIA I DATI PER LA PRENOTAZIONE AL CUP
#define CANCELLA_PRENOTAZIONE_UTENTE 11 //CANCELLA LA PRENOTAZIONE INVIATA AL CUP
#define INFO_PRENOTAZIONE_UTENTE 12 //RICEVE INFORMAZIONI RIGUARDANTI LA PRENOTAZIONE INVIATA AL CUP
#define LISTA_PRENOTAZIONI_UTENTE 13 //RICEVE LA LISTA DELLE PRENOTAZIONI ATTIVE

//LABEL DI COMUNICAZIONE CON IL SERVER DEL REPARTO
#define PRESTAZIONI_EROGABILI 0 //INVIA AL CUP LE PRESTAZIONI EROGABILI
#define DATE_DISPONIBILI 1 //INVIA AL CUP LE DATE PRENOTABILI PER UNA PRESTAZIONE
#define CONFERMA_DATA 2 //CONTROLLA E NEL CASO CONFERMA LA DATA AL CUP
#define SALVA_PRENOTAZIONE 3 //RICEVE DAL CUP I DATI DELLA PRENOTAZIONE
#define CANCELLA_PRENOTAZIONE_REPARTO 4 //CANCELLA LA PRENOTAZIONE INVIATA DAL CUP
#define INFO_PRENOTAZIONE_REPARTO 5 //INVIA INFORMAZIONI RIGUARDANTI LA PRENOTAZIONE INVIATA DAL CUP
#define LISTA_PRENOTAZIONI_REPARTO 6 //INVIA LA LISTA DELLE PRENOTAZIONI ATTIVE DELL'UTENTE INVIATO DAL CUP

#define PRESTAZIONE 51 //MAX CHAR PER DESCRIVERE LA PRESTAZIONE (50)
#define DATA 11 //MAX CHAR DATA (10)
#define NOME 21 //MAX CHAR NOME (20)
#define EXIT -1
#define COGNOME 31 //MAX CHAR COGNOME (30)
#define RICETTA 16 //MAX CHAR CODICE_RICETTA (15)
#define CODICE_PRENOTAZIONE 6 //MAX CHAR CODICE DI PRENOTAZIONE (5)
#define ORARIO 3 //MAX CHAR ORARIO APPUNTAMENTO/DISPONIBILITA (2)
#define REPARTO 31 //MAX CHAR NOME REPARTO (30)
#define PORTA_REPARTO 5 //MAX CHAR PORTA DEL SERVER DEL REPARTO

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

struct reparti
{
	char reparto[REPARTO];
	char porta[PORTA_REPARTO];
};

struct appuntamento
{
	char data[DATA];
	char orario[ORARIO];
};

int scelta_reparto(int);
void read_from_db_reparti(struct reparti **, int *);
int count_rows(int);
void ricevi_date_disponibili(int , struct appuntamento **,int *, struct prenotazione );
int conferma_appuntamento(int, struct prenotazione);
void invia_prenotazione_completa(int, struct prenotazione);
int invia_prestazioni_erogabili(int , char **, int );
int scelta_data_orario_disponibile(int , struct appuntamento *, int );
void invia_conferma_data(int ,int );
void genera_codice_prenotazione(char *);
void invia_codice_prenotazione_client(int ,char *);
