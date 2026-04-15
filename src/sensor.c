// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "sensor.h"
#include "memory.h"
#include "random_measurement.h"
#include "stdbool.h"

int execute_sensor(int sensor_id, struct info_container *info, struct buffers *buffs) {
  // Seed ranodmizada para o random_measurement
  seed_sensor_rng(sensor_id);
  // O sensor começa por esperar pela primeira ronda de medições
  int expected_m_id = 1;
  MeasurementInfo req;
  while (!(*info->terminate)) {
    // Tenta ler o pedido da ronda esperada.
    sensor_receive_request(&req, expected_m_id, info, buffs);
    if (*info->terminate) {
      break;
    }
    // Se ainda não há pedido novo, volta a tentar.
    if (req.m_id == -1) {
      continue;
    }
    // Processa pedido (gera medição e atualiza estado/estatísticas).
    sensor_process_request(&req, sensor_id, info);
    // Envia medição para o buffer sensor->controller.
    sensor_send_measurement(&req, info, buffs);
    // Passa para a próxima ronda.
    expected_m_id++;
  }
  return 0;
}

void sensor_receive_request(MeasurementInfo *req, int expected_m_id, struct info_container *info, struct buffers *buffs) {
  // id = -1 quando não existe request
  req->m_id = -1;

  // Enquanto não existir requests, tentar ler até à flag estar ativa
  while (req->m_id == -1 && !(*info->terminate)) {
    read_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, expected_m_id, req);
  }
}

void sensor_process_request(MeasurementInfo *m, int sensor_id, struct info_container *info) {
  // Se o estado da medição for REQUEST então proceder ao próximo passo
  if (m->state == REQUEST) {
    m->state = MEASURED;
    info->num_generated_measurements[sensor_id]++;
    m->sensor_id = sensor_id;
    m->value = get_measurement();
  }
}

void sensor_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
  bool success = false;

  // Enquanto não a escrita não der sucesso, tentar escrever até à flag não permitir
  while (!success && !(*info->terminate)) {
    success = write_sensor_controller_buffer(buffs->buff_sensors_controllers, info->buffers_size, m);
  }
}
