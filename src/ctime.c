#include "ctime.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void validate_timestamps(struct timestamps *ts) {
  if (ts == NULL) {
    perror("validate_timestamps");
    exit(1);
  }
}

void init_timestamps(struct timestamps *ts) {
  validate_timestamps(ts);
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
  validate_timestamps(ts);
  clock_gettime(CLOCK_REALTIME, &ts->main_time);
}

void timestamp_sensor(struct timestamps *ts) {
  validate_timestamps(ts);
  clock_gettime(CLOCK_REALTIME, &ts->sensor_time);
}

void timestamp_controller(struct timestamps *ts) {
  validate_timestamps(ts);
  clock_gettime(CLOCK_REALTIME, &ts->controller_time);
}

void timestamp_server(struct timestamps *ts) {
  validate_timestamps(ts);
  clock_gettime(CLOCK_REALTIME, &ts->server_time);
}

double timespec_diff(struct timespec *start, struct timespec *end) {
  if (start == NULL) {
    perror("timespec_diff_start");
    exit(1);
  }

  if (end == NULL) {
    perror("timespec_diff_end");
    exit(1);
  }

  return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}

void format_current_time(char *out, size_t out_size) {
  if (out == NULL || out_size == 0) {
    perror("format_current_time");
    exit(1);
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
