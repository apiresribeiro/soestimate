#include "controller.h"
#include "main.h"
#include "memory.h"
#include "random_measurement.h"
#include "stdbool.h"

int execute_controller(int controller_id, struct info_container *info, struct buffers *buffs) {
  // Variável local de modo a não mexer diretamente no buffer partilhado
  MeasurementInfo m;

  // Receber a medição
  controller_receive_measurement(&m, controller_id, info, buffs);

  // Validar a medição
  controller_process_measurement(&m, controller_id, info);

  // Mandar medição para servidor
  controller_send_measurement(&m, info, buffs);

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
  // Incrementar o número de medições
  info->num_generated_measurements[controller_id]++;

  // Se for inválido, então incrementar também o número de medições inválidas
  if (!is_valid_measurement(m->value)) {
    info->num_invalid_measurements[controller_id]++;
    m->state = INVALID;
  } else {
    // Se for válido, coloca o seu id no struct
    m->state = VALID;
    m->controller_id = controller_id;
  }
};

void controller_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
  bool success = false;
  // Tentar sempre escrever no buffer sempre que a flag terminate não tiver ativa
  while (!success && !(*info->terminate)) {
    success = write_controller_servers_buffer(buffs->buff_controllers_servers, info->buffers_size, m);
  }
};
