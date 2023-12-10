#include "client.h"
#include "conn.h"
#include "goat.h"
#include "message.h"
#include "utils.h"
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <signal.h>
#include <stdexcept>
#include <unistd.h>

Client &Client::get_instance() {
  static Client instance;
  return instance;
}

Client::Client() {
  struct sigaction action;
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = &signal_handler;

  if (sigaction(SIGUSR1, &action, NULL) == -1)
    std::exit(1);

  connected = false;
}

void Client::set_host_pid(pid_t host_pid) {
  this->host_pid = host_pid;
  sigqueue(host_pid, SIGUSR1, sigval{(int)getpid()});
}

void Client::signal_handler(int sig, siginfo_t *info, void *extra) {
  if (sig == SIGUSR1) {
    Client &client = Client::get_instance();

    unsigned int cl_id = info->si_value.sival_int;

    client.id = cl_id;
    client.sem_host_msg = sem_open(
        ("/host_msg_sem_" + std::to_string(cl_id)).c_str(), O_CREAT, 0666, 1);
    client.sem_client_msg = sem_open(
        ("/client_msg_sem_" + std::to_string(cl_id)).c_str(), O_CREAT, 0666, 1);

    client.connection = Conn::create(false, cl_id);

    client.connected = true;
  }
}

void Client::run() {
  while (true) {
    if (!connected) {
      continue;
      // wait 2 seconds
      usleep(2000000);
    }

    Message msg = {goat.roll(), goat.alive(), true};
    connection->write(msg);
    sem_post(sem_client_msg);

    wait_sem_with_timeout(sem_host_msg, sem_timeout_secs);
    connection->read(msg);

    if (!msg.active)
      std::exit(0);

    goat.set_status(msg.alive);
  }
}