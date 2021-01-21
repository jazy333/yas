#pragma once
#include <string>
#include <vector>
#include <memory>

namespace yas {
class DB {
 public:
  virtual int open(const std::string& path) = 0;
  virtual int close() = 0;
  virtual int get(const std::string& key, std::string& value) = 0;
  virtual int set(const std::string& key, const std::string& value,
                  bool override = true) = 0;
  virtual int del(const std::string& key) = 0;
  virtual int test(const std::string& key) = 0;
  virtual int sync() = 0;
  virtual size_t size() = 0;
  virtual int rebuild() = 0;
  virtual std::string path() = 0;
  virtual bool should_be_rebuilt() = 0;
  virtual std::string status() = 0;
  virtual ~DB() = default;
  class Iterator {
   public:
    virtual ~Iterator() = default;
    virtual int first() = 0;
    virtual int next() = 0;
    virtual int last() = 0;
    virtual int get(std::string& key, std::string& value) = 0;
  };
  virtual std::unique_ptr<Iterator> make_iterator() = 0;
};
}  // namespace yas