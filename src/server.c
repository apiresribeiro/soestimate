#include "server.h"

int execute_server(int server_id, struct info_container *info,
                   struct buffers *buffs) {
  return 1;
};

void read_expected_cycle_measurement(MeasurementInfo *m, int expected_m_id,
                                     const int *received_flags,
                                     struct info_container *info,
                                     struct buffers *buffs) {};

int server_process_measurement(MeasurementInfo *m, int server_id,
                               struct info_container *info, double *estimate,
                               int *valid_count) {
  return 1;
};

void server_print_estimate(int server_id, int m_id, double estimate,
                           int valid_count) {};
