// Grupo 1
// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "synchronization.h"
#include "main.h"
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
  sems->log_mutex = create_semaphore(SEM_LOG_MUTEX, 1);

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
  if (sems == NULL) {
    return;
  }

  int value;

  // Main -> Sensors
  sem_getvalue(sems->main_sensors->unread, &value);
  printf("main_sensors.unread: %d\n", value);
  sem_getvalue(sems->main_sensors->free_space, &value);
  printf("main_sensors.free_space: %d\n", value);
  sem_getvalue(sems->main_sensors->mutex, &value);
  printf("main_sensors.mutex: %d\n", value);

  // Sensors -> Controllers
  sem_getvalue(sems->sensors_controllers->unread, &value);
  printf("sensors_controllers.unread: %d\n", value);
  sem_getvalue(sems->sensors_controllers->free_space, &value);
  printf("sensors_controllers.free_space: %d\n", value);
  sem_getvalue(sems->sensors_controllers->mutex, &value);
  printf("sensors_controllers.mutex: %d\n", value);

  // Controllers -> Servers
  sem_getvalue(sems->controllers_servers->unread, &value);
  printf("controllers_servers.unread: %d\n", value);
  sem_getvalue(sems->controllers_servers->free_space, &value);
  printf("controllers_servers.free_space: %d\n", value);
  sem_getvalue(sems->controllers_servers->mutex, &value);
  printf("controllers_servers.mutex: %d\n", value);

  // Log Mutex
  sem_getvalue(sems->log_mutex, &value);
  printf("log_mutex: %d\n", value);

  // Terminate Mutex
  sem_getvalue(sems->terminate_mutex, &value);
  printf("terminate_mutex: %d\n", value);
}

void wakeup_processes(struct info_container *info) {
  if (info == NULL || info->sems == NULL) {
    return;
  }

  int n_process = info->n_servers + info->n_sensors + 2;

  // Pode haver mais que 1 processo bloqueado!
  for (int i = 0; i < n_process; i++) {
    sem_post(info->sems->main_sensors->unread);
    sem_post(info->sems->main_sensors->free_space);
    sem_post(info->sems->sensors_controllers->unread);
    sem_post(info->sems->sensors_controllers->free_space);
    sem_post(info->sems->controllers_servers->unread);
    sem_post(info->sems->controllers_servers->free_space);
  }
}
