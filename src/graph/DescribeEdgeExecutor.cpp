/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "base/Base.h"
#include "graph/DescribeEdgeExecutor.h"

namespace nebula {
namespace graph {

DescribeEdgeExecutor::DescribeEdgeExecutor(Sentence *sentence,
                                           ExecutionContext *ectx)
    : DescribeSchemaExecutor(ectx, "describe_edge") {
    sentence_ = static_cast<DescribeEdgeSentence*>(sentence);
}


Status DescribeEdgeExecutor::prepare() {
    return Status::OK();
}


void DescribeEdgeExecutor::execute() {
    auto status = checkIfGraphSpaceChosen();
    if (!status.ok()) {
        doError(std::move(status));
        return;
    }
    auto *name = sentence_->name();
    auto spaceId = ectx()->rctx()->session()->space();

    // Get the lastest ver
    auto future = ectx()->getMetaClient()->getEdgeSchema(spaceId, *name);
    auto *runner = ectx()->rctx()->runner();

    auto cb = [this] (auto &&resp) {
        if (!resp.ok()) {
            doError(Status::Error("Describe edge `%s' failed.",
                        sentence_->name()->c_str()));
            return;
        }

        resp_ = std::make_unique<cpp2::ExecutionResponse>();
        resp_->set_column_names(header_);
        std::vector<cpp2::RowValue> rows;
        for (auto& item : resp.value().columns) {
            std::vector<cpp2::ColumnValue> row;
            row.resize(6);
            row[0].set_str(item.name); // Field
            row[1].set_str(valueTypeToString(item.type));  // Type
            // Null
            if (item.get_could_null() != nullptr) {
                row[2].set_bool_val(item.get_could_null());
            }
            // Key
            if (item.get_key_type() != nullptr) {
                auto keyType = *item.get_key_type();
                auto keyTypeName = keyTypeToString(keyType);
                if (keyTypeName != nullptr) {
                    row[3].set_str(keyTypeName);
                }
            }
            // Default
            if (item.get_default_value() != nullptr) {
                //setColumnValue(row[4], *item.get_default_value());
                // covert to characters for the column require same type
                row[4].set_str(value2String(*item.get_default_value()));
            }
            // row[5].set_str("");  // Extra TODO(shylock) reserved now
            rows.emplace_back();
            rows.back().set_columns(std::move(row));
        }

        resp_->set_rows(std::move(rows));
        doFinish(Executor::ProcessControl::kNext);
    };

    auto error = [this] (auto &&e) {
        auto msg = folly::stringPrintf("Describe edge `%s' exception: %s.",
                sentence_->name()->c_str(), e.what().c_str());
        LOG(ERROR) << msg;
        doError(Status::Error(std::move(msg)));
    };

    std::move(future).via(runner).thenValue(cb).thenError(error);
}


void DescribeEdgeExecutor::setupResponse(cpp2::ExecutionResponse &resp) {
    resp = std::move(*resp_);
}

}   // namespace graph
}   // namespace nebula
