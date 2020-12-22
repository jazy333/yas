#pragma once
#include <string>
#include <vector>

namespace yas {
class DB {
 public:
  virtual int open(const std::string& path) = 0;
  virtual int close() = 0;
  virtual int get(const std::string& key, std::string& value) = 0;
  virtual int set(const std::string& key, const std::string& value) = 0;
  virtual int del(const std::string& key) = 0;
  virtual size_t size() = 0;
  virtual ~DB() = default;
};
}  // namespace yas