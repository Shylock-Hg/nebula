# Copyright (c) 2023 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License.

nebula_add_library(
    observer_obj
    OBJECT
    FileModifyObserver.cpp
)

nebula_add_subdirectory(test)
