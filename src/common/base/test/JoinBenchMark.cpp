// Copyright (c) 2022 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#include <folly/Benchmark.h>
#include <folly/init/Init.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "common/base/JoinHashTable.h"
#include "common/datatypes/List.h"
#include "common/datatypes/Value.h"

using nebula::List;
using nebula::Value;

BENCHMARK(std_1k, iters) {
  constexpr std::size_t kData = 1000;
  for (auto i = 0u; i < iters; i++) {
    std::unordered_map<std::size_t, std::size_t> t;
    t.reserve(kData);
    for (std::size_t j = 0; j < kData; j++) {
      t[j] = j;
    }
    for (std::size_t j = 0; j < kData; j++) {
      auto found = t.find(j);
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_RELATIVE(JoinHashTable_1k, iters) {
  constexpr std::size_t kData = 1000;
  for (auto i = 0u; i < iters; i++) {
    JoinHashTable<std::size_t, std::size_t> t(kData);
    for (std::size_t j = 0; j < kData; j++) {
      t[j] = j;
    }
    for (std::size_t j = 0; j < kData; j++) {
      auto found = t.find(j);
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_DRAW_LINE();

BENCHMARK(std_10k, iters) {
  constexpr std::size_t kData = 10000;
  for (auto i = 0u; i < iters; i++) {
    std::unordered_map<std::size_t, std::size_t> t;
    t.reserve(kData);
    for (std::size_t j = 0; j < kData; j++) {
      t[j] = j;
    }
    for (std::size_t j = 0; j < kData; j++) {
      auto found = t.find(j);
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_RELATIVE(JoinHashTable_10k, iters) {
  constexpr std::size_t kData = 10000;
  for (auto i = 0u; i < iters; i++) {
    JoinHashTable<std::size_t, std::size_t> t(kData);
    for (std::size_t j = 0; j < kData; j++) {
      t[j] = j;
    }
    for (std::size_t j = 0; j < kData; j++) {
      auto found = t.find(j);
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_DRAW_LINE();

BENCHMARK(std_10k_value_int, iters) {
  constexpr int64_t kData = 10000;
  for (auto i = 0u; i < iters; i++) {
    std::unordered_map<Value, std::vector<const List*>> t;
    t.reserve(kData);
    for (int64_t j = 0; j < kData; j++) {
      t[Value(j)].emplace_back(nullptr);
    }
    for (int64_t j = 0; j < kData; j++) {
      auto found = t.find(Value(j));
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_RELATIVE(JoinHashTable_10k_value_int, iters) {
  constexpr int64_t kData = 10000;
  for (auto i = 0u; i < iters; i++) {
    JoinHashTable<Value, std::vector<const List*>> t(kData);
    for (int64_t j = 0; j < kData; j++) {
      t[Value(j)].emplace_back(nullptr);
    }
    for (int64_t j = 0; j < kData; j++) {
      auto found = t.find(Value(j));
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_DRAW_LINE();

BENCHMARK(std_10k_value_string, iters) {
  constexpr int64_t kData = 10000;
  for (auto i = 0u; i < iters; i++) {
    std::unordered_map<Value, std::vector<const List*>> t;
    t.reserve(kData);
    for (int64_t j = 0; j < kData; j++) {
      t[Value(std::to_string(j))].emplace_back(nullptr);
    }
    for (int64_t j = 0; j < kData; j++) {
      auto found = t.find(Value(std::to_string(j)));
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_RELATIVE(JoinHashTable_10k_value_string, iters) {
  constexpr int64_t kData = 10000;
  for (auto i = 0u; i < iters; i++) {
    JoinHashTable<Value, std::vector<const List*>> t(kData);
    for (int64_t j = 0; j < kData; j++) {
      t[Value(std::to_string(j))].emplace_back(nullptr);
    }
    for (int64_t j = 0; j < kData; j++) {
      auto found = t.find(Value(std::to_string(j)));
      folly::doNotOptimizeAway(found);
    }
  }
}

BENCHMARK_DRAW_LINE();

int main(int argc, char** argv) {
  folly::init(&argc, &argv, true);

  folly::runBenchmarks();
  return 0;
}
