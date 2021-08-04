#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

sem_t main_empty,sec_empty;
sem_t sem_main, sem_sec;
sem_t impress;
pthread_t *main_buffer, *sec_buffer;
int main_index = 0, sec_index = 0, MAX;

void show_trace(int file_type, long int id, int imp_time){
    printf("\n---------------------------------------\n");
    printf("\nArquivo sendo impresso: [%ld]\n", id);
    switch (file_type){
    case 0:
        printf("Tipo do arquivo: administrativo\n");
        break;
    case 1:
        printf("Tipo do arquivo: diverso\n");
        break; 
    }
    printf("Tempo estimado: %ds\n", imp_time);
    printf("\nArquivos no buffer principal\n");
    printf("--------\n");
    for(int i=0; i<MAX; i++){
        printf("Slot %d [%ld] \n", i, main_buffer[i]);
    }
    printf("\nArquivos no buffer secundario\n");
    printf("--------\n");
    for(int i=0; i<MAX; i++){
        printf("Slot %d [%ld] \n", i, sec_buffer[i]);
    }
    printf("\n---------------------------------------\n");
}

void *adm_thread(){
    pthread_t id = pthread_self();
    sem_wait(&sem_main);
    main_buffer[main_index] = id;
    int cur_index = main_index;
    if(main_index!=MAX){
        main_index++;
    }
    sem_post(&sem_main);

    sem_wait(&impress);
    srand(time(NULL)); 
    int imp_time = (rand() % 3)+1;
    show_trace(0, id, imp_time);
    sleep(imp_time);
    if(main_index!=0){
        main_index--;
    }
    for(int i=0; i<MAX-1; i++){
        main_buffer[i] = main_buffer[i+1];
        main_buffer[i+1] = 0;
    }

    sem_post(&impress);
    sem_post(&main_empty);
    return NULL;
}

void *div_thread(){
    pthread_t id = pthread_self();
    sem_wait(&sem_sec);
    sec_buffer[sec_index] = id;
    int cur_index = sec_index;
    if(sec_index!=MAX){
        sec_index++;
    }
    sem_post(&sem_sec);

    sem_wait(&impress);
    srand(time(NULL)); 
    int imp_time = (rand() % 3)+1;
    show_trace(1, id, imp_time);
    sleep(imp_time);
    if(sec_index!=0){
        sec_index--;
    }
    for(int i=0; i<MAX-1; i++){
        sec_buffer[i] = sec_buffer[i+1];
        sec_buffer[i+1] = 0;
    }

    sem_post(&impress);
    sem_post(&sec_empty);
    return NULL;
}

int main(int argc, char **argv){
    int QTD_ADM, QTD_DIVERSOS, thread_count = 0;
    int adm_count = 0, div_count = 0;
    pthread_t *all_threads;
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
    sem_init(&sem_sec, 0, 1);
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
    sem_destroy(&sem_main);
    sem_destroy(&sec_empty);
    sem_destroy(&sem_sec);
    sem_destroy(&impress);

    free(main_buffer);
    free(sec_buffer);
    free(all_threads);
}
