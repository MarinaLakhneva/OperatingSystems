#pragma once

#include "conn.h"

#include <string>

class ConnFifo : public Conn {
private:
  std::string name;
  int descriptor = -1;

public:
  ConnFifo(const std::string &name, bool create);
  ~ConnFifo();

  inline void read(Message &msg) override;
  inline void write(const Message &msg) override;
};