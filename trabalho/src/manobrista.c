#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "unistd.h"
#include "semaphore.h"

#define N_CARROS 20
#define FILA_ESTACIONAMENTO 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t manobrista_cond = PTHREAD_COND_INITIALIZER;

sem_t sem_vagas;
sem_t sem_fila_estacionamento;

int qtd_carros = 0;
int carros_fila = 0;
int qtd_manobristas;
int qtd_vagas;

/*
    Essa função é responsável por simular o comportamento de um manobrista. Ele é uma thread que fica esperando
    um carro chegar, para então estacionar o carro. Se não tiver vagas no estacionamento, ele fica esperando até
    que um carro saia, liberando uma vaga. Após estacionar um carro, ele volta para a fila de carros para estacionar
    o próximo carro.
*/
void* manobrista(void* arg);

/*
    Essa função é responsável por simular o comportamento de um carro. Ele é uma thread que fica esperando
    um manobrista estacionar o carro. Após estacionar o carro, o motorista irá fazer alguma coisa, o que, demorará
    um certo tempo. Após isso, ele sai do estacionamento liberando uma vaga e a thread é finalizada.
*/
void* carro(void* arg);

void main(argc, argv)
int argc;
char *argv[];
{
    int i;
    int *id;

    qtd_manobristas = atoi(argv[1]);
    qtd_vagas = atoi(argv[2]);

    pthread_t manobristas[qtd_manobristas];
    pthread_t carros[N_CARROS];

    sem_init(&sem_vagas, 0, qtd_vagas);
    sem_init(&sem_fila_estacionamento, 0, FILA_ESTACIONAMENTO);
    

    for (i = 0; i < qtd_manobristas; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(manobristas[i]), NULL, manobrista, (void *)(id));
    }

    for (i = 0; i < N_CARROS; i++) {
        int* id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(carros[i]), NULL, carro, (void *)(id));
    }

    // chega_carros();

    for (i = 0; i < N_CARROS; i++) {
        pthread_join(carros[i], NULL);
    }
}

void* manobrista(void* arg)
{
    int id = *((int *)arg);
    // printf("Manobrista %d criado\n", id);
    while (1) {
        // Se não tiver carros ou vagas, o manobrista fica esperando

        pthread_mutex_lock(&mutex);
        while (carros_fila == 0 || qtd_vagas == 0) {
            printf("Manobrista %d esperando\n\n", id);
            pthread_cond_wait(&manobrista_cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);

        // Manobrista estaciona o carro

        printf("Manobrista %d estacionou um carro\n\n", id);

        // Quantidade de vagas disponiveis diminui
        // sem_wait(&sem_vagas);

        pthread_mutex_lock(&mutex);
        // qtd_manobristas--;
        qtd_carros++;
        qtd_vagas--;
        // qtd_manobristas++;
        // pthread_cond_signal(&carro_cond);
        pthread_mutex_unlock(&mutex);
    }
}


void* carro(void* arg) {
    int id = *((int *)arg);
    // printf("Carro %d chegou\n", id);
        // Tenta entrar na fila de carros
        if (sem_trywait(&sem_fila_estacionamento) == 0) {
            printf("Carro %d entrou na fila\n\n", id);
            // Da carro ao manobrista
            printf("Motorista do carro %d dando carro ao manobrista\n\n", id);
            sem_post(&sem_fila_estacionamento);
            pthread_cond_signal(&manobrista_cond);

            // Ocupa uma vaga no estacionamento
            sem_wait(&sem_vagas);
            // Faz suas tarefas
            printf("Motorista do carro %d fazendo alguma coisa\n\n", id);
            sleep(rand() % 20);
            // Vai embora
            printf("Carro %d saindo\n\n", id);
            sem_post(&sem_vagas);

        }
        else {
            printf("Carro %d não entrou na fila e foi embora\n\n", id);
        }
    
}

/*
    TODO: 
        Verificar se o programa esta funcionando corretamente

*/