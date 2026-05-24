// Grupo 1
// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "csignal.h"
#include "main.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t alarm_pending = 0;

void ignore_signals(void) {
  if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
    perror("ignore_signals");
  }
}

static void handle_sigint(int signum) {
  (void)signum;

  if (info == NULL || info->terminate == NULL) {
    return;
  }

  *info->terminate = 1;

  if (info->sems != NULL) {
    wakeup_processes(info);
  }
}

static void handle_sigalrm(int signum) {
  (void)signum;

  if (info == NULL || info->terminate == NULL || *info->terminate || info->period == 0) {
    return;
  }

  alarm_pending = 1;
  alarm(info->period);
}

int consume_alarm_tick(void) {
  if (alarm_pending) {
    alarm_pending = 0;
    return 1;
  }

  return 0;
}

void print_alarm_status(void) {
  if (info == NULL || info->total_measurements == NULL || info->sems == NULL) {
    return;
  }

  int total_cycles = *info->total_measurements;
  if (total_cycles <= 0) {
    return;
  }

  double *start_times = calloc((size_t)(total_cycles + 1), sizeof(double));
  double *result_times = calloc((size_t)(total_cycles + 1), sizeof(double));
  if (start_times == NULL || result_times == NULL) {
    free(start_times);
    free(result_times);
    return;
  }

  for (int i = 0; i <= total_cycles; i++) {
    start_times[i] = -1.0;
    result_times[i] = -1.0;
  }

  sem_wait(info->sems->log_mutex);
  FILE *file = fopen(info->log_filename, "r");
  if (file != NULL) {
    char line[256];
    int cycle_index = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
      char date[16];
      char clock_str[16];
      char command[16];

      if (sscanf(line, "%15s %15s %15s", date, clock_str, command) != 3) {
        continue;
      }

      if (strcmp(command, "measure") == 0) {
        cycle_index++;
        if (cycle_index <= total_cycles) {
          start_times[cycle_index] = parse_formatted_timestamp(date, clock_str);
        }
        continue;
      }

      if (strcmp(command, "result") == 0) {
        int m_id;
        int server_id;
        double estimate;
        double elapsed;
        int parsed = sscanf(line,
                            "%*s %*s result %d server %d estimate %lf elapsed %lf",
                            &m_id,
                            &server_id,
                            &estimate,
                            &elapsed);
        if (parsed == 4 && m_id >= 1 && m_id <= total_cycles) {
          result_times[m_id] = elapsed;
        }
      }
    }

    fclose(file);
  }
  sem_post(info->sems->log_mutex);

  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  double now_seconds = (double)now.tv_sec + ((double)now.tv_nsec / 1e9);

  for (int m_id = 1; m_id <= total_cycles; m_id++) {
    double elapsed = 0.0;

    if (result_times[m_id] >= 0.0) {
      elapsed = result_times[m_id];
    } else if (start_times[m_id] >= 0.0) {
      elapsed = now_seconds - start_times[m_id];
      if (elapsed < 0.0) {
        elapsed = 0.0;
      }
    }

    printf("%d %.3f\n", m_id, elapsed);
  }

  free(start_times);
  free(result_times);
}

void setup_signal_handlers(void) {
  struct sigaction sigint_action = {0};
  sigemptyset(&sigint_action.sa_mask);
  sigint_action.sa_handler = handle_sigint;
  sigint_action.sa_flags = 0;

  if (sigaction(SIGINT, &sigint_action, NULL) == -1) {
    perror("setup_signal_handlers SIGINT");
  }

  struct sigaction sigalrm_action = {0};
  sigemptyset(&sigalrm_action.sa_mask);
  sigalrm_action.sa_handler = handle_sigalrm;
  sigalrm_action.sa_flags = 0;

  if (sigaction(SIGALRM, &sigalrm_action, NULL) == -1) {
    perror("setup_signal_handlers SIGALRM");
  }

  if (info != NULL && info->period > 0) {
    alarm(info->period);
  }
}
