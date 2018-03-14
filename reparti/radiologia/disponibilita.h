#include <sys/types.h>
#include <fcntl.h>

#define PRESTAZIONE 51 //MAX CHAR PER DESCRIVERE LA PRESTAZIONE (50)
#define TRUE 1
#define FALSE 0
#define DATA 11 //MAX CHAR DATA (10)
#define N_PRESTAZIONI 3 //NUMERO DI PRESTAZIONI EROGABILI DAL REPARTO
#define PRESTAZIONI_EROGABILI 0 //INVIA AL CUP LE PRESTAZIONI EROGABILI
#define DATE_DISPONIBILI 1 //INVIA AL CUP LE DATE PRENOTABILI PER UNA PRESTAZIONE
#define CONFERMA_DATA 2 //CONTROLLA E NEL CASE CONFERMA LA DATA AL CUP
#define SALVA_PRENOTAZIONE 3 //RICEVE DAL CUP I DATI DELLA PRENOTAZIONE
#define CANCELLA_PRENOTAZIONE 4 //CANCELLA LA PRENOTAZIONE INVIATA DAL CUP
#define INFO_PRENOTAZIONE 5 //INVIA INFORMAZIONI RIGUARDANTI LA PRENOTAZIONE INVIATA DAL CUP
#define LISTA_PRENOTAZIONI_UTENTE 6 //INVIA LA LISTA DELLE PRENOTAZIONI ATTIVE DELL'UTENTE INVIATO DAL CUP
#define LISTA_PRENOTAZIONI 7 //INVIA LA LISTA DELLE PRENOTAZIONI AL CLIENT DEL MEDICO
#define EXIT -1
#define NOME 21 //MAX CHAR NOME (20)
#define COGNOME 31 //MAX CHAR COGNOME (30)
#define RICETTA 16 //MAX CHAR CODICE_RICETTA (15)
#define CODICE_PRENOTAZIONE 6 //MAX CHAR CODICE_PRENOTAZIONE (5)
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

struct disponibilita
{
	char prestazione[PRESTAZIONE];
	char data[DATA];
	char orario[ORARIO];
	char disponibile;
};

struct appuntamento
{
	char data[DATA];
	char orario[ORARIO];
};

void read_from_db(struct disponibilita **, int *);
int write_into_db(struct disponibilita *,int);
void invia_prestazioni_erogabili(int);
void invia_date_disponibili(int, char *);
void conferma_appuntamento(int, struct disponibilita);
void inserisci_prenotazione_in_agenda(int sock);
int write_into_db_prenotazioni(struct prenotazione *, int);
void read_from_db_prenotazioni(struct prenotazione **, int *);
int count_lines(int);
void cancella_prenotazione(int , char *);
void informazioni_prenotazione(int);
void invia_lista_prenotazioni(int,char *);
