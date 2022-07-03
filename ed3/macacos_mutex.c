#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_monkeysA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_monkeysB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER;

int numberMonkeysA = 0;
int numberMonkeysB = 0;

void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
         //Procedimentos para acessar a corda
      // Lock que seta a vez para os macacos passarem, seja no sentido AB  ou BA
      pthread_mutex_lock(&lock_turn);
        pthread_mutex_lock(&lock_monkeysA); // Lock que bloqueia o lado A para podermos fazer as operações
          numberMonkeysA++;
        if (numberMonkeysA == 1) {
          pthread_mutex_lock(&lock_corda); // Lock que bloqueia a corda só para os macaco no sentido AB
        }
        pthread_mutex_unlock(&lock_monkeysA);
      
      pthread_mutex_unlock(&lock_turn);
        
        printf("Macaco %d passado de A para B \n",i);
        printf("Macacos de A na corda %d\n", numberMonkeysA);
        sleep(1);
        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_monkeysA);
          numberMonkeysA--;
        if (numberMonkeysA == 0) {
          pthread_mutex_unlock(&lock_corda);
        }
        pthread_mutex_unlock(&lock_monkeysA); // Comportamento semelhante ao problema dos leitores-escritores

    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
         //Procedimentos para acessar a corda
      pthread_mutex_lock(&lock_turn); // Aqui acontece o mesmo que foi explicado no macacoAB
        pthread_mutex_lock(&lock_monkeysB);
          numberMonkeysB++;
        if (numberMonkeysB == 1) {
          pthread_mutex_lock(&lock_corda);
        }
        pthread_mutex_unlock(&lock_monkeysB);
      pthread_mutex_unlock(&lock_turn);
     
      
        printf("Macaco %d passado de B para A \n",i);
        printf("Macacos de B na corda %d\n", numberMonkeysB);
        sleep(1);
        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_monkeysB);
          numberMonkeysB--;
        if (numberMonkeysB == 0) {
          pthread_mutex_unlock(&lock_corda);
        }
        pthread_mutex_unlock(&lock_monkeysB);
    }
    pthread_exit(0);
}



void * gorila(void * a){
    while(1){
	//Procedimentos para acessar a corda
  pthread_mutex_lock(&lock_turn); // Este lock irá bloquear o acesso para que somente o gorila possa andar pela corda
    pthread_mutex_lock(&lock_corda);
  pthread_mutex_unlock(&lock_turn);
    printf("Gorila passado de A para B \n");
    sleep(5);
    pthread_mutex_unlock(&lock_corda);
        //Procedimentos para quando sair da corda
     }
    pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}
