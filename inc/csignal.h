#pragma once

#include "main.h"
#include <signal.h>

void setup_signal_handlers(void);

void ignore_signals(void);

int consume_alarm_tick(void);

void print_alarm_status(void);
