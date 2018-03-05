#include "mywrapper.h"
#include "pacchetti.h"
#include "disponibilita.h"

int main(int argc, char *argv[])
{
	int i;
	struct disponibilita lista_disponibilita[ROW];
	loadDB(lista_disponibilita);
	lista_disponibilita[0].disponibile = '0';
	if(conferma_appuntamento(lista_disponibilita) == 0)
		fprintf(stderr,"Errore in conferma appuntamento\n");
	else
		fprintf(stdout,"Appuntamento confermato\n");
	exit(0);
}
