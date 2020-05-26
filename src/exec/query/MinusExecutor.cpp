/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "exec/query/MinusExecutor.h"

#include "planner/PlanNode.h"

namespace nebula {
namespace graph {

folly::Future<Status> MinusExecutor::execute() {
    dumpLog();

    // TODO(yee):
    return start();
}

}   // namespace graph
}   // namespace nebula
