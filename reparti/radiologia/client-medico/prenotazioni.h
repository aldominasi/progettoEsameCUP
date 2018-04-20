#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#define PRESTAZIONE 51 //MAX CHAR PER DESCRIVERE LA PRESTAZIONE (50)
#define TRUE 1
#define FALSE 0
#define DATA 11 //MAX CHAR DATA (10)
#define N_PRESTAZIONI 3 //NUMERO DI PRESTAZIONI EROGABILI DAL REPARTO
#define LISTA_PRENOTAZIONI 7 //RICEVE LA LISTA DELLE PRENOTAZIONI AL CLIENT DEL MEDICO
#define LISTA_TUTTE_PRENOTAZIONI 8//RICEVE LA LISTA DI TUTTE LE PRENOTAZIONI PRESENTI IN RADIOLOGIA
#define EXIT -1
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

void invia_data(int);
void ricevi_prenotazioni(int , struct prenotazione **, int *);
void ordina_per_orario(struct prenotazione **, int );
void converti_data(int *,int *, int *, char *);
int confronta_giorno(char *,char *);
void ordina_per_giorno_e_orario(struct prenotazione **, int );
void visualizza_lista_prenotazioni(struct prenotazione *, int );
void invia_operazione(int, int);
void assegna_prenotazione(struct prenotazione *,struct prenotazione );
