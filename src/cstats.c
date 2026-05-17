#include "cstats.h"
#include <stdio.h>
#include <unistd.h>

void imprimeTextoSimples(FILE *ficheiroSaida, char *string) {
    fprintf(ficheiroSaida, "%s", string);
    printf("%s", string);
}

void write_stats(struct info_container *info) {
  FILE *ficheiroStatistics = fopen(info->statistics_filename, "w");
  imprimeTextoSimples(ficheiroStatistics, "===== FINAL STATISTICS =====\n");

  fprintf(ficheiroStatistics, "Main PID %d: \n", getpid());
  printf("Main PID: %d\n", getpid());

  fprintf(ficheiroStatistics, "n_sensores=%d, n_servers=%d, buffers_size=%d\n", info->n_sensors, info->n_servers, info->buffers_size);
  printf("n_sensors=%d, n_servers=%d, buffers_size=%d\n", info->n_sensors, info->n_servers, info->buffers_size);

  int total_generated = 0;
  for (int i = 0; i < info->n_sensors; i++) {
    total_generated += info->num_generated_measurements[i];
  }
  fprintf(ficheiroStatistics, "total_measurements: %d\n", total_generated);
  printf("total_measurements: %d\n", total_generated);

  imprimeTextoSimples (ficheiroStatistics, "Sensors:\n");

  for (int i = 0; i < info->n_sensors; i++) {
    fprintf(ficheiroStatistics, "  Sensor %d (PID %d) : generated %d\n", i, info->sensors_pids[i], info->num_generated_measurements[i]);
    printf("  Sensor %d (PID %d) : generated %d\n", i, info->sensors_pids[i], info->num_generated_measurements[i]);
  }

  imprimeTextoSimples (ficheiroStatistics, "Controllers:\n");

  for (int i = 0; i < info->n_sensors; i++) {
    fprintf(ficheiroStatistics, "  controller = %d pid = %d\n", i, info->controllers_pids[i]);
    printf("  controller = %d pid = %d\n", i, info->controllers_pids[i]);
  }
  imprimeTextoSimples (ficheiroStatistics, "Servers:\n");

  for (int i = 0; i < info->n_servers; i++) {
    fprintf(ficheiroStatistics, "  Server %d (PID %d) : estimates=%d\n", i, info->servers_pids[i], info->num_estimates[i]);
    printf("  Server %d (PID %d) : estimates=%d\n", i, info->servers_pids[i], info->num_estimates[i]);
  }
  
  fprintf(ficheiroStatistics, "Last logged ID %d:\n", *(info->last_logged_id));
  printf("Last logged ID: %d\n", *(info->last_logged_id));

  imprimeTextoSimples(ficheiroStatistics, "============================");

  fclose(ficheiroStatistics);
};
