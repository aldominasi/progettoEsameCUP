#include <sys/types.h>
#include <fcntl.h>

#define PRESTAZIONE 50
#define TRUE 1
#define FALSE 0
#define ROW 45
#define DATA 11

struct disponibilita
{
	char prestazione[PRESTAZIONE];
	char data[DATA];
	char orario[3];
	char disponibile;
};

void loadDB(struct disponibilita *);
int conferma_appuntamento(struct disponibilita *);
