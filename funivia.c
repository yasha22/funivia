
//librerie
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

//costanti
#define ERRORE_CREAZIONE_SEMAFORI	-1000
#define ERRORE_CREAZIONE_THREAD		-1001
#define ERRORE_JOIN			-1002 
#define NUMERO_MASSIMO_PASSEGGERI	 50
#define RED   "\x1B[31m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define RESET "\x1B[0m"

typedef sem_t semaphore;
typedef pthread_t thread;

//semafori per sincronizzare e rendez-vous
semaphore semaforo_porta_1;
semaphore semaforo_porta_2;

//prototipi
void *funzione_porta_1(void*);
void *funzione_porta_2(void*);

int main(int argc, char **argv)
{
	thread thread_porta_1;
	thread thread_porta_2;
	
	unsigned int passeggeri = 0;
	unsigned int *entrati_porta_1 = 0;
	unsigned int *entrati_porta_2 = 0;
	
	//semaforo
	if( sem_init(&semaforo_porta_1, 0, 0) == -1
	|| sem_init(&semaforo_porta_2, 0, 1) == -1 ){	
		printf("ERRORE_CREAZIONE_SEMAFORI");
		exit(ERRORE_CREAZIONE_SEMAFORI);
	}
	
	//cobegin threads
	if( pthread_create(&thread_porta_1, NULL, funzione_porta_1, (void*)&passeggeri) != 0 
	|| pthread_create(&thread_porta_2, NULL, funzione_porta_2, (void*)&passeggeri )!= 0){
		printf("ERRORE_CREAZIONE_THREAD");
		exit(ERRORE_CREAZIONE_THREAD);
	}
	
	//join
	if (pthread_join(thread_porta_1, (void **)&entrati_porta_1) == -1 
	|| pthread_join(thread_porta_2, (void **)&entrati_porta_2) == -1){
		printf("ERRORE_JOIN");
		exit(ERRORE_JOIN); 
	}

	printf(RED "\npersone entrate da porta 1: %d\n"RESET, *entrati_porta_1);
	printf(RED "persone entrate da porta 2: %d\n"RESET, *entrati_porta_2);
	
	return 0;
}

void *funzione_porta_1(void* arg)
{
	unsigned int *passeggeri = arg;
	unsigned int passeggeri_entrati = 0;
	
	while(true)
	{
		
		//sezione critica		
		sem_wait(&semaforo_porta_2);
		if(*passeggeri < NUMERO_MASSIMO_PASSEGGERI){
		
			//entra un passeggero
			*passeggeri += 1;
			printf(BLU "Entrata una persona dalla porta 1, posti liberi: %d\n"RESET,
				(NUMERO_MASSIMO_PASSEGGERI - *passeggeri));
			
			//fine sezione critica
			sem_post(&semaforo_porta_1);
			
			passeggeri_entrati += 1;
			
			if(*passeggeri < NUMERO_MASSIMO_PASSEGGERI) continue;
			else break;
			
		} break;
	}
	
	//printf("persone entrate da porta 1: %d\n",passeggeri_entrati);
	pthread_exit(&passeggeri_entrati);
}

void *funzione_porta_2(void* arg)
{
	unsigned int *passeggeri = arg;
	unsigned int passeggeri_entrati = 0;
	
	while(true)
	{

		sem_wait(&semaforo_porta_1);
		if(*passeggeri < NUMERO_MASSIMO_PASSEGGERI){
			
			//entra un passeggero
			*passeggeri += 1;
			printf(MAG "Entrata una persona dalla porta 2, posti liberi: %d\n"RESET,
				(NUMERO_MASSIMO_PASSEGGERI - *passeggeri));		
		
			//fine sezione critica
			sem_post(&semaforo_porta_2);	
			
			passeggeri_entrati += 1;	
			
			if(*passeggeri < NUMERO_MASSIMO_PASSEGGERI) continue;
			else break;
		
		} break;

	}
	//printf("\npersone entrate da porta 2: %d\n",passeggeri_entrati);
	pthread_exit(&passeggeri_entrati);
}
