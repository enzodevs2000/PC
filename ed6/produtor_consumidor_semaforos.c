#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer


void * produtor(void * meuid);
void * consumidor (void * meuid);

int produce_item();
void insert_item(int item);
int remove_item();
void consume_item();
void mostra_buffer();

sem_t empty, full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int count_buffer = 0;


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
  sem_init(&empty, 0, N); // Inicializar os semaforos
  sem_init(&full, 0, 0);
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi)
{
 
  while(1)
  {
   int item = produce_item();

   sem_wait(&empty); // diminui o contador do semaforo empty

   pthread_mutex_lock(&mutex); // Protege a regiao critica

    insert_item(item);
    count_buffer++;
   
   pthread_mutex_unlock(&mutex);

   sem_post(&full); // Aumenta o contado do semaforo full

   mostra_buffer();

  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
 while(1)
  {
    sem_wait(&full); // Diminui o contador do semaforo full

    pthread_mutex_lock(&mutex); // Protege a regiao critica

      remove_item();
      count_buffer--;
    
    pthread_mutex_unlock(&mutex);

    sem_post(&empty); // Aumenta O contador do semaforo empty

    consume_item();

    mostra_buffer();
  }
  pthread_exit(0);
  
}


int produce_item() {
  int item = rand() % 5;
  printf("O item %d foi produzido.\n", item);
  sleep(5);
  return item;
}

void insert_item(int item) {
  printf("O item %d foi inserido no buffer\n", item);
  sleep(3);
}

int remove_item() {
  
  printf("Um item foi removido do buffer\n");
  sleep(3);
  
}

void consume_item() {
  
  printf("Um item foi consumido.\n");
  
  sleep(8);
}

void mostra_buffer() {
  int i;
  printf("Total de elementos no buffer: %d\n", count_buffer);
  
  printf("\n\n");
}