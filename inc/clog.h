#pragma once
#include "main.h"
#include <stdio.h>

void clean_log(const char *log_filename);

void write_log(struct info_container *info, const char *command);

void write_server_result(struct info_container *info, int server_id, int m_id, double estimate, double elapsed_time);
