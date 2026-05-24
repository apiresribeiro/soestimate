#include "clog.h"
#include "ctime.h"
#include <stdio.h>
#include <stdlib.h>

void clean_log(const char *log_filename) {
  FILE *file = fopen(log_filename, "w");

  if (file == NULL) {
    perror("clean_log");
    exit(1);
  }

  fclose(file);
}

void write_log(struct info_container *info, const char *command) {
  sem_wait(info->sems->log_mutex);

  FILE *file = fopen(info->log_filename, "a"); // a -> modo append
  if (file == NULL) {
    perror("write_log");
    sem_post(info->sems->log_mutex);
    exit(1);
  }

  char time[32];
  format_current_time(time, sizeof(time));

  fprintf(file, "%s %s\n", time, command);
  fclose(file);

  sem_post(info->sems->log_mutex);
}

void write_server_result(struct info_container *info, int server_id, int m_id, double estimate, double elapsed_time) {
  sem_wait(info->sems->log_mutex);

  if (*(info->last_logged_id) >= m_id) {
    sem_post(info->sems->log_mutex);
    return;
  }

  FILE *file = fopen(info->log_filename, "a"); // a -> modo append
  if (file == NULL) {
    perror("write_server_result");
    sem_post(info->sems->log_mutex);
    exit(1);
  }

  char time[32];
  format_current_time(time, sizeof(time));

  fprintf(file, "%s result %d server %d estimate %.4f elapsed %.3f\n", time, m_id, server_id, estimate, elapsed_time);
  fclose(file);

  *(info->last_logged_id) = m_id;

  sem_post(info->sems->log_mutex);
}
