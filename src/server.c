#include "server.h"
#include "memory.h"
#include <stdio.h>

int execute_server(int server_id, struct info_container *info, struct buffers *buffs) {
  int expected_m_id = 1;               // Próximo ciclo que o servidor está a espera
  int n_controllers = info->n_sensors; // 1 controller por sensor
  MeasurementInfo m;
  double estimate = 0.0;
  int valid_count = 0;
  int received_flags[n_controllers]; // Flags da ronda atual, 0 -> controller ainda não recebido, 1 -> controller recebido

  // Loop principal, corre até a flag terminate
  while (!(*info->terminate)) {
    // Início de nova ronda, limpar variáveis locais
    for (int i = 0; i < n_controllers; i++) {
      received_flags[i] = 0;
    }
    estimate = 0.0;
    valid_count = 0;

    // Recolher medições desta ronda até estar completa.
    while (!(*info->terminate)) {
      // Tentar ler a próxima medição em falta da ronda
      read_expected_cycle_measurement(&m, expected_m_id, received_flags, info, buffs);

      if (*info->terminate) {
        break;
      }
      // Se não houver novas medições, continuar
      if (m.m_id == -1) {
        continue;
      }

      // Marcar controller recebido para esta ronda.
      if (m.controller_id >= 0 && m.controller_id < n_controllers) {
        received_flags[m.controller_id] = 1;
      }

      // Quando todas as contribuições da ronda estiverem processadas:
      if (server_process_measurement(&m, server_id, info, &estimate, &valid_count)) {
        // Imprimir o resultado da ronda
        server_print_estimate(server_id, expected_m_id, estimate, valid_count);
        expected_m_id++;
        break; // passa para a próxima ronda
      }
    }
  }
  return 0;
};

void read_expected_cycle_measurement(MeasurementInfo *m, int expected_m_id, const int *received_flags, struct info_container *info, struct buffers *buffs) {
  m->m_id = -1;
  int n_controllers = info->n_sensors;

  while (m->m_id == -1 && !(*info->terminate)) {
    int expected_controller_id = -1;
    for (int i = 0; i < n_controllers; i++) {
      if (received_flags[i] == 0) {
        expected_controller_id = i;
        break;
      }
    }
    if (expected_controller_id == -1) {
      break; // ronda já completa
    }
    read_controller_servers_buffer(buffs->buff_controllers_servers, info->buffers_size, expected_m_id, expected_controller_id, m);
  }
}

int server_process_measurement(MeasurementInfo *m, int server_id, struct info_container *info, double *estimate, int *valid_count) {
  static int local_m_id = -1; // m_id da ronda atual
  static int count = 0;       // Total de medições (inválidas ou válidas) da ronda atual
  static double sum = 0.0;    // Soma dos valores de medições válidas da ronda atual

  // Se não chegou uma nova ronda, não existe média
  if (m->m_id == -1) {
    return 0;
  }

  // Se chegou uma nova ronda, dar reset às variáveis locais
  if (local_m_id != m->m_id) {
    local_m_id = m->m_id;
    count = 0;
    sum = 0.0;
    *valid_count = 0;
  }

  // Inicio da medição
  count++;

  // Verificar se a medição é válida para colocar no count
  if (m->state == VALID) {
    sum += m->value;
    (*valid_count)++;
  }

  // Se faltarem controllers desta ronda, ainda não é possível efetuar a média
  if (count < info->n_sensors) {
    return 0;
  }

  // Se houver medições válidas, faz a média, caso contrário 0.0
  if (*valid_count > 0) {
    *estimate = sum / (*valid_count);
  } else {
    *estimate = 0.0;
  }

  // Atualiza o struct
  info->num_estimates[server_id]++;

  // Ronda completa
  return 1;
};

void server_print_estimate(int server_id, int m_id, double estimate, int valid_count) {
  // Se houver pelo menos um valor válido, imprimir a informação
  if (valid_count > 0) {
    printf("[Server %d] New estimate: cycle %d = %.4f (valid_count=%d)\n", server_id, m_id, estimate, valid_count);
  } else {
    printf("[Server %d] Cycle %d all measurements were invalid\n", server_id, m_id);
  }
};
