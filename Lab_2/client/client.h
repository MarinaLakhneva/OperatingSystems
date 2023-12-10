#pragma once
#include "conn.h"
#include "goat.h"
#include <semaphore.h>
#include <signal.h>

class Client {
public:
  static Client &get_instance();

  void set_host_pid(pid_t host_pid);

  void run();

private:
  Client();

  static void signal_handler(int sig, siginfo_t *info, void *instance);

  bool connected;
  pid_t host_pid;
  Goat goat;
  sem_t *sem_client_msg, *sem_host_msg;
  Conn *connection;
  const size_t sem_timeout_secs = 10;
  int id;
};
