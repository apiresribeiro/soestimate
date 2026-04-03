#include "sensor.h"
#include "memory.h"
#include "random_measurement.h"
#include "stdbool.h"

int execute_sensor(int sensor_id, struct info_container *info, struct buffers *buffs) {
  seed_sensor_rng(sensor_id);
  int expected_m_id = *info->total_measurements + 1;
  MeasurementInfo req;

  sensor_receive_request(&req, expected_m_id, info, buffs);

  sensor_process_request(&req, sensor_id, info);

  sensor_send_measurement(&req, info, buffs);

  return 0;
}

void sensor_receive_request(MeasurementInfo *req, int expected_m_id, struct info_container *info, struct buffers *buffs) {
  req->m_id = -1;

  while (req->m_id == -1 && !(info->terminate)) {
    read_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, expected_m_id, req);
  }
}

void sensor_process_request(MeasurementInfo *m, int sensor_id, struct info_container *info) {
  if (m->state == REQUEST) {
    m->state = MEASURED;
    info->num_generated_measurements[sensor_id]++;
    m->sensor_id = sensor_id;
  }
}

void sensor_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
  bool success = false;

  while (!success && !(info->terminate)) {
    success = write_sensor_controller_buffer(buffs->buff_sensors_controllers, info->buffers_size, m);
  }
}
