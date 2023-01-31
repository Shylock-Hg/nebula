// Copyright (c) 2023 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#include <fcntl.h>
#include <folly/synchronization/Baton.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>

#include <thread>

#include "common/observer/FileModifyObserver.h"

namespace nebula {

class FileModifyObserverTest : public ::testing::Test {
 protected:
  void SetUp() override {
    auto fd = ::open(f1_.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    ::close(fd);
    fd = ::open(f2_.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    ::close(fd);
    fd = ::open(f3_.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    ::close(fd);
  }

  void TearDown() override {
    ::unlink(f1_.c_str());
    ::unlink(f2_.c_str());
    ::unlink(f3_.c_str());
  }

  std::string f1_{"/tmp/file_modify_observer_test.1"};
  std::string f2_{"/tmp/file_modify_observer_test.2"};
  std::string f3_{"/tmp/file_modify_observer_test.3"};
};

TEST_F(FileModifyObserverTest, OBOneFile) {
  FileModifyObserver ob({f1_});
  bool m = false;
  std::thread t([&ob, &m]() { m = ob.modified(); });

  ::sleep(3);
  ASSERT_EQ(m, false);

  // modify
  auto fd = ::open(f1_.c_str(), O_CREAT | O_WRONLY | O_TRUNC);
  ::write(fd, "hello world", sizeof("hello world"));
  ::close(fd);

  ::sleep(3);
  ASSERT_EQ(m, true);

  t.join();
}

TEST_F(FileModifyObserverTest, OBMultiFiles) {
  FileModifyObserver ob({f1_, f2_, f3_});
  bool m = false;
  std::thread t([&ob, &m]() { m = ob.modified(); });

  ::sleep(3);
  ASSERT_EQ(m, false);

  // modify
  auto fd = ::open(f1_.c_str(), O_CREAT | O_WRONLY | O_TRUNC);
  ::write(fd, "hello world", sizeof("hello world"));
  ::close(fd);

  ::sleep(3);
  ASSERT_EQ(m, true);

  t.join();
}

}  // namespace nebula
