#include <sys/types.h>
#include <fcntl.h>

#define PRESTAZIONE 50
#define TRUE 1
#define FALSE 0
#define ROW 45
#define DATA 11
#define N_PRESTAZIONI 3
#define PRESTAZIONI_EROGABILI 0
#define DATE_DISPONIBILI 1
#define NOME 20
#define COGNOME 30
#define RICETTA 15
#define CODICE_PRENOTAZIONE 5

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
	char orario_appuntamento[3];
	char codice_ricetta[RICETTA];
	char codice_prenotazione[CODICE_PRENOTAZIONE];
};

struct disponibilita
{
	char prestazione[PRESTAZIONE];
	char data[DATA];
	char orario[3];
	char disponibile;
};

void read_from_db(struct disponibilita *);
int write_into_db(struct disponibilita *);
void invia_prestazioni_erogabili(int,struct disponibilita *);
void invia_date_disponibili(int, struct disponibilita *, char *);
void conferma_appuntamento(int, struct disponibilita);
void inserisci_prenotazione_in_agenda(int sock);
int write_into_db_prenotazioni(struct prenotazione *, int);
void read_from_db_prenotazioni(struct prenotazione *, int *);
int count_lines(int);
void cancella_prenotazione(int , char *);
