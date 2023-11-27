#pragma once
#include "conn.h"
#include <string>

class ConnShm : public Conn {
public:
  ConnShm(bool create, const std::string &name);
  ~ConnShm();

  inline void read(Message &msg) override;
  inline void write(const Message &msg) override;

private:
  std::string shm_name;
  void *mem;
  const size_t mem_size = 4096;
};
