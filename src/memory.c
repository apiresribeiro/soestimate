// Alexandre Ribeiro - 63723
// André Vilarinho - 63727
// Margarida Torres - 63751
#include "memory.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// Função auxiliar para evitar duplicação de código
static void create_shm_name(char *dst, const char *name) {
  char uid_str[32];
  dst[0] = '/';
  dst[1] = '\0';
  strcat(dst, name);

  // Converte uid para string
  sprintf(uid_str, "%u", getuid());

  strcat(dst, uid_str);
}

void *allocate_dynamic_memory(int size) {
  if (size <= 0) {
    return NULL;
  }
  // Aloca 1 * size bytes, inicializa tudo a zeros
  void *ptr = calloc(1, size);
  return ptr;
};

void *create_shared_memory(char *name, int size) {
  if (name == NULL || size <= 0) {
    return NULL;
  }

  char shm_name[256];
  create_shm_name(shm_name, name);

  // Cria a zona partilhada
  int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    return NULL;
  }

  // Define o tamanho do segmento
  if (ftruncate(fd, size) == -1) {
    close(fd);
    shm_unlink(shm_name);
    return NULL;
  }

  // Projeta em memória
  void *ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);

  if (ptr == MAP_FAILED) {
    shm_unlink(shm_name);
    return NULL;
  }
  // Mete todos os valores a 0
  memset(ptr, 0, size);

  return ptr;
};

void deallocate_dynamic_memory(void *ptr) {
  free(ptr);
};

void destroy_shared_memory(char *name, void *ptr, int size) {
  char shm_name[256];
  create_shm_name(shm_name, name);
  munmap(ptr, size);
  shm_unlink(shm_name);
};

int write_main_sensors_buffer(struct circ_buffer *buffer, int buffer_size, const MeasurementInfo *req) {
  if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0 || req == NULL) {
    return 0;
  }

  int in = buffer->ptrs->in;
  int out = buffer->ptrs->out;
  int next = (in + 1) % buffer_size;

  // Buffer cheio (garante sempre pelo menos 1 slot vazio)
  if (next == out) {
    return 0;
  }

  // Copia o struct para o in
  buffer->buffer[in] = *req;
  // Avança a posição de escrita
  buffer->ptrs->in = next;

  return 1;
};

void read_main_sensors_buffer(struct circ_buffer *buffer, int buffer_size, int expected_m_id, MeasurementInfo *req) {
  if (req == NULL) {
    return;
  }

  // Não há request disponível, default
  req->m_id = -1;

  if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0) {
    return;
  }

  int in = buffer->ptrs->in;
  int out = buffer->ptrs->out;

  // Precorre todas as posições ocupadas do buffer
  while (out != in) {
    MeasurementInfo *atual = &buffer->buffer[out];
    // Verifica se o m_id que estamos à procura é o que estamos a precorrer
    if (atual->m_id == expected_m_id && atual->counter_sensors > 0) {
      // Caso sim, coloca a cópia local no sensor
      *req = *atual;
      // Decrementa o couter de sensores
      atual->counter_sensors--;

      // Quando todos os sensores acabarem de ler, avança o out
      if (atual->counter_sensors == 0 && out == buffer->ptrs->out) {
        buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
      }
      return;
    }
    out = (out + 1) % buffer_size;
  }
};

int write_sensor_controller_buffer(struct ra_buffer *buffer, int buffer_size, const MeasurementInfo *m) {
  if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0 || m == NULL) {
    return 0;
  }

  for (int i = 0; i < buffer_size; i++) {
    if (buffer->ptrs[i] == 0) {
      buffer->buffer[i] = *m;
      // Marca como ocupado
      buffer->ptrs[i] = 1;
      return 1;
    }
  }

  // Cheio
  return 0;
};

void read_sensor_controller_buffer(struct ra_buffer *buffer, int controller_id, int buffer_size, MeasurementInfo *m) {
  if (m == NULL) {
    return;
  }

  // Default, não há request
  m->m_id = -1;

  if (buffer == NULL || buffer->buffer == NULL || buffer->ptrs == NULL || buffer_size <= 0) {
    return;
  }

  for (int i = 0; i < buffer_size; i++) {
    MeasurementInfo *atual = &buffer->buffer[i];
    if (buffer->ptrs[i] == 1 && buffer->buffer[i].sensor_id == controller_id) {
      // Coloca a cópia local no controller
      *m = *atual;
      // Liberta a slot
      buffer->ptrs[i] = 0;
      return;
    }
  }
};

int write_controller_servers_buffer(struct ra_buffer *buffer, int buffer_size, const MeasurementInfo *m) {
  if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0 || m == NULL) {
    return 0;
  }

  for (int i = 0; i < buffer_size; i++) {
    if (buffer->ptrs[i] == 0) {
      buffer->buffer[i] = *m;
      // Marca como ocupado
      buffer->ptrs[i] = 1;
      return 1;
    }
  }

  // Cheio
  return 0;
};

void read_controller_servers_buffer(struct ra_buffer *buffer, int buffer_size, int expected_m_id, int expected_controller_id, MeasurementInfo *m) {
  if (m == NULL) {
    return;
  }

  // Default, não há request
  m->m_id = -1;

  if (buffer == NULL || buffer->buffer == NULL || buffer->ptrs == NULL || buffer_size <= 0) {
    return;
  }

  for (int i = 0; i < buffer_size; i++) {
    MeasurementInfo *atual = &buffer->buffer[i];
    if (buffer->ptrs[i] == 1 && buffer->buffer[i].controller_id == expected_controller_id) {
      if (atual->m_id == expected_m_id && atual->counter_servers > 0) {
        // Coloca a cópia local no controller
        *m = *atual;
        // Decrementa o counter de servers
        atual->counter_servers--;

        if (atual->counter_servers == 0) {
          // Liberta a slot
          buffer->ptrs[i] = 0;
        }
        return;
      }
    }
  }
};
