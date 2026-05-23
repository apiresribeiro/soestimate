#include "synchronization.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

sem_t *create_semaphore(char *name, int value) {
  sem_t *result = sem_open(name, O_CREAT, 0644, value);
  if (result == SEM_FAILED) {
    perror("create_semaphore");
    exit(1);
  }
  return result;
}

void destroy_semaphore(char *name, sem_t *sem) {
  if (sem_close(sem) == -1) {
    perror("destroy_semaphore_close");
    exit(1);
  }
  if (sem_unlink(name) == -1) {
    perror("destroy_semaphore_unlink");
    exit(1);
  }
}

struct buffer_semaphores *create_buffer_semaphores(int value, char *unread_name, char *free_name, char *mutex_name) {
  struct buffer_semaphores *sems = malloc(sizeof(struct buffer_semaphores));
  if (sems == NULL) {
    perror("create_buffer_semaphores");
    exit(1);
  }

  sems->unread = create_semaphore(unread_name, 0);
  sems->free_space = create_semaphore(free_name, value);
  sems->mutex = create_semaphore(mutex_name, 1);
  return sems;
}

void destroy_buffer_semaphores(struct buffer_semaphores *sems, char *unread_name, char *free_name, char *mutex_name) {
  if (sems == NULL) {
    perror("destroy_buffer_semaphores");
    exit(1);
  }
  destroy_semaphore(unread_name, sems->unread);
  destroy_semaphore(free_name, sems->free_space);
  destroy_semaphore(mutex_name, sems->mutex);
  free(sems);
}

struct semaphores *create_all_semaphores(unsigned value) {
  struct semaphores *sems = malloc(sizeof(struct semaphores));
  if (sems == NULL) {
    perror("create_all_semaphores");
    exit(1);
  }
  sems->main_sensors = create_buffer_semaphores(value, SEM_MAIN_SENSORS_UNREAD, SEM_MAIN_SENSORS_FREE, SEM_MAIN_SENSORS_MUTEX);
  sems->sensors_controllers = create_buffer_semaphores(value, SEM_SENSORS_CONTROLLERS_UNREAD, SEM_SENSORS_CONTROLLERS_FREE, SEM_SENSORS_CONTROLLERS_MUTEX);
  sems->controllers_servers = create_buffer_semaphores(value, SEM_CONTROLLERS_SERVERS_UNREAD, SEM_CONTROLLERS_SERVERS_FREE, SEM_CONTROLLERS_SERVERS_MUTEX);

  sems->terminate_mutex = create_semaphore(SEM_TERMINATE_MUTEX, 1);
  sems->log_mutex = create_semaphore(SEM_LOG_MUTEX, value);

  return sems;
}

void destroy_all_semaphores(struct semaphores *sems) {
  if (sems == NULL) {
    perror("destroy_all_semaphores");
    exit(1);
  }
  destroy_buffer_semaphores(sems->main_sensors, SEM_MAIN_SENSORS_UNREAD, SEM_MAIN_SENSORS_FREE, SEM_MAIN_SENSORS_MUTEX);
  destroy_buffer_semaphores(sems->sensors_controllers, SEM_SENSORS_CONTROLLERS_UNREAD, SEM_SENSORS_CONTROLLERS_FREE, SEM_SENSORS_CONTROLLERS_MUTEX);
  destroy_buffer_semaphores(sems->controllers_servers, SEM_CONTROLLERS_SERVERS_UNREAD, SEM_CONTROLLERS_SERVERS_FREE, SEM_CONTROLLERS_SERVERS_MUTEX);

  destroy_semaphore(SEM_TERMINATE_MUTEX, sems->terminate_mutex);
  destroy_semaphore(SEM_LOG_MUTEX, sems->log_mutex);

  free(sems);
}

void print_all_semaphores(struct semaphores *sems) {
  int value;
  // Main -> Sensors
  sem_getvalue(sems->main_sensors->unread, &value);
  printf("Nome: %d\n", value);
}

void wakeup_processes(struct info_container *info) {
}
