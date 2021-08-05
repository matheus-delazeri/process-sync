#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

sem_t main_empty, sec_empty;
sem_t sem_main, sem_sec;
sem_t impress_adm, impress_div;
pthread_t *main_buffer, *sec_buffer;
int main_index = 0, sec_index = 0, MAX, exec_time = 0, adm_ver = 0;

void show_trace(int file_type, long int id, int imp_time){
    printf("\n\t-------------------------------------------\n");
    printf("\n\tArquivo sendo impresso: [%ld]\n", id);
    switch (file_type){
    case 0:
        printf("\tTipo do arquivo: administrativo\n");
        break;
    case 1:
        printf("\tTipo do arquivo: diverso\n");
        break; 
    }
    printf("\tTempo estimado: %ds\n", imp_time);
    printf("\n\tArquivos no buffer principal\n");
    printf("\t-----------\n");
    for(int i=0; i<MAX; i++){
        printf("\tSlot %d - [%ld] \n", i, main_buffer[i]);
    }
    printf("\n\tArquivos no buffer secundario\n");
    printf("\t-----------\n");
    for(int i=0; i<MAX; i++){
        printf("\tSlot %d - [%ld] \n", i, sec_buffer[i]);
    }
}
int buffer_empty(int buffer){
    int ver = 1;
    switch (buffer){
    case 0:
        for(int i=0; i<MAX; i++){
            if(main_buffer[i]!=0){
                ver = 0;
                break;
            }
        }
        break;
    case 1:
        for(int i=0; i<MAX; i++){
            if(sec_buffer[i]!=0){
                ver = 0;
                break;
            }
        }
        break;
    }
    return ver;
}

void *adm_thread(){
    pthread_t id = pthread_self();
    sem_wait(&main_empty);
    sem_wait(&sem_main);
    main_buffer[main_index] = id;
    if(main_index!=MAX){
        main_index++;
    }
    sem_post(&sem_main);

    sem_wait(&impress_adm);
    adm_ver++;
    srand(time(NULL)); 
    int imp_time = (rand() % 3)+1;
    exec_time += imp_time;
    show_trace(0, id, imp_time);
    sleep(imp_time);
    if(main_index!=0){
        main_index--;
    }
    if(MAX!=1){
        for(int i=0; i<MAX-1; i++){
                main_buffer[i] = main_buffer[i+1];
                main_buffer[i+1] = 0;
        }
    }

    printf("\n\tO arquivo [%ld] foi impresso com sucesso!\n", id);
    printf("\n\t-------------------------------------------\n");
    sem_post(&main_empty);
    if((adm_ver==2 && buffer_empty(1)==0)||buffer_empty(0)==1){
        sem_post(&impress_div);
    }else{
        sem_post(&impress_adm);
    }

}

void *div_thread(){
    pthread_t id = pthread_self();
    sem_wait(&sec_empty);
    sem_wait(&sem_sec);
    sec_buffer[sec_index] = id;
    if(sec_index!=MAX){
        sec_index++;
    }
    sem_post(&sem_sec);

    sem_wait(&impress_div);
    srand(time(NULL)); 
    int imp_time = (rand() % 3)+1;
    exec_time += imp_time;
    show_trace(1, id, imp_time);
    sleep(imp_time);
    if(sec_index!=0){
        sec_index--;
    }
    if(MAX!=1){
        for(int i=0; i<MAX-1; i++){
            sec_buffer[i] = sec_buffer[i+1];
            sec_buffer[i+1] = 0;
        }
    }
    adm_ver = 0;

    printf("\n\tO arquivo [%ld] foi impresso com sucesso!\n", id);
    printf("\n\t-------------------------------------------\n");
    sem_post(&sec_empty);
    if(buffer_empty(0)==0){
        sem_post(&impress_adm);
    }else{
        sem_post(&impress_div);
    }

}

int main(int argc, char **argv){
    int QTD_ADM, QTD_DIVERSOS, thread_count = 0;
    int adm_count = 0, div_count = 0, imp_div_value;
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
    imp_div_value = QTD_ADM!=0 ? 0 : 1;

    sem_init(&main_empty, 0, MAX);
    sem_init(&sem_main, 0, 1);
    sem_init(&sec_empty, 0, MAX);
    sem_init(&sem_sec, 0, 1);
    sem_init(&impress_adm, 0, 1);
    sem_init(&impress_div, 0, imp_div_value);
    main_buffer = (pthread_t*)malloc(MAX * sizeof(pthread_t));
    sec_buffer = (pthread_t*)malloc(MAX * sizeof(pthread_t));
    all_threads = (pthread_t*)malloc((QTD_ADM+QTD_DIVERSOS) * sizeof(pthread_t));

    while(adm_count != QTD_ADM){
        pthread_create(&all_threads[thread_count], NULL, adm_thread, NULL);
        adm_count++;
        thread_count++;
    }
    while(div_count != QTD_DIVERSOS){
        pthread_create(&all_threads[thread_count], NULL, div_thread, NULL);
        div_count++;
        thread_count++;
    }
    for(int i=0; i<(QTD_ADM+QTD_DIVERSOS); i++){
        pthread_join(all_threads[i], NULL);
    }
    printf("\n\tArquivos no buffer principal\n");
    printf("\t-----------\n");
    for(int i=0; i<MAX; i++){
        printf("\tSlot %d - [%ld] \n", i, main_buffer[i]);
    }
    printf("\n\tArquivos no buffer secundario\n");
    printf("\t-----------\n");
    for(int i=0; i<MAX; i++){
        printf("\tSlot %d - [%ld] \n", i, sec_buffer[i]);
    }
    printf("\n\t-------------------------------------------\n");
    printf("\tArquivos administrativos impressos: [%d]\n", QTD_ADM);
    printf("\tArquivos diversos impressos: [%d]\n", QTD_DIVERSOS);
    printf("\tTotal: [%d]\n", QTD_ADM+QTD_DIVERSOS);
    printf("\tTempo de execucao: %ds\n", exec_time);
    printf("\t-------------------------------------------\n");

    sem_destroy(&main_empty);
    sem_destroy(&sem_main);
    sem_destroy(&sec_empty);
    sem_destroy(&sem_sec);
    sem_destroy(&impress_adm);
    sem_destroy(&impress_div);

    free(main_buffer);
    free(sec_buffer);
    free(all_threads);
}