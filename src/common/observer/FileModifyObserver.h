// Copyright (c) 2023 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#pragma once

#include <folly/Format.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace nebula {

class FileModifyObserver {
 public:
  explicit FileModifyObserver(const std::vector<std::string> &files) : fd_(::inotify_init()) {
    if (fd_ < 0) {
      throw std::runtime_error(folly::sformat("Failed to init inotify: {}.", strerror(errno)));
    }
    for (const auto &file : files) {
      auto wd = ::inotify_add_watch(fd_, file.c_str(), IN_CLOSE_WRITE);
      if (wd < 0) {
        ::close(fd_);
        throw std::runtime_error(
            folly::sformat("Failed to add inotify watch: {}.", strerror(errno)));
      }
    }
  }

  ~FileModifyObserver() {
    // Will clean watches too.
    ::close(fd_);
  }

  // return true when any file is modified.
  bool modified();

 private:
  int fd_{-1};

  static constexpr size_t kEventSize = sizeof(struct inotify_event);
  static constexpr size_t kBufLen = 1024 * (kEventSize + 16);
};

}  // namespace nebula
