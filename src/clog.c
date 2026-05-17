#include "clog.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

void reescrever_log_existente(const char*log_filename){
    FILE *ficheiro_rescrever = fopen(log_filename, "w");
}

char* get_time(){
    char *buffer = malloc (32 * sizeof(char));
    struct timespec ts; 
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm *timeinfo = localtime(&ts.tv_sec);
    char data_hora[32];
    strftime(data_hora, sozeof(data_hora), "%Y%m%d %H:%M:%S", timeinfo);
    int milisegundos = ts.tv_nsec / 1000000;
    snprintf(buffer, 32, "%s.%03d", data_hora, milisegundos); 
    return buffer;
}

void print_log(struct info_container *info, const char *comando) {
    //sem_wait(info->sems->log_mutex); 
    FILE *f_log = fopen(info->log_filename, "a"); // a -> modo append
    if (f_log != NULL) {
        char *tempo_atual = get_time();
        fprintf(f_log, "%s %s\n", tempo_atual, comando);
        free(tempo_atual);
        fclose(f_log);  
    }    
    //sem_post(info->sems->log_mutex); 
}

void print_server_result(struct info_container *info, int server_id, int m_id, double estimate, double elapsed_time){
    FILE *f_log = fopen(info->log_filename, "a"); // a -> modo append
    fprintf (f_log, "%s result %d estimate %d elapsed %d", get_time(), m_id, server_id, estimate, elapsed_time);
}