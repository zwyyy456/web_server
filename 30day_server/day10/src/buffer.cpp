#include "../inc/buffer.h"
#include <cstring>
#include <iostream>
#include <sys/types.h>

Buffer::Buffer() {
}

Buffer::~Buffer() {
}

void Buffer::Append(const char *str, int size) {
    for (int i = 0; i < size; ++i) {
        if (str[i] == '\0') {
            break;
        }
        buf_.push_back(str[i]);
    }
}

auto Buffer::size() -> ssize_t {
    return buf_.size();
}

const char *Buffer::c_str() {
    return buf_.c_str();
}

void Buffer::Clear() {
    buf_.clear();
}

void Buffer::getline() {
    buf_.clear();
    std::getline(std::cin, buf_);
}