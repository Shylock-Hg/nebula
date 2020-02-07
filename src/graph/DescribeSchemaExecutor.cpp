/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "DescribeSchemaExecutor.h"

namespace nebula {
namespace graph {

/*static*/ void DescribeSchemaExecutor::setColumnValue(cpp2::ColumnValue& col,
    const ::nebula::cpp2::Value& value) {
    switch (value.getType()) {
        case ::nebula::cpp2::Value::Type::bool_value: {
            col.set_bool_val(value.get_bool_value());
            break;
        }
        case ::nebula::cpp2::Value::Type::int_value: {
            col.set_integer(value.get_int_value());
            break;
        }
        case ::nebula::cpp2::Value::Type::double_value: {
            col.set_double_precision(value.get_double_value());
            break;
        }
        case ::nebula::cpp2::Value::Type::string_value: {
            col.set_str(value.get_string_value());
            break;
        }
        case ::nebula::cpp2::Value::Type::timestamp: {
            col.set_timestamp(value.get_timestamp());
            break;
        }
        case ::nebula::cpp2::Value::Type::__EMPTY__: {
            // TODO Default NULL if NULL type introduced
            col.set_str("NULL");
            break;
        }
    }
}

/*static*/ std::vector<cpp2::RowValue> DescribeSchemaExecutor::genRows(
    const std::vector<::nebula::cpp2::ColumnDef>& cols) {
    std::vector<cpp2::RowValue> rows;
    for (auto& item : cols) {
        std::vector<cpp2::ColumnValue> row;
        row.resize(6);
        // Field
        row[0].set_str(item.name);
        // Type
        row[1].set_str(valueTypeToString(item.type));
        // Null
        if (item.get_could_null() != nullptr) {
            row[2].set_bool_val(*item.get_could_null());
        } else {
            // For testing checking with one value
            row[2].set_bool_val(false);
        }
        // Key
        if (item.get_key_type() != nullptr) {
            auto keyType = *item.get_key_type();
            auto keyTypeName = keyTypeToString(keyType);
            row[3].set_str(keyTypeName);
        } else {
            // For testing checking with one value
            row[3].set_str(std::string());
        }
        // Default
        // TODO now the console require column has same type
        if (item.get_default_value() != nullptr) {
            setColumnValue(row[4], *item.get_default_value());
        } else {
            // TODO Default NULL if NULL type introduced
            row[4].set_str("NULL");
        }
        // Extra TODO(shylock) reserved now
        row[5].set_str(std::string());
        rows.emplace_back();
        rows.back().set_columns(std::move(row));
    }
    return rows;
}

}  // namespace graph
}  // namespace nebula
