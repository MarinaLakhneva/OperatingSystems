#pragma once
#include <filesystem>
#include <string>
#include <vector>

class Config {
public:
  Config() = default;
  Config(const std::string &path);
  void read_config(const std::string &path);
  std::filesystem::path get_dir() const;
  int get_depth() const;

private:
  std::filesystem::path dir;
  int depth;
};
