# Copyright (c) 2023 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License.

nebula_add_test(
    NAME
        file_modify_observer_test
    SOURCES
        FileModifyObserverTest.cpp
    OBJECTS
        $<TARGET_OBJECTS:observer_obj>
    LIBRARIES
        gtest
        gtest_main
)
