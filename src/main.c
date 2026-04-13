#include "memory.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main_args(int argc, char *argv[], struct info_container *info) {
  if (argc != 3) {
    printf("[Main] Usage: ./SOestimate n_sensors n_servers buff_size");
    exit(0);
  }
  info->n_sensors = atoi(argv[1]);
  info->n_servers = atoi(argv[2]);
  info->buffers_size = atoi(argv[3]);
};

void create_dynamic_memory_structs(struct info_container *info, struct buffers *buffs) {
  int tamanhoBuffer = info->buffers_size * sizeof(MeasurementInfo);

  // Main-sensor buffer
  buffs->buff_main_sensors = allocate_dynamic_memory(sizeof(struct circ_buffer));    // Aloca o espaço para o buffer circular
  buffs->buff_main_sensors->buffer = allocate_dynamic_memory(tamanhoBuffer);         // Aloca o espaço para os dados (array de measurementInfo)
  buffs->buff_main_sensors->ptrs = allocate_dynamic_memory(sizeof(struct pointers)); // Aloca espaço para os pointers do buffer cicrular
  // Inicializa os pointers a 0
  buffs->buff_main_sensors->ptrs->in = 0;
  buffs->buff_main_sensors->ptrs->out = 0;

  // Sensor-controller buffer
  buffs->buff_sensors_controllers = allocate_dynamic_memory(sizeof(struct ra_buffer));
  buffs->buff_sensors_controllers->buffer = allocate_dynamic_memory(tamanhoBuffer);
  buffs->buff_sensors_controllers->ptrs = allocate_dynamic_memory(sizeof(struct pointers));

  // Controller-server buffer
  buffs->buff_controllers_servers = allocate_dynamic_memory(sizeof(struct ra_buffer));
  buffs->buff_controllers_servers->buffer = allocate_dynamic_memory(tamanhoBuffer);
  buffs->buff_controllers_servers->ptrs = allocate_dynamic_memory(sizeof(struct pointers));
};

void create_shared_memory_structs(struct info_container *info, struct buffers *buffs) {
  int tamanhoBuffer = info->buffers_size * sizeof(MeasurementInfo);
  int tamanhoRA = sizeof(struct ra_buffer) + tamanhoBuffer;
  int tamanhoCirc = sizeof(struct circ_buffer) + tamanhoBuffer;

  // Buffer memória partilhada main-sensor
  buffs->buff_main_sensors->buffer = create_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, tamanhoCirc);
  buffs->buff_main_sensors->ptrs = create_shared_memory(ID_SHM_MAIN_SENSORS_PTR, tamanhoCirc);
  for (int i = 0; i < info->buffers_size; i++) {
    buffs->buff_main_sensors->buffer[i].counter_sensors = info->n_sensors;
    buffs->buff_main_sensors->buffer[i].counter_servers = info->n_servers;
  }

  // Buffer memoria partilhada sensor-controller
  buffs->buff_sensors_controllers->buffer = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, tamanhoRA);
  buffs->buff_sensors_controllers->ptrs = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, tamanhoRA);
  for (int i = 0; i < info->buffers_size; i++) {
    buffs->buff_sensors_controllers->buffer[i].counter_sensors = info->n_sensors;
    buffs->buff_sensors_controllers->buffer[i].counter_servers = info->n_servers;
  }

  // buffer memoria partilhada controller-server
  buffs->buff_controllers_servers->buffer = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, tamanhoRA);
  buffs->buff_controllers_servers->ptrs = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, tamanhoRA);
  for (int i = 0; i < info->buffers_size; i++) {
    buffs->buff_controllers_servers->buffer[i].counter_sensors = info->n_sensors;
    buffs->buff_controllers_servers->buffer[i].counter_servers = info->n_servers;
  }
};

// FIXME: Isto está mal
void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs) {
  deallocate_dynamic_memory(buffs->buff_main_sensors->buffer);
  deallocate_dynamic_memory(buffs->buff_main_sensors->ptrs);
  deallocate_dynamic_memory(buffs->buff_main_sensors);
};

void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs) {
  char nome_buffer[64]; // buffer para guardar o nome
  uid_t uid = getuid();
  int tamanhoBuffer = info->buffers_size * sizeof(MeasurementInfo);
  int tamanhoTotal = sizeof(struct ra_buffer) + tamanhoBuffer;

  snprintf(nome_buffer, sizeof(nome_buffer), "%d_scname", uid);
  destroy_shared_memory(nome_buffer, buffs->buff_sensors_controllers, tamanhoTotal);

  snprintf(nome_buffer, sizeof(nome_buffer), "%d_csname", uid);
  destroy_shared_memory(nome_buffer, buffs->buff_controllers_servers, tamanhoTotal);
};

void create_processes(struct info_container *info, struct buffers *buffs) {
  // process.h
};

void user_interaction(struct info_container *info, struct buffers *buffs) {};

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
