#define NOME 50
#define COGNOME 50
#define VISITA 50
#define DATA 11
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
	char visita[VISITA];
	int codice_visita;
	char data_appuntamento[DATA];
	char codice_ricetta[RICETTA];
	char codice_prenotazione[5];
};

struct appuntamenti
{
	char data[DATA];
	int orario;
};
