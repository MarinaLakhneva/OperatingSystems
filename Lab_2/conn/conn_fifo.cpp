#include "conn_fifo.h"
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

ConnFifo::ConnFifo(const std::string &name, bool create) {
  this->name = name;

  if (create) {
    if (mkfifo(name.c_str(), 0666) < 0) {
      syslog(LOG_ERR, "ERROR: failed to create");
      std::exit(1);
    }
  }
  descriptor = ::open(name.c_str(), O_RDWR);
  if (descriptor < 0) {
    syslog(LOG_ERR, "ERROR: failed to open");
    std::exit(1);
  }
}

void ConnFifo::read(Message &msg) {
  if (::read(descriptor, &msg, sizeof(Message)) < 0) {
    syslog(LOG_ERR, "ERROR: failed to read");
    std::exit(1);
  }
}

void ConnFifo::write(const Message &msg) {
  if (::write(descriptor, &msg, sizeof(Message)) < 0) {
    syslog(LOG_ERR, "ERROR: failed to write");
    std::exit(1);
  }
}

ConnFifo::~ConnFifo() {
  if (::close(descriptor) < 0) {
    syslog(LOG_ERR, "ERROR: failed to close");
    std::exit(1);
  }
  if (unlink(name.c_str()) < 0) {
    syslog(LOG_ERR, "ERROR: failed to unlink");
    std::exit(1);
  }
}

Conn *Conn::create(bool create, int conn_id) {
  std::string name = "/tmp/fifo_conn_" + std::to_string(conn_id);

  return new ConnFifo(name, create);
}
