#include "host.h"
#include <iostream>

int main() {
  std::cout << "Starting game" << std::endl << std::endl;
  Host &host = Host::get_instance();

  host.start_game();

  std::cout << "Exiting game" << std::endl;
  return 0;
}