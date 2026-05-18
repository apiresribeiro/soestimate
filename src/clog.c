#include "clog.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void reescrever_log_existente(const char *log_filename) {
  FILE *ficheiro_rescrever = fopen(log_filename, "w");
}

char *get_time() {
  char *buffer = malloc(32 * sizeof(char));
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm *timeinfo = localtime(&ts.tv_sec);
  char data_hora[32];
  strftime(data_hora, sizeof(data_hora), "%Y%m%d %H:%M:%S", timeinfo);
  int milisegundos = ts.tv_nsec / 1000000;
  snprintf(buffer, 32, "%s.%03d", data_hora, milisegundos);
  return buffer;
}

void write_log(struct info_container *info, const char *comando) {
  // sem_wait(info->sems->log_mutex);
  FILE *f_log = fopen(info->log_filename, "a"); // a -> modo append
  if (f_log != NULL) {
    char *time = get_time();
    fprintf(f_log, "%s %s\n", time, comando);
    free(time);
    fclose(f_log);
  }
  // sem_post(info->sems->log_mutex);
}

void write_server_result(struct info_container *info, int server_id, int m_id, double estimate, double elapsed_time) {
  FILE *f_log = fopen(info->log_filename, "a"); // a -> modo append
  char *time = get_time();
  fprintf(f_log, "%s result %d estimate %d elapsed %f\n", time, m_id, server_id, estimate, elapsed_time);
  free(time);
  fclose(f_log);
}
