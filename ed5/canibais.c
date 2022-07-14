#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

typedef struct {
  //pthread_t thread;
} Restaurante;

pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t canibais = PTHREAD_COND_INITIALIZER;
pthread_mutex_t comer = PTHREAD_MUTEX_INITIALIZER;
int comida = 0;

   
void *canibal(void*meuid);
void *cozinheiro(int m);


void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) {
    //pegar uma porção de comida e acordar o cozinheiro se as porções 
    pthread_mutex_lock(&comer);
    // Enquanto nao tiver comida, os canibais dormem
    while (comida == 0) {
      pthread_cond_wait(&canibais, &comer);
    }

    comida--;
    printf("O canibal %d pegou uma porcao. %d porcoes restantes\n",*(int *)(pi), comida);

    // Se acabar a comida, o ultimo que pegou avisa o cozinheiro e ele acorda
    if (comida == 0) {
      printf("Acabou a comida!!\n");
      pthread_cond_signal(&cozinheiro_cond);
    }

    pthread_mutex_unlock(&comer);

    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(10);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
     
    //dormir enquanto tiver comida
    pthread_mutex_lock(&comer);

    // Enquanto tiver comida o cozinheiro dorme
    while (comida > 0) {
      pthread_cond_wait(&cozinheiro_cond, &comer);
    }

    // Cozinheiro acordado faz mais m porcoes
    printf("cozinheiro: vou cozinhar\n");
    sleep(20);
    
    comida = m;
    printf("Cozinheiro --Fiz mais %d porcoes, temos %d porcoes para comer\n", m, comida);

    //acordar os canibais
    if (comida == m) {
      printf("TA NA MESA!!\n");
      // Quando o cozinheiro faz as porcoes, ele acorda todos os carnivoros com o broadcast
      pthread_cond_broadcast(&canibais);
    }

    pthread_mutex_unlock(&comer);
   }

}

