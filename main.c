#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

sem_t main_empty,sec_empty;
sem_t sem_main;
sem_t impress;
pthread_t *main_buffer, *sec_buffer;
int main_index = 0, sec_index = 0, MAX;

void show_trace(){
    printf("Threads in main buffer\n");
    printf("----------------------\n");
    for(int i=0; i<MAX; i++){
        printf("Slot %d [%ld] \n", i, main_buffer[i]);
    }
    printf("----------------------\n");
}

void *adm_thread(){
    pthread_t id = pthread_self();
    sem_wait(&sem_main);
    main_buffer[main_index] = id;
    if(main_index!=MAX){
        main_index++;
    }
    sem_post(&sem_main);

    sem_wait(&impress);
    sleep(1);
    main_buffer[main_index] = 0;
    if(main_index!=0){
        main_index--;
    }

    show_trace();
    sem_post(&impress);
    sem_post(&main_empty);
    return NULL;
}

void *div_thread(){
    sec_index++;
    sem_wait(&impress);

    pthread_t id = pthread_self();

    sem_post(&impress);
    sec_index--;
    sem_post(&sec_empty);
    return NULL;
}

int main(int argc, char **argv){
    int QTD_ADM, QTD_DIVERSOS, thread_count = 0;
    int adm_count = 0, div_count = 0;
    pthread_t *all_threads;
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

    sem_init(&main_empty, 0, MAX);
    sem_init(&sem_main, 0, 1);
    sem_init(&sec_empty, 0, MAX);
    sem_init(&impress, 0, 1);
    main_buffer = (pthread_t*)malloc(MAX * sizeof(pthread_t));
    sec_buffer = (pthread_t*)malloc(MAX * sizeof(pthread_t));
    all_threads = (pthread_t*)malloc((QTD_ADM+QTD_DIVERSOS) * sizeof(pthread_t));

    while(adm_count != QTD_ADM){
        sem_wait(&main_empty);
        pthread_create(&all_threads[thread_count], NULL, adm_thread, NULL);
        adm_count++;
        thread_count++;
    }
    while(div_count != QTD_DIVERSOS){
        sem_wait(&sec_empty);
        pthread_create(&all_threads[thread_count], NULL, div_thread, NULL);
        div_count++;
        thread_count++;
    }
    for(int i=0; i<(QTD_ADM+QTD_DIVERSOS); i++){
        pthread_join(all_threads[i], NULL);
    }

    sem_destroy(&main_empty);
    sem_destroy(&sec_empty);
    sem_destroy(&impress);

    free(main_buffer);
    free(sec_buffer);
    free(all_threads);
}
