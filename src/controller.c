#include "controller.h"
#include "main.h"
#include "memory.h"
#include "random_measurement.h"
#include "stdbool.h"

int execute_controller(int controller_id, struct info_container *info, struct buffers *buffs) {
  // Estrutura local para guardar a medição lida do buffer partilhado.
  MeasurementInfo m;
  // Loop principal do controller: continua até a flag de terminação ficar ativa.
  while (!(*info->terminate)) {
    // Tenta receber a próxima medição destinada a este controller.
    controller_receive_measurement(&m, controller_id, info, buffs);

    // Se a terminação foi ativada enquanto esperava, sai imediatamente.
    if (*info->terminate) {
      break;
    }

    // m_id = -1 significa que não foi lida uma nova medição válida nesta tentativa.
    if (m.m_id == -1) {
      continue;
    }

    // Valida a medição
    controller_process_measurement(&m, controller_id, info);
    // Envia a medição processada para o buffer controller -> server.
    controller_send_measurement(&m, info, buffs);
  }
  // Terminação normal do processo controller.
  return 0;
};

void controller_receive_measurement(MeasurementInfo *m, int controller_id, struct info_container *info, struct buffers *buffs) {
  m->m_id = -1;
  // Tentar recever do buffer sempre que a flag permitir
  while (m->m_id == -1 && !(*info->terminate)) {
    read_sensor_controller_buffer(buffs->buff_sensors_controllers, controller_id, info->buffers_size, m);
  }
};

void controller_process_measurement(MeasurementInfo *m, int controller_id, struct info_container *info) {
  // Coloca o id do controllador no struct
  m->controller_id = controller_id;
  // Se for inválido, então incrementar também o número de medições inválidas
  if (!is_valid_measurement(m->value)) {
    info->num_invalid_measurements[controller_id]++;
    m->state = INVALID;
  } else {
    // Coloca o estado a válido
    m->state = VALID;
  }
};

void controller_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
  bool success = false;
  // Tentar sempre escrever no buffer sempre que a flag terminate não tiver ativa
  while (!success && !(*info->terminate)) {
    success = write_controller_servers_buffer(buffs->buff_controllers_servers, info->buffers_size, m);
  }
};
