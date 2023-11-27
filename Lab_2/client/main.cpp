#include "client.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Invalid amount of arguments" << std::endl;
    std::exit(1);
  }

  pid_t host_pid = std::atoi(argv[1]);

  Client &client = Client::get_instance();
  client.set_host_pid(host_pid);
  client.run();

  return 0;
}