#include "conn_mq.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/syslog.h>

ConnMq::ConnMq(const std::string &name, bool create) {
  queue_name = name;

  if (create) {
    mq_attr attr;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_maxmsg = 1;

    descriptor = mq_open(name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
  } else {
    descriptor = mq_open(name.c_str(), O_RDWR);
  }

  if (descriptor == -1) {
    syslog(LOG_ERR, "ERROR: failed to open shared queue");
    std::exit(1);
  }
}

ConnMq::~ConnMq() {
  if (mq_close(descriptor) == -1 || mq_unlink(queue_name.c_str()) == -1) {
    std::cout << "Couldn't free shared memory" << std::endl;
    std::exit(1);
  }
}

void ConnMq::read(Message &msg) {
  mq_receive(descriptor, (char *)&msg, sizeof(msg), &default_prio);
}

void ConnMq::write(const Message &msg) {
  mq_send(descriptor, (const char *)&msg, sizeof(msg), default_prio);
}

Conn *Conn::create(bool create, int conn_id) {
  std::string name = "/mq_wolf_and_goats_" + std::to_string(conn_id);

  return new ConnMq(name, create);
}
