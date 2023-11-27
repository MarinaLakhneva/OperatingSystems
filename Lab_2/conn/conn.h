#pragma once
#include "message.h"
#include <cstdlib>

class Conn {
public:
  virtual void read(Message &msg) = 0;
  virtual void write(const Message &msg) = 0;
  static Conn *create(bool create, int conn_id);
  virtual ~Conn() = default;
};
