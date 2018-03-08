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
