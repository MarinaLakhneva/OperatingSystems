#pragma once

#include "conn.h"
#include <semaphore.h>

struct ClientInfo {
  sem_t *sem_host_msg;
  sem_t *sem_client_msg;
  Conn *connection;
  size_t death_row;
  unsigned int client_id;
  bool active;
};