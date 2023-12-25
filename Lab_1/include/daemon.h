#pragma once
#include "config.h"
#include <filesystem>

class Daemon {
public:
  static Daemon &create_daemon(const std::string &config_path);
  void run();
  Daemon() = delete;
  Daemon(const Daemon &) = delete;
  Daemon(Daemon &&) = delete;

protected:
  static void signal_handler(int sig);
  void close_daemon();
  void make_daemon() const;

private:
  Daemon(const std::string &config_path);

  static std::string config_path;
  static Config config;
  unsigned int time_interval = 60;
  std::string process_name = "mydaemon";
  std::string pid_file = std::filesystem::absolute("mydaemon.pid");
};
