#include "controller.h"
#include "main.h"
#include "random_measurement.h"
#include "stdbool.h"

int execute_controller(int controller_id, struct info_container *info,
                       struct buffers *buffs) {
  return 1;
};

void controller_receive_measurement(MeasurementInfo *m, int controller_id,
                                    struct info_container *info,
                                    struct buffers *buffs) {};

void controller_process_measurement(MeasurementInfo *m, int controller_id,
                                    struct info_container *info) {
  if (!(m->sensor_id == controller_id)) {
    info->num_invalid_measurements++;
    return; // TODO Colocar um erro?
  }

  // Incrementar o número de medições
  info->num_generated_measurements++;

  // Se for inválido, então incrementar também o número de medições inválidas
  if (!is_valid_measurement(m->value)) {
    info->num_invalid_measurements++;
  }
};

void controller_send_measurement(MeasurementInfo *m,
                                 struct info_container *info,
                                 struct buffers *buffs) {
  // Verificar se o array ptrs está livre, se sim, colocar o valor no buffer
  for (int i = 0; i < info->buffers_size; i++) {
    if (buffs->buff_sensors_controllers->ptrs[i] == false) {
      buffs->buff_sensors_controllers->buffer->value =
          m->value; // TODO Isto está certo?
    }
  }
};
