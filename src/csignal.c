#include "csignal.h"
#include "main.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ignore_signals(void) {
  if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
    perror("ignore_signals");
  }
}

static void handle_sigint(int signum) {
  (void)signum;
  if (info == NULL || buffs == NULL) {
    return;
  }
  if (*info->terminate) {
    return;
  }
  end_execution(info, buffs);
  _exit(0);
}

static void handle_sigalrm(int signum) {
  (void)signum;

  if (info == NULL || *info->terminate || info->period == 0) {
    return;
  }

  print_stat(info);
  alarm(info->period);
}

void setup_signal_handlers(void) {
  if (signal(SIGINT, handle_sigint) == SIG_ERR) {
    perror("setup_signal_handlers SIGINT");
  }

  if (signal(SIGALRM, handle_sigalrm) == SIG_ERR) {
    perror("setup_signal_handlers SIGALRM");
  }

  if (info != NULL && info->period > 0) {
    alarm(info->period);
  }
}
