#include "controller.h"

int execute_controller(int controller_id, struct info_container *info,
                       struct buffers *buffs) {
  return 1;
};

void controller_receive_measurement(MeasurementInfo *m, int controller_id,
                                    struct info_container *info,
                                    struct buffers *buffs) {};

void controller_process_measurement(MeasurementInfo *m, int controller_id,
                                    struct info_container *info) {};

void controller_send_measurement(MeasurementInfo *m,
                                 struct info_container *info,
                                 struct buffers *buffs) {};
