/* Copyright (c) 2022 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#include "common/expression/PatternComprehensionExpression.h"

#include "common/expression/ExprVisitor.h"

namespace nebula {

const Value& PatternComprehensionExpression::eval(ExpressionContext& ctx) {
  Pattern ret;

  auto& listVal = collection_->eval(ctx);
  if (listVal.isNull() || listVal.empty()) {
    result_ = listVal;
    return result_;
  }
  if (!listVal.isPattern()) {
    result_ = Value::kNullBadType;
    return result_;
  }

  auto& list = listVal.getPattern();

  if (filter_ == nullptr && mapping_ == nullptr) {
    result_ = std::move(list);
    return result_;
  }

  for (size_t i = 0; i < list.size(); ++i) {
    auto& v = list[i];
    ctx.setVar(innerVar_, v);
    if (filter_ != nullptr) {
      auto& filterVal = filter_->eval(ctx);
      if (!filterVal.empty() && !filterVal.isNull() && !filterVal.isBool()) {
        return Value::kNullBadType;
      }
      if (filterVal.empty() || filterVal.isNull() || !filterVal.getBool()) {
        continue;
      }
    }

    if (mapping_ != nullptr) {
      ret.emplace_back(mapping_->eval(ctx));
    } else {
      ret.emplace_back(v);
    }
  }

  result_ = std::move(ret);
  return result_;
}

Expression* PatternComprehensionExpression::clone() const {
  auto expr =
      PatternComprehensionExpression::make(pool_,
                                           innerVar_,
                                           collection_->clone(),
                                           filter_ != nullptr ? filter_->clone() : nullptr,
                                           mapping_ != nullptr ? mapping_->clone() : nullptr);
  if (hasOriginString()) {
    expr->setOriginString(originString_);
  }
  return expr;
}

bool PatternComprehensionExpression::operator==(const Expression& rhs) const {
  if (kind() != rhs.kind()) {
    return false;
  }

  const auto& expr = static_cast<const PatternComprehensionExpression&>(rhs);

  if (innerVar_ != expr.innerVar_) {
    return false;
  }

  if (*collection_ != *expr.collection_) {
    return false;
  }

  if (hasFilter() != expr.hasFilter()) {
    return false;
  }
  if (hasFilter()) {
    if (*filter_ != *expr.filter_) {
      return false;
    }
  }

  if (hasMapping() != expr.hasMapping()) {
    return false;
  }
  if (hasMapping()) {
    if (*mapping_ != *expr.mapping_) {
      return false;
    }
  }

  return true;
}

void PatternComprehensionExpression::writeTo(Encoder& encoder) const {
  encoder << kind_;
  encoder << Value(hasFilter());
  encoder << Value(hasMapping());
  encoder << Value(hasOriginString());

  encoder << innerVar_;
  encoder << *collection_;
  if (hasFilter()) {
    encoder << *filter_;
  }
  if (hasMapping()) {
    encoder << *mapping_;
  }
  if (hasOriginString()) {
    encoder << originString_;
  }
}

void PatternComprehensionExpression::resetFrom(Decoder& decoder) {
  bool hasFilter = decoder.readValue().getBool();
  bool hasMapping = decoder.readValue().getBool();
  bool hasString = decoder.readValue().getBool();

  innerVar_ = decoder.readStr();
  collection_ = decoder.readExpression(pool_);
  if (hasFilter) {
    filter_ = decoder.readExpression(pool_);
  }
  if (hasMapping) {
    mapping_ = decoder.readExpression(pool_);
  }
  if (hasString) {
    originString_ = decoder.readStr();
  }
}

std::string PatternComprehensionExpression::toString() const {
  std::string buf;
  buf.reserve(256);

  buf += "[";
  buf += innerVar_;
  buf += " IN ";
  buf += collection_ ? collection_->toString() : "";
  if (hasFilter()) {
    buf += " WHERE ";
    buf += filter_->toString();
  }
  if (hasMapping()) {
    buf += " | ";
    buf += mapping_->toString();
  }
  buf += "]";

  return buf;
}

void PatternComprehensionExpression::accept(ExprVisitor* visitor) {
  visitor->visit(this);
}

}  // namespace nebula
