#ifndef CLOG_H
#define CLOG_H
#include <stdio.h>
#include "main.h" 

void reescrever_log_existente(const char*log_filename);

void print_log(struct info_container *info, const char *comando);

void print_server_result(struct info_container *info, int server_id, int m_id, double estimate, int valid_count, double elapsed_time);


#endif