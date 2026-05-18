#include "ctime.h"
#include <stdio.h>
#include <time.h>

void init_timestamps(struct timestamps *ts) {
  // Main
  ts->main_time.tv_sec = 0;
  ts->main_time.tv_nsec = 0;

  // Sensor
  ts->sensor_time.tv_sec = 0;
  ts->sensor_time.tv_nsec = 0;

  // Controller
  ts->controller_time.tv_sec = 0;
  ts->controller_time.tv_nsec = 0;

  // Server
  ts->server_time.tv_sec = 0;
  ts->server_time.tv_nsec = 0;
}

void timestamp_main(struct timestamps *ts) {
  clock_gettime(CLOCK_REALTIME, &ts->main_time);
}

void timestamp_sensor(struct timestamps *ts) {
  clock_gettime(CLOCK_REALTIME, &ts->sensor_time);
}

void timestamp_controller(struct timestamps *ts) {
  clock_gettime(CLOCK_REALTIME, &ts->controller_time);
}

void timestamp_server(struct timestamps *ts) {
  clock_gettime(CLOCK_REALTIME, &ts->server_time);
}

double timespec_diff(struct timespec *start, struct timespec *end) {
  return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}

void format_current_time(char *out, size_t out_size) {
  if (out == NULL || out_size == 0) {
    return;
  }

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  struct tm tm_now;
  // Converte de segundos para ano mês dia...
  localtime_r(&ts.tv_sec, &tm_now);

  int ms = (int)(ts.tv_nsec / 1000000);

  snprintf(out,
           out_size,
           "%04d%02d%02d %02d:%02d:%02d.%03d",
           tm_now.tm_year + 1900,
           tm_now.tm_mon + 1,
           tm_now.tm_mday,
           tm_now.tm_hour,
           tm_now.tm_min,
           tm_now.tm_sec,
           ms);
}
