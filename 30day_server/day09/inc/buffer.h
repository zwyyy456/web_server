#pragma once

#include <string>
#include <sys/types.h>

class Buffer {
  private:
    std::string buf_;

  public:
    Buffer();
    ~Buffer();

    void Append(const char *str, int size);
    auto size() -> ssize_t;
    auto c_str() -> const char *;

    void Clear();
    void getline();
};