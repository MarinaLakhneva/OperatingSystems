#pragma once
#include "client_info.h"
#include "wolf.h"
#include <signal.h>
#include <vector>

class Host {
public:
  ~Host();
  static Host &get_instance();

  void start_game();

private:
  Host();

  static void *input_roll(void *roll);
  static void signal_handler(int sig, siginfo_t *info, void *instance);
  unsigned int add_client();

  std::vector<ClientInfo> cl_info_vec;
  unsigned int active_clients;
  int min_clients = 1;
  int max_clients = 5;
  const size_t sem_timeout_secs = 10;

  Wolf wolf;
  const size_t ALIVE_GOAT_GAP_REQUIRED = 70;
  const size_t DEAD_GOAT_GAP_REQUIRED = 20;
};