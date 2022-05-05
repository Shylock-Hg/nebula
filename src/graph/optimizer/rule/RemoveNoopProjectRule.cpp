/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#include "graph/optimizer/rule/RemoveNoopProjectRule.h"

#include "graph/optimizer/OptContext.h"
#include "graph/optimizer/OptGroup.h"
#include "graph/planner/plan/PlanNode.h"
#include "graph/planner/plan/Query.h"

using nebula::graph::PlanNode;
using nebula::graph::QueryContext;

namespace nebula {
namespace opt {

std::unique_ptr<OptRule> RemoveNoopProjectRule::kInstance =
    std::unique_ptr<RemoveNoopProjectRule>(new RemoveNoopProjectRule());

RemoveNoopProjectRule::RemoveNoopProjectRule() {
  RuleSet::QueryRules().addRule(this);
}

const Pattern& RemoveNoopProjectRule::pattern() const {
  static Pattern pattern = Pattern::create(graph::PlanNode::Kind::kProject);
  return pattern;
}

StatusOr<OptRule::TransformResult> RemoveNoopProjectRule::transform(
    OptContext* octx, const MatchedResult& matched) const {
  const auto* projGroupNode = matched.node;
  const auto* oldProjNode = projGroupNode->node();
  DCHECK_EQ(oldProjNode->kind(), PlanNode::Kind::kProject);

  TransformResult result;
  result.eraseAll = true;
  const auto* projGroup = projGroupNode->group();
  const auto depGroups = projGroupNode->dependencies();
  DCHECK_EQ(depGroups.size(), 1);
  const auto* depGroup = depGroups.front();
  const auto& groupNodes = depGroup->groupNodes();
  for (auto* groupNode : groupNodes) {
    auto* newNode = groupNode->node()->clone();
    newNode->setOutputVar(oldProjNode->outputVar());
    auto* newGroupNode = OptGroupNode::create(octx, newNode, projGroup);
    newGroupNode->setDeps(groupNode->dependencies());
    result.newGroupNodes.emplace_back(newGroupNode);
  }

  return result;
}

bool RemoveNoopProjectRule::match(OptContext* octx, const MatchedResult& matched) const {
  if (!OptRule::match(octx, matched)) {
    return false;
  }

  auto* projGroupNode = matched.node;
  DCHECK_EQ(projGroupNode->node()->kind(), PlanNode::Kind::kProject);
  auto depGroups = projGroupNode->dependencies();

  // handled in Pattern::match
  DCHECK_EQ(depGroups.size(), 1);
  auto* node = depGroups.front()->groupNodes().front()->node();
  auto kind = node->kind();
  // disable BinaryInputNode/SetOp (multi input)
  // disable IndexScan/PassThrough (multi output)
  if (!node->isSingleInput() || kind == PlanNode::Kind::kUnion || kind == PlanNode::Kind::kMinus ||
      kind == PlanNode::Kind::kIntersect || kind == PlanNode::Kind::kFilter ||
      kind == PlanNode::Kind::kPassThrough || kind == PlanNode::Kind::kLimit ||
      kind == PlanNode::Kind::kDedup) {
    return false;
  }

  auto* projNode = static_cast<const graph::Project*>(projGroupNode->node());
  std::vector<YieldColumn*> cols = projNode->columns()->columns();
  for (auto* col : cols) {
    if (col->expr()->kind() != Expression::Kind::kVarProperty &&
        col->expr()->kind() != Expression::Kind::kInputProperty) {
      return false;
    }
  }
  const auto* depNode = node;
  const auto& depColNames = depNode->colNames();
  const auto& projColNames = projNode->colNames();
  auto colsNum = depColNames.size();
  if (colsNum != projColNames.size()) {
    return false;
  }
  for (size_t i = 0; i < colsNum; ++i) {
    if (depColNames[i].compare(projColNames[i])) {
      return false;
    }
    const auto* propExpr = static_cast<PropertyExpression*>(cols[i]->expr());
    if (propExpr->prop() != projColNames[i]) {
      return false;
    }
  }

  return true;
}

std::string RemoveNoopProjectRule::toString() const {
  return "RemoveNoopProjectRule";
}

}  // namespace opt
}  // namespace nebula
