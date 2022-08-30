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

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * f_batedor(void * arg);
void * f_goleiro(void *arg);

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
            sleep(2);    
        }
        
    }

    for (i = 0; i < GOLEIROS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&(goleiros[i].goleiro), NULL, f_goleiro, (void *) (id));
        if(erro) {
            printf("A thread %d nao foi criada!!!\n", *(int *) id);
        }
        else{
            printf("O goleiro %d foi criado!\n", *(int *) id);
            sleep(2);    
        }
    }

    for (i = 0; i < BATEDORES; i++) {
        pthread_join(batedores[i], NULL);
    }

    for (i = 0; i < GOLEIROS; i++) {
        pthread_join(goleiros[i].goleiro, NULL);
    }
    
}

void* f_batedor(void *arg) {
    //pthread_mutex_lock(&lock);
    printf("Jogador %d: Bateu!\n", *(int *) arg);
    
    //pthread_mutex_unlock(&lock);

    pthread_exit(0);
}

void* f_goleiro(void *arg) {
    printf("Goleiro %d defendeu!\n", *(int *) arg);
}