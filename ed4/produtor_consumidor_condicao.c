#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer


void * produtor(void * meuid);
void * consumidor (void * meuid);

int produce_item();
void insert_item(int item);
int remove_item();
void consume_item(int item);
void mostra_buffer();

pthread_cond_t cond_productor = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffer_m = PTHREAD_MUTEX_INITIALIZER;

int count_buffer = 0;
int buffer[N];


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
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
  
    // Lock para condicionar a exclusão mútua
   pthread_mutex_lock(&mutex);
   
   int item = produce_item(); 

  // Enquanto o buffer não estiver cheio o produtor estará produzindo
   while (count_buffer == N){
      pthread_cond_wait(&cond_productor, &mutex);
   }

   // Insere no buffer e incrementa o contado
   insert_item(item);
   count_buffer++;

   // Se tiver pelor menos um elemento no buffer, o consumidor pode começar a funcionar
   if (count_buffer == 1) {
     pthread_cond_signal(&cond_consumer);
   }

   printf("Tamanho no buffer: %d\n", count_buffer);
   mostra_buffer();

   pthread_mutex_unlock(&mutex);


  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
 while(1)
  {
    // Este lock é para o mesmo caso do produtor
    pthread_mutex_lock(&mutex);

    // Enquanto o buffer estiver vazio, o consumidor fica dormindo.
    while (count_buffer == 0) {
      pthread_cond_wait(&cond_consumer, &mutex);
    }

    // Remove o elemento do buffer e decrementa o contador
    int item = remove_item();
    count_buffer--;

    // Se abre uma vaga no buffer, o produtor pode voltar a funcionar
    if (count_buffer == N - 1) {
      pthread_cond_signal(&cond_productor);
    }
    
    consume_item(item);
    mostra_buffer();
    
    pthread_mutex_unlock(&mutex);

  }
  pthread_exit(0);
  
}

int produce_item() {
  int item = rand() % 5;
  printf("O item %d foi produzido.\n", item);
  sleep(1);
  return item;
}

void insert_item(int item) {
  buffer[count_buffer] = item;
  printf("O item %d foi inserido no buffer\n", item);
  sleep(1);
}

int remove_item() {
  int item = buffer[count_buffer];
  if (item != -1) {
    printf("O item %d foi removido do buffer\n", buffer[item]);
    sleep(1);
    buffer[count_buffer] = -1;
  }
  return item;
}

void consume_item(int item) {
  if (item != -1) {
    printf("O item %d foi consumido.\n", item);
  }
  sleep(3);
}
void mostra_buffer() {
  int i;
  printf("\n\n -----BUFFER-----\n");
  for (i = 0; i < N; i++) {
    printf("%d - %d\n", i, buffer[i]);
  }
  printf("\n\n");
}