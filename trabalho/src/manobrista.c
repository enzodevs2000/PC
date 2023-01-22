#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "unistd.h"
#include "semaphore.h"

#define N_CARROS 20 // Número de threads representando os carros
#define FILA_ESTACIONAMENTO 2 // Número de vagas na fila para o estacionamento

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Lock para garantir exclusão mútua nas variáveis compartilhadas
pthread_cond_t manobrista_cond = PTHREAD_COND_INITIALIZER; // Variável de condição para o manobrista

sem_t sem_vagas; // Semáforo que representa a quantidade de vagas do estacionamento
sem_t sem_fila_estacionamento; // Semáforo que representa a quantidade de vagas que há na fila para o estacionamento

int qtd_carros = 0; // Quantidade de carros que estão estacionados no estacionamento
int carros_fila = 0; // Quantidade de carros que estão na fila para estacionar
int carros_embora = 0; // Quantidade de carros que já saíram do estacionamento

int qtd_manobristas; // Quantidade de manobristas que vão trabalhar no estacionamento
int qtd_vagas; // Quantidade de vagas do estacionamento

void *manobrista(void *arg);
/*
    Essa função é responsável por simular o comportamento de um manobrista. Ele é uma thread que fica esperando
    um carro chegar, para então estacionar o carro. Se não tiver vagas no estacionamento, ele fica esperando até
    que um carro saia, liberando uma vaga. Após estacionar um carro, ele volta para a fila de carros para estacionar
    o próximo carro.
*/

void *carro(void *arg);
/*
    Essa função é responsável por simular o comportamento de um carro. Ele é uma thread que fica esperando
    um manobrista estacionar o carro. Após estacionar o carro, o motorista irá fazer alguma coisa, o que, demorará
    um certo tempo. Após isso, ele sai do estacionamento liberando uma vaga e a thread é finalizada.
*/

void main(argc, argv) int argc;
char *argv[];
{
    int i;
    int *id;
    int vagas;

    qtd_manobristas = atoi(argv[1]);
    vagas = atoi(argv[2]);
    qtd_vagas = vagas;

    pthread_t manobristas[qtd_manobristas];
    pthread_t carros[N_CARROS];

    sem_init(&sem_vagas, 0, qtd_vagas);
    sem_init(&sem_fila_estacionamento, 0, FILA_ESTACIONAMENTO);

    for (i = 0; i < qtd_manobristas; i++) {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(manobristas[i]), NULL, manobrista, (void *)(id));
    }

    for (i = 0; i < N_CARROS; i++) {
        int *id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(carros[i]), NULL, carro, (void *)(id));
    }

    for (i = 0; i < N_CARROS; i++) {
        pthread_join(carros[i], NULL); // O programa é finalizado quando todos os carros já passaram pelo estacionamento,
                                       //   seja por estacionar ou por não conseguir estacionar
    }
}

void *manobrista(void *arg) {
    int id = *((int *)arg);
    while (1) {
        // Se não tiver carros ou vagas, o manobrista fica esperando

        pthread_mutex_lock(&mutex);
        while (carros_fila == 0 || qtd_vagas == 0) {
            printf("Manobrista %d esperando\n\n", id);
            pthread_cond_wait(&manobrista_cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);

        // Manobrista estaciona o carro
        sleep(2);
        printf("Manobrista %d estacionou um carro\n\n", id);

        // Quantidade de vagas disponiveis diminui e quantidade de carros no estacionamento aumenta

        pthread_mutex_lock(&mutex);

        qtd_carros++;
        qtd_vagas--;
        carros_fila--; // Sincronização para diminuir a quantidade de carros na fila

        pthread_mutex_unlock(&mutex);
    }
}

void *carro(void *arg) {
    int id = *((int *)arg);
    // Tenta entrar na fila de carros, se conseguir, então diminui a quantidade de permissões do semáforo
    if (sem_trywait(&sem_fila_estacionamento) == 0) {
        printf("Carro %d entrou na fila\n\n", id);

        pthread_mutex_lock(&mutex);
        carros_fila++; // Sincronização para aumentar a quantidade de carros na fila
        pthread_mutex_unlock(&mutex);

        // Da carro ao manobrista e libera uma vaga na fila dando um UP no semáforo
        sem_post(&sem_fila_estacionamento);

        printf("Motorista do carro %d dando carro ao manobrista\n\n", id);
        

        // Se tiver pelo menos um carro na fila, então o manobrista é acordado
        pthread_mutex_lock(&mutex);
        if (carros_fila == 1) {
            pthread_cond_signal(&manobrista_cond);
        }

        pthread_mutex_unlock(&mutex);

        // Ocupa uma vaga no estacionamento dando um down no semáforo
        
        sem_wait(&sem_vagas);

        // Motorista faz suas tarefas
        printf("Motorista do carro %d fazendo alguma coisa\n\n", id);
        sleep(5);

        // Motorista vai embora liberando uma vaga no estacionamento e dando um UP no semáforo
        printf("Carro %d saindo\n\n", id);

        pthread_mutex_lock(&mutex);
        qtd_vagas++;
        pthread_mutex_unlock(&mutex);

        sem_post(&sem_vagas);
    }
    else { // Se não conseguir entrar na fila, então o carro vai embora
        // Sincronização para contar o número de carros que foram embora
        pthread_mutex_lock(&mutex);
        printf("Carro %d não entrou na fila e foi embora\n\n", id);
        carros_embora++;
        printf("Carros que foram embora: %d\n\n", carros_embora);
        pthread_mutex_unlock(&mutex);
    }
}
