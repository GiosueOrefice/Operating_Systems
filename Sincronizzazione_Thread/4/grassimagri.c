#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 3 /* max capacita ponte */
#define dx 0 /*costanti di direzione*/
#define sn 1

typedef struct{

	int nmagri[2]; /* numero magri sul ponte (per ogni dir.)*/
	int ngrassi[2];/* numero grassi sul ponte (per ogni dir.)*/
	pthread_mutex_t lock;/*lock associato al"ponte" */
	pthread_cond_t codamagri[2]; /* var. cond. sosp. magri */
	pthread_cond_t codagrassi[2]; /* var. cond. sosp. grassi */


}ponte;

/* Inizializzazione del ponte */
void init (ponte *p){
	pthread_mutex_init (&p->lock, NULL);

	pthread_cond_init (&p->codamagri[dx], NULL);
	pthread_cond_init (&p->codamagri[sn], NULL);
	pthread_cond_init (&p->codagrassi[dx], NULL);
	pthread_cond_init (&p->codagrassi[sn], NULL);

	p->nmagri[dx]=0;
	p->nmagri[sn]=0;
	p->ngrassi[dx]=0;
	p->ngrassi[sn]=0;

}

/*operazioni di utilita`: */
int sulponte(ponte p); /* calcola il num. di persone sul ponte */
int altra_dir(int d); /* calcola la direzione opposta a d */

/* Accesso al ponte di un magro in direzione d: */
void accessomagri (ponte *p, int d){ 
	pthread_mutex_lock (&p->lock);
	/* controlla le codizioni di accesso:*/
	while ( (sulponte(*p)==MAX) || /* vincolo di capacita` */
	(p->ngrassi[altra_dir(d)]>0) ){/*ci sono grassi in direzione opposta */

		pthread_cond_wait (&p->codamagri[d], &p->lock);
		printf("magro aspetta %lu\n",pthread_self());
	}

	/* entrata: aggiorna lo stato del ponte */
	p->nmagri[d]++;
	printf("Magro in dir %d: sto attraversando %lu\n", d,pthread_self());
	pthread_mutex_unlock (&p->lock);
	}

/*Accesso al ponte di un grasso in dir.d: */
void accessograssi (ponte *p, int d){

	pthread_mutex_lock (&p->lock);
	/* controlla le codizioni di accesso:*/
	while ( (sulponte(*p)==MAX) || (p->ngrassi[altra_dir(d)]>0)|| (p->nmagri[altra_dir(d)]>0) ){

		pthread_cond_wait (&p->codagrassi[d], &p->lock);
		printf("grasso aspetta %lu\n",pthread_self());
	}
	
	/* entrata: aggiorna lo stato del ponte */
	p->ngrassi[d]++;
	printf("Grasso in dir %d: sto attraversando  %lu\n", d,pthread_self());
	pthread_mutex_unlock (&p->lock);
}

/* Rilascio del ponte di un magro in direzione d: */
void rilasciomagri (ponte *p, int d){
	pthread_mutex_lock (&p->lock);
	/* uscita: aggiorna lo stato del ponte */
	p->nmagri[d]--;
	/* risveglio in ordine di priorita` */
	pthread_cond_broadcast (&p->codamagri[altra_dir(d)]);
	pthread_cond_broadcast (&p->codamagri[d]);
	pthread_cond_broadcast (&p->codagrassi[altra_dir(d)]);
	pthread_cond_broadcast (&p->codagrassi[d]);

	printf("USCITA: magro in direzione %d\n\n", d);
	pthread_mutex_unlock (&p->lock);
}

/* Rilascio del ponte di un grasso in direzione d: */
void rilasciograssi (ponte *p, int d){
	pthread_mutex_lock (&p->lock);
	/* uscita: aggiorna lo stato del ponte */
	p->ngrassi[d]--;
	/* risveglio in ordine di priorita` */
	pthread_cond_broadcast (&p->codamagri[altra_dir(d)]);
	pthread_cond_broadcast (&p->codamagri[d]);
	pthread_cond_broadcast (&p->codagrassi[altra_dir(d)]);
	pthread_cond_broadcast (&p->codagrassi[d]);
	printf("\nUSCITA: grasso in direzione %d\n\n", d);
	pthread_mutex_unlock (&p->lock);
}

/* Programma di test: genero un numero arbitrario di thread magri e grassi nelle due direzioni */
#define MAXT 20 /* num. max di thread per tipo e per direzione */

ponte p;

void *magro (void *arg){ /*codice del thread "magro" */
	int d;
	d=atoi((char *)arg); /*assegno la direzione */
	accessomagri (&p, d);
	/* ATTRAVERSAMENTO: */
	sleep(1);
	rilasciomagri(&p,d);
	return NULL;
}

void *grasso (void *arg){ /*codice del thread "grasso" */ 
	int d;
	d=atoi((char *)arg); /*assegno la direzione */
	accessograssi (&p, d);
	sleep(1);
	rilasciograssi(&p,d);
	return NULL;
}

int main (){

	pthread_t th_M[2][MAXT], th_G[2][MAXT];
	int NMD, NMS, NGD, NGS, i;
	void *retval;
	init (&p);

	/* Creazione threads: */
	printf("quanti magri in direzione dx? ");
	scanf("%d", &NMD);
	printf("quanti magri in direzione sn? ");
	scanf("%d", &NMS);
	printf("quanti grassi in direzione dx? ");
	scanf("%d", &NGD);
	printf("quanti grassi in direzione sn? ");
	scanf("%d", &NGS);
	printf("\n\n");
	/*CREAZIONE GRASSI IN DIREZIONE DX */
	for (i=0; i<NGD; i++)
	pthread_create (&th_G[dx][i], NULL, grasso, "0");
	/*CREAZIONE GRASSI IN DIREZIONE SN */
	for (i=0; i<NGS; i++)
	pthread_create (&th_G[sn][i], NULL, grasso, "1");
	/*CREAZIONE MAGRI IN DIREZIONE DX */
	for (i=0; i<NMD; i++)
	pthread_create (&th_M[dx][i], NULL, magro, "0");
	/*CREAZIONE MAGRI IN DIREZIONE SN */
	for (i=0; i<NMS; i++)
	pthread_create (&th_M[sn][i], NULL, magro, "1");

	/* Attesa teminazione threads creati: */
	/*ATTESA MAGRI IN DIREZIONE DX */
	for (i=0; i<NMD; i++)
	pthread_join(th_M[dx][i], &retval);
	/*ATTESA MAGRI IN DIREZIONE SN */
	for (i=0; i<NMS; i++)
	pthread_join(th_M[sn][i], &retval);
	/*ATTESA GRASSI IN DIREZIONE DX */
	for (i=0; i<NGD; i++)
	pthread_join(th_G[dx][i], &retval);
	/*ATTESA GRASSI IN DIREZIONE SN */
	for (i=0; i<NGS; i++)
	pthread_join(th_G[sn][i], &retval);
	return 0;
}

/* definizione funzioni utilita`:*/
int sulponte(ponte p){ 
	return p.nmagri[dx] + p.ngrassi[dx] + p.nmagri[sn]+ p.ngrassi[sn];
}

int altra_dir(int d){
 if (d==sn) 
 	return dx;
else 
	return sn;
}