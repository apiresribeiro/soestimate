// Grupo 1
// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "csettings.h"
#include "main.h"
#include <stdio.h>

int read_args_file(const char *args_filename, struct info_container *info) {
  if (args_filename == NULL || info == NULL) {
    return -1;
  }

  FILE *file = fopen(args_filename, "r");
  if (file == NULL) {
    perror("read_args_file");
    return -1;
  }

  int parsed = fscanf(file, "%d\n%d\n%d\n", &info->n_sensors, &info->n_servers, &info->buffers_size);
  fclose(file);

  if (parsed != 3) {
    fprintf(stderr, "Invalid args file format\n");
    return -1;
  }

  return 0;
}

int read_settings_file(const char *settings_filename, struct info_container *info) {
  if (settings_filename == NULL || info == NULL) {
    return -1;
  }

  FILE *file = fopen(settings_filename, "r");
  if (file == NULL) {
    perror("read_settings_file");
    return -1;
  }

  int parsed = fscanf(file, "%99s\n%99s\n%u\n", info->log_filename, info->statistics_filename, &info->period);
  fclose(file);

  if (parsed != 3) {
    fprintf(stderr, "Invalid settings file format\n");
    return -1;
  }

  return 0;
}
