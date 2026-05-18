#ifndef CLOG_H
#define CLOG_H
#include "main.h"
#include <stdio.h>

void reescrever_log_existente(const char *log_filename);

void write_log(struct info_container *info, const char *comando);

void write_server_result(struct info_container *info, int server_id, int m_id, double estimate, double elapsed_time);

#endif
