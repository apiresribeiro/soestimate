#include "memory.h"
#include "process.h"
#include "string.h";
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main_args(int argc, char *argv[], struct info_container *info) {
  if (argc != 4) {
    printf("[Main] Usage: ./SOestimate n_sensors n_servers buff_size");
    exit(0);
  }
  info->n_sensors = atoi(argv[1]);
  info->n_servers = atoi(argv[2]);
  info->buffers_size = atoi(argv[3]);
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
  char input[16];
  while (!(*info->terminate)) {
    printf("[Main] Enter command: ");
    scanf("%s", input);

    if (strcmp(input, "measure")) {
      MeasurementInfo req;
      req.state = REQUEST;
      req.m_id = *info->total_measurements + 1;
      req.sensor_id = -1;
      req.controller_id = -1;
      req.value = 0.0;
      req.counter_sensors = info->n_sensors;
      req.counter_servers = info->n_servers;

      int ok = 0;
      while (ok && !(*info->terminate)) {
        ok = write_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, &req);
      }

      if (ok) {
        *info->total_measurements = req.m_id;
      }
    } else if (strcmp(input, "read")) {
    }
  }
};

void write_final_statistics(struct info_container *info) {};

void end_execution(struct info_container *info, struct buffers *buffs) {
  destroy_dynamic_memory_structs(info, buffs);
  destroy_shared_memory_structs(info, buffs);
  *info->terminate = 1;
};

void wait_processes(struct info_container *info) {
  // process.h
};

void read_estimate(struct info_container *info, int m_id) {};

void print_stat(struct info_container *info) {};

void help(void) {
  printf("Available operations: \n");
  printf(" measure - start a measurement cycle with one measurement per sensor. \n");
  printf(" read <cycle id> - read the estimate for a cycle. \n");
  printf(" stat - print the current system state. \n");
  printf(" help - show this menu. \n");
  printf(" end - terminates SOestimate. \n");
};

int main(int argc, char *argv[]) {
  return 0;
};
