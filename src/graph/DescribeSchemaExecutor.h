/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#pragma once

#include "graph/Executor.h"

namespace nebula {
namespace graph {

class DescribeSchemaExecutor : public Executor {
protected:
    DescribeSchemaExecutor(ExecutionContext* ectx, const std::string& name) : Executor(ectx, name) {}

    static const char* keyTypeToString(::nebula::cpp2::KeyType type) {
        switch (type) {
            case ::nebula::cpp2::KeyType::PRI:
                return "PRI";
            case ::nebula::cpp2::KeyType::UNI:
                return "UNI";
            case ::nebula::cpp2::KeyType::MUL:
                return "MUL";
        }
        return nullptr;
    }

    // convert nebula value to string
    static std::string value2String(const ::nebula::cpp2::Value& value) {
        switch (value.getType()) {
            case ::nebula::cpp2::Value::Type::bool_value: {
                //return value.get_bool_value() ? "True" : "False";
                return std::to_string(value.get_bool_value());
            }
            case ::nebula::cpp2::Value::Type::int_value: {
                return std::to_string(value.get_int_value());
            }
            case ::nebula::cpp2::Value::Type::double_value: {
                //col.set_double_precision(value.get_double_value());
                return std::to_string(value.get_double_value());
            }
            case ::nebula::cpp2::Value::Type::string_value: {
                //col.set_str(value.get_string_value());
                //break;
                return value.get_string_value();
            }
            case ::nebula::cpp2::Value::Type::timestamp: {
                //col.set_timestamp(value.get_timestamp());
                //break;
                return std::to_string(value.get_timestamp());
            }
            case ::nebula::cpp2::Value::Type::__EMPTY__: {
//                break;
                return std::string();

            }
        }
        return std::string();
    }

    static void setColumnValue(cpp2::ColumnValue& col, const ::nebula::cpp2::Value& value) {
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
                break;
            }
        }
    }

    // Describe schema header
    const std::vector<std::string> header_{"Field", "Type", "Null", "Key", "Default", "Extra"};
};

}  // namespace graph
}  // namespace nebula
