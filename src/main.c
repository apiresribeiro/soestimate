// Grupo 1
// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "main.h"
#include "clog.h"
#include "cstats.h"
#include "memory.h"
#include "process.h"
#include "synchronization.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void main_args(int argc, char *argv[], struct info_container *info) {
  if (argc == 3) {
    FILE *ficheirosSaida = fopen(argv[2], "r");
    FILE *ficheiroEntrada = fopen(argv[1], "r");
    if (ficheiroEntrada == NULL || ficheirosSaida == NULL) {
      printf("Impossivel abrir ficheiros");
      exit(1);
    }
    fscanf(ficheiroEntrada, "%d\n%d\n%d\n", &info->n_sensors, &info->n_servers, &info->buffers_size);
    fscanf(ficheirosSaida, "%s\n%s\n%d\n", info->log_filename, info->statistics_filename, &info->period);

    fclose(ficheiroEntrada);
    fclose(ficheirosSaida);
  } else {
    perror("main_args");
    exit(1);
  }
};

void create_dynamic_memory_structs(struct info_container *info, struct buffers *buffs) {
  // PIDS buffers
  info->sensors_pids = allocate_dynamic_memory(info->n_sensors * sizeof(int));
  info->controllers_pids = allocate_dynamic_memory(info->n_sensors * sizeof(int));
  info->servers_pids = allocate_dynamic_memory(info->n_servers * sizeof(int));

  // Main-sensor buffer
  buffs->buff_main_sensors = allocate_dynamic_memory(sizeof(struct circ_buffer)); // Aloca o espaço para o buffer circular

  // Sensor-controller buffer
  buffs->buff_sensors_controllers = allocate_dynamic_memory(sizeof(struct ra_buffer));

  // Controller-server buffer
  buffs->buff_controllers_servers = allocate_dynamic_memory(sizeof(struct ra_buffer));
};

void create_shared_memory_structs(struct info_container *info, struct buffers *buffs) {
  int size_measurements = info->buffers_size * sizeof(MeasurementInfo);
  int size_circ_ptrs = sizeof(struct pointers);
  int size_ra_ptrs = info->buffers_size * sizeof(int);

  // Buffer memória partilhada main-sensor
  buffs->buff_main_sensors->buffer = create_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, size_measurements);
  buffs->buff_main_sensors->ptrs = create_shared_memory(ID_SHM_MAIN_SENSORS_PTR, size_circ_ptrs);
  for (int i = 0; i < info->buffers_size; i++) {
    buffs->buff_main_sensors->buffer[i].counter_sensors = info->n_sensors;
    buffs->buff_main_sensors->buffer[i].counter_servers = info->n_servers;
  }

  // Buffer memoria partilhada sensor-controller
  buffs->buff_sensors_controllers->buffer = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, size_measurements);
  buffs->buff_sensors_controllers->ptrs = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, size_ra_ptrs);
  for (int i = 0; i < info->buffers_size; i++) {
    buffs->buff_sensors_controllers->buffer[i].counter_sensors = info->n_sensors;
    buffs->buff_sensors_controllers->buffer[i].counter_servers = info->n_servers;
  }

  // buffer memoria partilhada controller-server
  buffs->buff_controllers_servers->buffer = create_shared_memory(ID_SHM_CONTROLLERS_SERVERS_VERSION, size_measurements);
  buffs->buff_controllers_servers->ptrs = create_shared_memory(ID_SHM_CONTROLLERS_SERVERS_LATEST, size_ra_ptrs);
  for (int i = 0; i < info->buffers_size; i++) {
    buffs->buff_controllers_servers->buffer[i].counter_sensors = info->n_sensors;
    buffs->buff_controllers_servers->buffer[i].counter_servers = info->n_servers;
  }

  info->num_generated_measurements = create_shared_memory(ID_SHM_SENSORS_GENERATED, info->n_sensors * sizeof(int));
  info->num_invalid_measurements = create_shared_memory(ID_SHM_SENSORS_INVALID, info->n_sensors * sizeof(int));
  info->num_estimates = create_shared_memory(ID_SHM_SERVERS_PROCESSED, info->n_servers * sizeof(int));
  info->total_measurements = create_shared_memory(ID_SHM_TOTAL_MEASUREMENTS, sizeof(int));
  info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));
  info->last_logged_id = create_shared_memory(ID_SHM_LAST_LOGGED_ID, sizeof(int));
};

void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs) {
  deallocate_dynamic_memory(info->sensors_pids);
  deallocate_dynamic_memory(info->controllers_pids);
  deallocate_dynamic_memory(info->servers_pids);
  deallocate_dynamic_memory(buffs->buff_main_sensors);
  deallocate_dynamic_memory(buffs->buff_sensors_controllers);
  deallocate_dynamic_memory(buffs->buff_controllers_servers);
};

void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs) {
  int size_measurements = info->buffers_size * sizeof(MeasurementInfo);
  int size_circ_ptrs = sizeof(struct pointers);
  int size_ra_ptrs = info->buffers_size * sizeof(int);

  // Main-sensor
  destroy_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, buffs->buff_main_sensors->buffer, size_measurements);
  destroy_shared_memory(ID_SHM_MAIN_SENSORS_PTR, buffs->buff_main_sensors->ptrs, size_circ_ptrs);

  // Sensor Controller
  destroy_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, buffs->buff_sensors_controllers->buffer, size_measurements);
  destroy_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, buffs->buff_sensors_controllers->ptrs, size_ra_ptrs);

  // Controller-server
  destroy_shared_memory(ID_SHM_CONTROLLERS_SERVERS_VERSION, buffs->buff_controllers_servers->buffer, size_measurements);
  destroy_shared_memory(ID_SHM_CONTROLLERS_SERVERS_LATEST, buffs->buff_controllers_servers->ptrs, size_ra_ptrs);

  destroy_shared_memory(ID_SHM_SENSORS_GENERATED, info->num_generated_measurements, info->n_sensors * sizeof(int));
  destroy_shared_memory(ID_SHM_SENSORS_INVALID, info->num_invalid_measurements, info->n_sensors * sizeof(int));
  destroy_shared_memory(ID_SHM_SERVERS_PROCESSED, info->num_estimates, info->n_servers * sizeof(int));
  destroy_shared_memory(ID_SHM_TOTAL_MEASUREMENTS, info->total_measurements, sizeof(int));
  destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));
  destroy_shared_memory(ID_SHM_LAST_LOGGED_ID, info->last_logged_id, sizeof(int));
};

void create_processes(struct info_container *info, struct buffers *buffs) {
  // Sensor
  for (int i = 0; i < info->n_sensors; i++) {
    info->sensors_pids[i] = launch_process(SENSOR_PROCESS, i, info, buffs);
  }
  // Controller
  for (int i = 0; i < info->n_sensors; i++) {
    info->controllers_pids[i] = launch_process(CONTROLLER_PROCESS, i, info, buffs);
  }
  // Server
  for (int i = 0; i < info->n_servers; i++) {
    info->servers_pids[i] = launch_process(SERVER_PROCESS, i, info, buffs);
  }
};

void user_interaction(struct info_container *info, struct buffers *buffs) {
  clean_log(info->log_filename);
  char input[16];
  while (!(*info->terminate)) {
    printf("\n[Main] Enter command: ");
    fflush(stdout);
    if (fgets(input, 15, stdin) == NULL) {
      *info->terminate = 1;
      break;
    }

    if (strncmp(input, "measure", 7) == 0) {
      write_log(info, "measure");
      MeasurementInfo req;
      req.state = REQUEST;
      req.m_id = *info->total_measurements + 1;
      req.sensor_id = -1;
      req.controller_id = -1;
      req.value = 0.0;
      req.counter_sensors = info->n_sensors;
      req.counter_servers = info->n_servers;

      int ok = false;
      while (!ok && !(*info->terminate)) {
        sem_wait(info->sems->main_sensors->free_space);
        sem_wait(info->sems->main_sensors->mutex);
        ok = write_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, &req);
        sem_post(info->sems->main_sensors->mutex);

        if (ok) {
          for (int i = 0; i < info->n_sensors; i++) {
            sem_post(info->sems->main_sensors->unread);
          }
        } else {
          sem_post(info->sems->main_sensors->free_space);
        }
      }

      if (ok) {
        *info->total_measurements = req.m_id;
        printf("[Main] Measurement cycle started with m_id=%d.\n", req.m_id);
      }

    } else if (strncmp(input, "read", 4) == 0) {
      int m_id = -1;
      if (sscanf(input, "read %d", &m_id) == 1) {
        char read_command[32];
        snprintf(read_command, sizeof(read_command), "read %d", m_id);
        write_log(info, read_command);
        read_estimate(info, m_id);
      } else {
        printf("[Main] Invalid command format. Use 'read <m_id>'.\n");
      }
    } else if (strncmp(input, "stat", 4) == 0) {
      write_log(info, "stat");
      print_stat(info);
    } else if (strncmp(input, "help", 4) == 0) {
      write_log(info, "help");
      help();
    } else if (strncmp(input, "end", 3) == 0) {
      write_log(info, "end");
      *info->terminate = 1;
    } else {
      printf("[Main] Unknown command. Type 'help'.\n");
    }
  }
};

void write_final_statistics(struct info_container *info) {
  write_stats(info);
};
void end_execution(struct info_container *info, struct buffers *buffs) {
  *info->terminate = 1;
  wakeup_processes(info);
  wait_processes(info);
  write_final_statistics(info);
  destroy_shared_memory_structs(info, buffs);
  destroy_dynamic_memory_structs(info, buffs);
  destroy_all_semaphores(info->sems);
};

void wait_processes(struct info_container *info) {
  for (int i = 0; i < info->n_sensors; i++) {
    wait_process(info->sensors_pids[i]);
  }

  for (int i = 0; i < info->n_sensors; i++) {
    wait_process(info->controllers_pids[i]);
  }

  for (int i = 0; i < info->n_servers; i++) {
    wait_process(info->servers_pids[i]);
  }
};

void read_estimate(struct info_container *info, int m_id) {
  if (m_id <= 0) {
    printf("[Main] Invalid cycle id. Use a value >= 1.\n");
    return;
  }

  sem_wait(info->sems->log_mutex);

  FILE *file = fopen(info->log_filename, "r");
  if (file == NULL) {
    sem_post(info->sems->log_mutex);
    printf("[Main] Could not open log file '%s'.\n", info->log_filename);
    return;
  }

  char line[256];
  int found = 0;
  int server_id = -1;
  double estimate = 0.0;
  double elapsed = 0.0;

  while (fgets(line, sizeof(line), file) != NULL) {
    int parsed_m_id;
    int parsed_server_id;
    double parsed_estimate;
    double parsed_elapsed;

    int parsed = sscanf(line,
                        "%*s %*s result %d server %d estimate %lf elapsed %lf",
                        &parsed_m_id,
                        &parsed_server_id,
                        &parsed_estimate,
                        &parsed_elapsed);

    if (parsed == 4 && parsed_m_id == m_id) {
      found = 1;
      server_id = parsed_server_id;
      estimate = parsed_estimate;
      elapsed = parsed_elapsed;
    }
  }

  fclose(file);
  sem_post(info->sems->log_mutex);

  if (found) {
    printf("[Main] estimate for cycle %d = %.4f (server %d, elapsed %.3f)\n", m_id, estimate, server_id, elapsed);
  } else {
    printf("[Main] no estimate found for cycle %d\n", m_id);
  }
};

void print_stat(struct info_container *info) {
  printf("[Main] Current SOestimate state:\n");
  printf("[Main] n_sensors=%d, n_servers=%d, buffers_size=%d\n", info->n_sensors, info->n_servers, info->buffers_size);
  printf("[Main] terminate=%d, total_measurements=%d\n", *info->terminate, *info->total_measurements);

  // Sensors
  printf("[Main] Sensors:\n");
  for (int i = 0; i < info->n_sensors; i++) {
    printf("[Main]  sensor=%d pid=%d generated=%d\n", i, info->sensors_pids[i], info->num_generated_measurements[i]);
  }

  // Controllers
  printf("[Main] Controllers:\n");
  for (int i = 0; i < info->n_sensors; i++) {
    printf("[Main]  controller=%d pid=%d\n", i, info->controllers_pids[i]);
  }

  // Servers
  printf("[Main] Servers:\n");
  for (int i = 0; i < info->n_servers; i++) {
    printf("[Main]  server=%d pid=%d estimates=%d\n", i, info->servers_pids[i], info->num_estimates[i]);
  }
};

void help(void) {
  printf("[Main] Available operations:\n");
  printf("[Main]  measure - start a measurement cycle with one measurement per sensor.\n");
  printf("[Main]  read <cycle_id> - read the estimate for a cycle.\n");
  printf("[Main]  stat - print the current system state.\n");
  printf("[Main]  help - show this menu.\n");
  printf("[Main]  end - terminate SOestimate.\n");
};

int main(int argc, char *argv[]) {
  struct info_container info;

  struct buffers buffs;
  main_args(argc, argv, &info);
  create_dynamic_memory_structs(&info, &buffs);
  create_shared_memory_structs(&info, &buffs);
  info.sems = create_all_semaphores(info.buffers_size);
  create_processes(&info, &buffs);

  printf("[Main] Available operations:\n");
  printf("[Main]  measure\n");
  printf("[Main]  read <cycle_id>\n");
  printf("[Main]  stat\n");
  printf("[Main]  help\n");
  printf("[Main]  end\n\n");

  user_interaction(&info, &buffs);
  end_execution(&info, &buffs);

  return 0;
};
