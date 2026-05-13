#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

sem_t *create_semaphore(char *name, int value);

void destroy_semaphore(char *name, sem_t *sem);

struct buffer_semaphores *create_buffer_semaphores(int value, char *unread_name, char *free_name, char *mutex_name);

void destroy_buffer_semaphores(struct buffer_semaphores *sems, char *unread_name, char *free_name, char *mutex_name);

struct semaphores *create_all_semaphores(unsigned value);

void destroy_all_semaphores(struct semaphores *sems);

void print_all_semaphores(struct semaphores *sems);

void wakeup_processes(struct info_container *info);
