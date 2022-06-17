/* Copyright (c) 2022 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#pragma once

#include "common/expression/Expression.h"

namespace nebula {

class PatternComprehensionExpression final : public Expression {
  friend class Expression;

 public:
  PatternComprehensionExpression& operator=(const PatternComprehensionExpression& rhs) = delete;
  PatternComprehensionExpression& operator=(PatternComprehensionExpression&&) = delete;

  static PatternComprehensionExpression* make(ObjectPool* pool,
                                              std::unique_ptr<MatchPath>&& matchPath,
                                              Expression* val = nullptr,
                                              const std::string& variable = "",
                                              Expression* filter = nullptr) {
    return pool->add(
        new PatternComprehensionExpression(pool, std::move(matchPath), val, variable, filter));
  }

  bool operator==(const Expression& rhs) const override;

  const Value& eval(ExpressionContext& ctx) override;

  std::string toString() const override;

  std::string rawString() const override {
    return toString();
  }

  void accept(ExprVisitor* visitor) override;

  Expression* clone() const override;

  const MatchPath& matchPath() const {
    return matchPath_;
  }

  Expression* val() {
    return val_;
  }

  const std::string& variable() const {
    return variable_;
  }

  Expression* filter() {
    return filter_;
  }

  void setPattern(MatchPath matchPath) {
    matchPath_ = std::move(matchPath);
  }

  void setVal(Expression* val) {
    val_ = val;
  }

  void setVariable(std::string variable) {
    variable_ = std::move(variable);
  }

  void setFilter(Expression* expr) {
    filter_ = expr;
  }

  bool hasVariable() const {
    return variable_ != nullptr;
  }

  bool hasFilter() const {
    return filter_ != nullptr;
  }

 private:
  // [ variable = (v)-[:like]->() WHERE v.name = 'Tim Duncan' | v.name ]
  explicit PatternComprehensionExpression(ObjectPool* pool,
                                          std::unique_ptr<MatchPath>&& matchPath,
                                          Expression* val = nullptr,
                                          const std::string& variable = "",
                                          Expression* filter = nullptr)
      : Expression(pool, Kind::kPatternComprehension),
        matchPath_(std::move(matchPath)),
        val_(val),
        variable_(variable),
        filter_(filter) {}

  void writeTo(Encoder& encoder) const override;

  void resetFrom(Decoder& decoder) override;

 private:
  std::unique_ptr<MatchPath> matchPath_;
  Expression* val_{nullptr};
  std::string variable_;         // variable_ is optional
  Expression* filter_{nullptr};  // filter_ is optional

  Value result_;
};

}  // namespace nebula
