#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

sem_t main_sem;
sem_t sec_sem;
int ver = 0;

void *adm_thread(){
    sem_wait(&main_sem);
    pthread_t id = pthread_self();
    printf("Thread [%ld] is adm\n", id);

    if(ver!=0){
        sem_post(&sec_sem);
    }else{
        sem_post(&main_sem);
        ver++;
    }
    return NULL;
}

void *div_thread(){
    sem_wait(&sec_sem);

    pthread_t id = pthread_self();
    printf("Thread [%ld] is diverse\n", id);

    ver = 0;
    sem_post(&main_sem);
    return NULL;
}

int main(int argc, char **argv){
    int MAX, QTD_ADM, QTD_DIVERSOS;
    int adm_count = 0, div_count = 0;
    pthread_t *main_buffer, *sec_buffer;
    srand(time(NULL)); 
    if (argc == 4 && strtol(argv[1], NULL, 10) != 0) {
        MAX = abs(strtol(argv[1], NULL, 10));
        QTD_ADM = abs(strtol(argv[2], NULL, 10));
        QTD_DIVERSOS = abs(strtol(argv[3], NULL, 10));
    } else if(argc > 1 && strtol(argv[1], NULL, 10)==0) {
        printf("O armazenamento (MAX) deve ser maior que 0.\n");
        exit(0);
    }
    else {
        printf("É necessário passar a capacidade de armazenamento (MAX), o numero de arquivos administrativos (QTD_ADM) e o numero de arquivos diversos (QTD_DIVERSOS) por parametro.\n");
        printf("Ex.: ./<file-name> 100 4 4\n");
        exit(0);
    }

    sem_init(&main_sem, 0, 1);
    sem_init(&sec_sem, 0, 0);
    main_buffer = (pthread_t*)malloc(MAX * sizeof(pthread_t));
    sec_buffer = (pthread_t*)malloc(MAX * sizeof(pthread_t));
    while(adm_count!=QTD_ADM || div_count!=QTD_DIVERSOS){
        if(adm_count!=QTD_ADM+1){
            pthread_create(&main_buffer[adm_count], NULL, adm_thread, NULL);
        }
        if(div_count!=QTD_DIVERSOS+1){
            pthread_create(&sec_buffer[div_count], NULL, div_thread, NULL);
        }

        adm_count = adm_count<=QTD_ADM ? adm_count+1 : QTD_ADM+1;
        div_count = div_count<=QTD_DIVERSOS ? div_count+1 : QTD_DIVERSOS+1;
    }
    while(adm_count<=QTD_ADM || div_count<=QTD_DIVERSOS){
        pthread_join(main_buffer[adm_count], NULL);
        pthread_join(sec_buffer[div_count], NULL);

        adm_count = adm_count<=QTD_ADM ? adm_count+1 : adm_count;
        div_count = div_count<=QTD_DIVERSOS ? div_count+1 : div_count;
    }

    sem_destroy(&main_sem);
    sem_destroy(&sec_sem);

    free(main_buffer);
    free(sec_buffer);
}
