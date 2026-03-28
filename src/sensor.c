#include "sensor.h"
#include "random_measurement.h"

int execute_sensor(int sensor_id, struct info_container *info, struct buffers *buffs) {
  int expectedMid = 1;
  MeasurementInfo req;
  while(1){
      sensor_receive_request(&req, expectedMid, info, buffs);
      sensor_process_request(&req, sensor_id, info);
      sensor_send_measurement(&req, info, buffs);
      expectedMid++;
  }
  return 0;
}

void sensor_receive_request(MeasurementInfo *req, int expected_m_id, struct info_container *info, struct buffers *buffs) {
  req->m_id = -1;
  while(req->m_id == -1){
    read_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, expected_m_id, req);
  }
}

void sensor_process_request(MeasurementInfo *m, int sensor_id, struct info_container *info) {
  m->sensor_id = sensor_id;
  m->value = get_measurement();
  m->state = MEASURED;
}

void sensor_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
  int sucess = 0;
  while(sucess == 0){
    sucess = write_sensor_controller_buffer(buffs->buff_sensors_controllers, info->buffers_size, m);
  }
}
