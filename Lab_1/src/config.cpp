#include "config.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <syslog.h>
#include <vector>

namespace fs = std::filesystem;

Config::Config(const std::string &path) { read_config(path); }

void Config::read_config(const std::string &path) {
  std::ifstream f(path);
  f >> dir >> depth;

  if (f.eof())
    syslog(LOG_NOTICE, "Wrong config format");
}

std::filesystem::path Config::get_dir() const { return dir; }

int Config::get_depth() const { return depth; }
