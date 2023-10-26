#include "daemon.h"
#include "config.h"
#include <csignal>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

namespace fs = std::filesystem;

std::string Daemon::config_path;
Config Daemon::config;

void Daemon::signal_handler(int sig) {
  switch (sig) {
  case SIGHUP:
    config.read_config(config_path);
    break;

  case SIGTERM:
    syslog(LOG_NOTICE, "Daemon closed");
    closelog();
    exit(EXIT_SUCCESS);

  default:
    break;
  }
}

Daemon::Daemon(const std::string &config_path) {
  this->config_path = fs::absolute(config_path);
  config = Config(config_path);
}

Daemon &Daemon::create_daemon(const std::string &config_path) {
  static Daemon instance(config_path);
  return instance;
}

void Daemon::make_daemon() const {
  std::signal(SIGHUP, signal_handler);
  std::signal(SIGTERM, signal_handler);

  if (fork() != 0)
    exit(EXIT_FAILURE);
  if (setsid() < 0)
    exit(EXIT_FAILURE);
  if (fork() != 0)
    exit(EXIT_FAILURE);
  umask(0);
  if (chdir("/") != 0)
    exit(EXIT_FAILURE);

  for (long x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
    close(x);
  openlog(process_name.c_str(), LOG_PID, LOG_DAEMON);

  std::ofstream f(pid_file, std::ios_base::trunc);
  f << getpid();
}

void Daemon::run() {
  close_daemon();
  make_daemon();
  syslog(LOG_NOTICE, "Daemon started");

  while (true) {
    fs::path dir = config.get_dir();
    int depth = config.get_depth();

    std::vector<fs::path> dirs_for_deletion;

    auto iter = fs::recursive_directory_iterator(dir);
    for (const auto &entry : iter) {
      if (iter.depth() == depth && entry.is_directory())
        dirs_for_deletion.push_back(entry);
    }

    for (const auto &entry : dirs_for_deletion) {
      fs::remove_all(entry);
    }

    sleep(time_interval);
  }
}

void Daemon::close_daemon() {
  std::ifstream f(pid_file);
  int pid;
  f >> pid;
  if (fs::exists("/proc/" + std::to_string(pid)))
    kill(pid, SIGTERM);
}