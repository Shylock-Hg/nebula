// Copyright (c) 2021 vesoft inc. All rights reserved.
//
// This source code is licensed under Apache 2.0 License.

#pragma once

#include "common/expression/ListComprehensionExpression.h"
#include "common/expression/PredicateExpression.h"
namespace nebula {

  class ListComprehensionExpression;
  class PredicateExpression;
  class ReduceExpression;

namespace graph {
  class QueryContext;
}

// TODO remove inner variable from global variable map
class InnerVariableUtil final {
 public:
  InnerVariableUtil() = delete;

  // Rewrites the list comprehension expression so that the innerVar_ will be replaced by a newly
  // generated variable.
  // This is to prevent duplicate variable names existing both inside and outside the expression.
  static void rewriteLC(graph::QueryContext *qctx,
                        ListComprehensionExpression *lc,
                        const std::string &oldVarName);

  // Rewrites the predicate comprehension expression so that the innerVar_ will be replaced by a
  // newly generated variable. This is to prevent duplicate variable names existing both inside and
  // outside the expression.
  static void rewritePred(graph::QueryContext *qctx,
                          PredicateExpression *pred,
                          const std::string &oldVarName);

  // Rewrites the list reduce expression so that the innerVar_ and the accumulator_ will be replaced
  // by newly generated variables. This is to prevent duplicate variable names existing both inside
  // and outside the expression.
  static void rewriteReduce(graph::QueryContext *qctx,
                            ReduceExpression *reduce,
                            const std::string &oldAccName,
                            const std::string &oldVarName);
};

}  // namespace nebula
