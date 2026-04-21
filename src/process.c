// Grupo 1
// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "process.h"
#include "controller.h"
#include "sensor.h"
#include "server.h"
#include <sys/wait.h>
#include <unistd.h>

int launch_process(enum process_type process_type, int process_id, struct info_container *info, struct buffers *buffs) {
  int pid = fork();

  // Erro no fork()
  if (pid < 0) {
    return -1;
  };

  // Processo filho
  if (pid == 0) {
    int ret;
    switch (process_type) {
    case SENSOR_PROCESS:
      ret = execute_sensor(process_id, info, buffs);
      break;
    case CONTROLLER_PROCESS:
      ret = execute_controller(process_id, info, buffs);
      break;
    case SERVER_PROCESS:
      ret = execute_server(process_id, info, buffs);
      break;
    default:
      _exit(1);
    }
    destroy_dynamic_memory_structs(info, buffs);
    _exit(ret);
  }

  // Proceso pai devolve o pid do processo filho
  return pid;
};

int wait_process(int process_id) {
  int status;
  if (waitpid(process_id, &status, 0) == -1) {
    return -1;
  }
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return -1;
};
