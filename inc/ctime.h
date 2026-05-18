#include <time.h>

struct timestamps {
  struct timespec main_time;
  struct timespec sensor_time;
  struct timespec controller_time;
  struct timespec server_time;
};

// Inicializa todos os timestamps a zero
void init_timestamps(struct timestamps *ts);

// Regista o instante atual em cada timestamp
void timestamp_main(struct timestamps *ts);
void timestamp_sensor(struct timestamps *ts);
void timestamp_controller(struct timestamps *ts);
void timestamp_server(struct timestamps *ts);

// Calcula a diferença em segundos com decimais
double timespec_diff(struct timespec *start, struct timespec *end);

// Formata para: YYYYYMMDD HH:MM:SS.mmm
void format_current_time(char *out, size_t out_size);
