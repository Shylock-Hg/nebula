
// Copyright (c) 2023 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#include "common/observer/FileModifyObserver.h"

#include <iostream>

namespace nebula {

bool FileModifyObserver::modified() {
  int inotifyLength, inotifyI = 0;
  char inotifyBuffer[kBufLen];

  inotifyLength = ::read(fd_, inotifyBuffer, kBufLen);

  if (inotifyLength < 0) {
    return false;
  }
  while (inotifyI < inotifyLength) {
    struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&inotifyBuffer[inotifyI]);

    // Check event
    if (event->mask & IN_CLOSE_WRITE) {
      if (event->mask & IN_ISDIR) {
        // Ignore directory change
      } else {
        return true;
      }
    }
    inotifyI += kEventSize + event->len;
  }
  return false;
}

}  // namespace nebula
