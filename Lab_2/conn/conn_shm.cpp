#include "conn_shm.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/syslog.h>
#include <unistd.h>

ConnShm::ConnShm(bool create, const std::string &name) {
  shm_name = name;
  int fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    syslog(LOG_ERR, "ERROR: failed to open shared memory file descriptor");
    std::exit(1);
  }

  if (create)
    if (ftruncate(fd, mem_size) == -1) {
      syslog(LOG_ERR,
             "ERROR: failed to truncate file descriptor to given size");
      std::exit(1);
    }

  mem = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    syslog(LOG_ERR, "ERROR: failed to allocate shared memory with mmap");
    std::exit(1);
  }
}

ConnShm::~ConnShm() {
  if (munmap(mem, mem_size) == -1 || shm_unlink(shm_name.c_str()) == -1) {
    std::cout << "Couldn't free shared memory" << std::endl;
    std::exit(1);
  }
}

inline void ConnShm::read(Message &msg) { std::memcpy(&msg, mem, sizeof(msg)); }

inline void ConnShm::write(const Message &msg) {
  std::memcpy(mem, &msg, sizeof(msg));
}

Conn *Conn::create(bool create, int conn_id) {
  std::string name = "/shm_wolf_and_goats" + std::to_string(conn_id);
  return new ConnShm(create, name);
}
