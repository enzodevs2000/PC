#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BATEDORES 15
#define GOLEIROS 5

typedef struct {
    pthread_t goleiro;
    int forca_defesa;
} Goleiro;

void * f_batedor(void * arg);

void main(argc, argv) 
int argc;
char* argv[];
{
    pthread_t batedores[BATEDORES];
    Goleiro goleiros[GOLEIROS];

    int i;
    int* id;
    int erro;

    for(i = 0; i < BATEDORES; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    erro = pthread_create(&(batedores[i]), NULL, f_batedor, (void *) (id));
        
        if(erro) {
            printf("A thread %d nao foi criada!!!\n", *(int *) id);
        }
        else{
            printf("O batedor %d foi criado!\n", *(int *) id);
        }
        
    }

    pthread_join(batedores[0], NULL);
    
}

void* f_batedor(void *arg) {
    //printf("Jogador %d: Bateu!\n", *(int *) arg);
    pthread_exit(0);
}