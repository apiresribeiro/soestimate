#include "controller.h"
#include "main.h"
#include "memory.h"
#include "random_measurement.h"
#include "stdbool.h"

int execute_controller(int controller_id, struct info_container *info, struct buffers *buffs) {
  // FIXME: Isto provavavelmente ta errado

  // Receber a medição
  controller_receive_measurement(buffs->buff_sensors_controllers->buffer, controller_id, info, buffs);

  // Validar a medição
  controller_process_measurement(buffs->buff_sensors_controllers->buffer, controller_id, info);

  // Mandar medição para servidor
  controller_send_measurement(buffs->buff_sensors_controllers->buffer, info, buffs);

  return 0;
};

void controller_receive_measurement(MeasurementInfo *m, int controller_id, struct info_container *info, struct buffers *buffs) {
  // TODO:
};

void controller_process_measurement(MeasurementInfo *m, int controller_id, struct info_container *info) {
  if (!(m->sensor_id == controller_id)) {
    info->num_invalid_measurements++;
    *info->terminate = 1; // FIXME: Colocar um erro?
  }

  // Incrementar o número de medições
  info->num_generated_measurements++;
  m->state = VALID;

  // Se for inválido, então incrementar também o número de medições inválidas
  if (!is_valid_measurement(m->value)) {
    info->num_invalid_measurements++;
    m->state = INVALID;
  }
};

void controller_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
  // Verificar se o array ptrs está livre, se sim, colocar o valor no buffer
  for (int i = 0; i < info->buffers_size; i++) {
    bool isEmpty = buffs->buff_controllers_servers->ptrs[i];
    if (isEmpty) {
      buffs->buff_controllers_servers->buffer->value = m->value; // FIXME: Isto está certo?

      // Atualizar o array ptrs de modo a atualizar o valor do boolean
      isEmpty = false;
      break;
    }
  }
};
