#include "host.h"
#include "client.h"
#include "client_info.h"
#include "conn.h"
#include "message.h"
#include "utils.h"
#include "wolf.h"
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <signal.h>
#include <stdexcept>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

Host::Host() {
  struct sigaction action;
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = &signal_handler;

  if (sigaction(SIGUSR1, &action, NULL) == -1)
    std::exit(1);
}

void Host::signal_handler(int sig, siginfo_t *info, void *extra) {
  if (sig == SIGUSR1) {
    Host &host = Host::get_instance();

    unsigned int id = host.add_client();
    sigqueue(info->si_value.sival_int, SIGUSR1, sigval{(int)id});
  }
}

Host &Host::get_instance() {
  static Host instance;
  return instance;
}

unsigned int Host::add_client() {
  unsigned int cl_id = cl_info_vec.size();

  sem_t *sem_host_msg = sem_open(
      ("/host_msg_sem_" + std::to_string(cl_id)).c_str(), O_CREAT, 0666, 1);
  sem_t *sem_client_msg = sem_open(
      ("/client_msg_sem_" + std::to_string(cl_id)).c_str(), O_CREAT, 0666, 1);

  Conn *connection = Conn::create(true, cl_id);

  cl_info_vec.emplace_back(
      ClientInfo{sem_host_msg, sem_client_msg, connection, 0, cl_id, true});

  ++active_clients;

  return cl_id;
}

Host::~Host() {
  for (const auto &cl_info : cl_info_vec) {
    if (sem_close(cl_info.sem_host_msg) == -1 ||
        sem_close(cl_info.sem_client_msg) == -1) {
      std::cout << "Couldn't close semaphores" << std::endl;
      std::exit(1);
    }

    if (sem_unlink(
            ("/host_msg_sem" + std::to_string(cl_info.client_id)).c_str()) ==
            -1 ||
        sem_unlink(
            ("/client_msg_sem" + std::to_string(cl_info.client_id)).c_str()) ==
            -1) {
      std::cout << "Couldn't unlink semaphores" << std::endl;
      std::exit(1);
    }

    delete cl_info.connection;
  }
}

void *Host::input_roll(void *roll) {
  size_t tmp;

  if (std::cin.fail())
    std::cin.clear();

  std::cin >> tmp;

  (*(size_t *)roll) = tmp;

  pthread_exit(NULL);
}

void Host::start_game() {
  while (true) {
    if (active_clients == 0) {
      sleep(5);
      continue;
    }

    size_t wolf_roll = (size_t)-1;

    pthread_t read_thread;
    std::cout << "Input wolf roll value: ";

    pthread_create(&read_thread, NULL, Host::input_roll, &wolf_roll);
    sleep(3);

    if (wolf_roll == (size_t)-1) {
      pthread_cancel(read_thread);
      pthread_join(read_thread, NULL);
      wolf_roll = wolf.roll();
    } else {
      pthread_join(read_thread, NULL);
    }

    std::cout << std::endl << "Wolf rolled " << wolf_roll << std::endl;

    for (auto &cl_info : cl_info_vec) {
      if (!cl_info.active)
        continue;

      wait_sem_with_timeout(cl_info.sem_client_msg, sem_timeout_secs);

      Message msg;
      cl_info.connection->read(msg);
      std::cout << "Goat " << cl_info.client_id << " rolled " << msg.roll
                << std::endl;

      // Determine new goat state
      if (msg.alive) {
        if ((abs((int)msg.roll - (int)wolf_roll) <= ALIVE_GOAT_GAP_REQUIRED))
          msg.alive = true;
        else
          msg.alive = false;
      } else {
        if ((abs((int)msg.roll - (int)wolf_roll) <= DEAD_GOAT_GAP_REQUIRED))
          msg.alive = true;
        else
          msg.alive = false;
      }

      auto bool_to_str = [](bool val) -> std::string {
        return val ? "True" : "False";
      };
      std::cout << "Goat " << cl_info.client_id
                << " alive: " << bool_to_str(msg.alive) << std::endl;

      if (!msg.alive)
        ++cl_info.death_row;
      else
        cl_info.death_row = 0;

      std::cout << "Goat " << cl_info.client_id
                << " death row: " << cl_info.death_row << std::endl;

      if (cl_info.death_row > 1) {
        std::cout << "Goat " << cl_info.client_id << " died" << std::endl;
        msg.active = false;
        cl_info.active = false;
        --active_clients;
      }

      cl_info.connection->write(msg);
      sem_post(cl_info.sem_host_msg);
    }
    std::cout << "End of round" << std::endl << std::endl;
  }
}
