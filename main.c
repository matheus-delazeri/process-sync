#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    int *gen_time;
    int *imp_time;
} file_struct;

void *impress(void *file){
    file_struct *actual_file = file;
    pthread_t id = pthread_self();
    int gen_time = *(actual_file->gen_time);
    int imp_time = *(actual_file->imp_time);
    int *return_args = malloc(sizeof (int));
    return_args[0] = id;

    free(actual_file);
    return return_args;
}

int main(int argc, char **argv){
    int i, imp_time, gen_time, MAX, QTD_ADM, QTD_DIVERSOS;
    pthread_t *adm_vect, *div_vect;
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

    adm_vect = (pthread_t*)malloc(QTD_ADM * sizeof(pthread_t));
    for(i=0; i<QTD_ADM; i++){
        gen_time = rand()%6;
        imp_time = rand()%6;
        file_struct *file = malloc(sizeof *file);
        file->gen_time = &gen_time;
        file->imp_time = &imp_time;
        if(pthread_create(&adm_vect[i], NULL, impress, file)){
            free(file);
        }
    }
    for(i=0; i<QTD_ADM; i++){
        void *status = NULL;
        pthread_join(adm_vect[i], &status);
        printf("Id: %d\n", ((int*)status)[0]); 
    }

    div_vect = (pthread_t*)malloc(QTD_DIVERSOS * sizeof(pthread_t));


    free(adm_vect);
    free(div_vect);
}
