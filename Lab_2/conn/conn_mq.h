#pragma once

#include "conn.h"
#include <mqueue.h>
#include <string>

class ConnMq : public Conn {
public:
  ConnMq(const std::string &name, bool create);
  ~ConnMq();

  inline void read(Message &msg) override;
  inline void write(const Message &msg) override;

private:
  mqd_t descriptor;
  std::string queue_name;
  unsigned int default_prio = 10;
};
