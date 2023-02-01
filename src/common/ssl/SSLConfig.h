
/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#ifndef COMMON_SSL_SSLCONFIG_H
#define COMMON_SSL_SSLCONFIG_H

#include <folly/experimental/observer/SimpleObservable.h>
#include <folly/io/async/SSLContext.h>
#include <gflags/gflags.h>
#include <wangle/ssl/SSLContextConfig.h>

#include <atomic>
#include <memory>
#include <thread>

DECLARE_bool(enable_ssl);
DECLARE_bool(enable_graph_ssl);
DECLARE_bool(enable_meta_ssl);

namespace nebula {

class SSLConfig {
 public:
  SSLConfig() = default;

  void init();

  ~SSLConfig() {
    stop();
  }

  void stop() {
    stop_.store(true);
    t_.join();
  }

  auto getObserver() {
    return sslConfigOb_.getObserver();
  }

  static std::shared_ptr<wangle::SSLContextConfig> sslContextConfig();

  static std::shared_ptr<folly::SSLContext> createSSLContext();

 private:
  std::thread t_{};
  std::atomic_bool stop_{false};

  folly::observer::SimpleObservable<wangle::SSLContextConfig> sslConfigOb_{};
};

}  // namespace nebula
#endif
