#include "memory.h"

void *allocate_dynamic_memory(int size) {};

void *create_shared_memory(char *name, int size) {};

void deallocate_dynamic_memory(void *ptr) {};

void destroy_shared_memory(char *name, void *ptr, int size) {};

int write_main_sensors_buffer(struct circ_buffer *buffer, int buffer_size,
                              const MeasurementInfo *req) {
  return 1;
};

void read_main_sensors_buffer(struct circ_buffer *buffer, int buffer_size,
                              int expected_m_id, MeasurementInfo *req) {};

int write_sensor_controller_buffer(struct ra_buffer *buffer, int buffer_size,
                                   const MeasurementInfo *m) {
  return 1;
};

void read_sensor_controller_buffer(struct ra_buffer *buffer, int controller_id,
                                   int buffer_size, MeasurementInfo *m) {};

int write_controller_servers_buffer(struct ra_buffer *buffer, int buffer_size,
                                    const MeasurementInfo *m) {
  return 1;
};

void read_controller_servers_buffer(struct ra_buffer *buffer, int buffer_size,
                                    int expected_m_id,
                                    int expected_controller_id,
                                    MeasurementInfo *m) {};
