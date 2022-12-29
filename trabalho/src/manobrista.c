#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "unistd.h"

#define MAXCARROS 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t manobrista_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t carro_cond = PTHREAD_COND_INITIALIZER;

int qtd_carros = 1;

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

/*
    A ideia dessa função é sempre gerar novos carros, que são threads. Assim, simulando a chegada de novos carros em 
    um estacionamento.
*/
void chega_carros();


void main(argc, argv)
int argc;
char *argv[];
{
    int i;
    int *id;
    int qtd_manobristas, qtd_vagas;

    qtd_manobristas = atoi(argv[1]);
    qtd_vagas = atoi(argv[2]);

    pthread_t manobristas[qtd_manobristas];
    // pthread_t carros;

    for (i = 0; i < qtd_manobristas; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(manobristas[i]), NULL, manobrista, (void *)(id));
    }

    chega_carros();

    // for (i = 0; i < qtd_manobristas; i++) {
    //     pthread_join(manobristas[i], NULL);
    // }
}

void* manobrista(void* arg)
{
    int id = *((int *)arg);
    printf("Manobrista %d criado\n", id);
}

void chega_carros() {
    // int i = 0;
    while(1) { 
        /*
            A ideia é que o programa sempre crie novos carros, que são threads. Mas para evitar um número muito grande,
            o que deixaria complicado a visualização, criei um limite de no máximo 100 carros

        */
        if (qtd_carros <= MAXCARROS) {
            pthread_t carro;
            int* id = (int *)malloc(sizeof(int));
            *id = qtd_carros;
            pthread_create(&carro, NULL, carro, (void *)(id));
            qtd_carros++;
        }
    }
}

void* carro(void* arg) {
    int id = *((int *)arg);
    printf("Carro %d chegou\n", id);
}