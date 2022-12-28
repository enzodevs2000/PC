#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "unistd.h"

void* manobrista(void* arg);
void* carros(void* arg);

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



    // for (i = 0; i < qtd_manobristas; i++) {
    //     pthread_join(manobristas[i], NULL);
    // }
}

void* manobrista(void* arg)
{
    int id = *((int *)arg);
    printf("Manobrista %d criado\n", id);
}

void* carros() {
    while(1) {
        pthread_create
    }
}