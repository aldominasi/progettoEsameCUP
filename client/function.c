#include "frdwr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

void scegli_reparto(int sock)
{
	int i, n, length, codice_scelto;
	char **reparti;
	while(FullRead(sock,&n,sizeof(int)) > 0);
	reparti = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		reparti[i] = (char *)malloc(length * sizeof(char));
		while(FullRead(sock,&reparti[i],length) > 0);
		fprintf(stdout,"codice: %d reparto: %s\n",i+20,reparti[i]);
	}
	fprintf(stdout,"Inserire il codice relativo al reparto a cui si intende far riferimento: ");
	fscanf(stdin, "%d",&codice_scelto);
	FullWrite(sock,&codice_scelto,sizeof(int));
}

void scegli_prestazioni_erogabili(int sock)
{
	int i, n, length, codice_scelto;
	char **prestazioni;
	while(FullRead(sock,&n,sizeof(int)) > 0);
	prestazioni = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		prestazioni[i] = (char *)malloc(length*sizeof(char));
		while(FullRead(sock,&prestazioni[i],length) > 0);
		fprintf(stdout,"codice: %d reparto: %s\n",i+40,prestazioni[i]);
	}
	fprintf(stdout,"Inserire il codice relativo alla prestazione a cui si intende far riferimento: ");
	fscanf(stdin,"%d",&codice_scelto);
	FullWrite(sock,&codice_scelto,sizeof(int));
}

void invia_operazione(int sock,int operazione)
{
	FullWrite(sock,&operazione,sizeof(int));
}
