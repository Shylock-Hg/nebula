/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#include <gtest/gtest.h>

#include "common/base/Base.h"
#include "common/time/TimezoneInfo.h"
#include "common/time/parser/DatetimeReader.h"

namespace nebula {

TEST(DatetimeReader, DateTime) {
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:3.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 3, 22, 22, 3, 233300), result.value());
  }
  // with offset
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:3.2333+02:30");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 3, 19, 52, 3, 233300), result.value());
  }
  // lack day
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01T22:22:3.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 3, 233300), result.value());
  }
  // lack month
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019T22:22:3.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 3, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019T22:22");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019T22");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 0, 0, 0), result.value());
  }
  // lack us
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-1T22:22:3");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 3, 0), result.value());
  }
  // lack second
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-1T22:22");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 0, 0), result.value());
  }
  // lack minute
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-1T22");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 0, 0, 0), result.value());
  }
  // datetime just include date
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-1-1");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 0, 0, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-1");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 0, 0, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 0, 0, 0, 0), result.value());
  }
  // without delimiter
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("20190103T222203.2333+2200");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 3, 20, 22, 3, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("20190103T222203.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 3, 22, 22, 3, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("20190103T2222.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 3, 22, 22, 0, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("201901T2222.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 0, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("201901T2222");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 1, 1, 22, 22, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("20190203");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 2, 3, 0, 0, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("201902");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2019, 2, 1, 0, 0, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("02");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::DateTime(2, 1, 1, 0, 0, 0, 0), result.value());
  }
}

TEST(DatetimeReader, DateTimeFailed) {
  // out of range offset
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:3.2333+24:30");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // only time
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("22:22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // with unexpected character
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:3.2333x");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:3.2333\n");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("\t2019-01-03T22:22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not ending delimiter
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-T22:22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not ending prefix
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("2019-01-03T22:22:3.");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not exits prefix
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDatetime("-01-03T22:22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
}

TEST(DatetimeReader, Date) {
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01-03");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(2019, 1, 3), result.value());
  }
  // lack day
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(2019, 1, 1), result.value());
  }
  // lack month and day
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(2019, 1, 1), result.value());
  }
  // lack month
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(2019, 1, 1), result.value());
  }
  // Without delimiter
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("20190103");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(2019, 1, 3), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("201901");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(2019, 1, 1), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("01");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Date(1, 1, 1), result.value());
  }
}

TEST(DatetimeReader, DateFailed) {
  // don't support offset
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01-03+23:00");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // with unexpected character
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01-03*");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01-03\n");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("\t2019-01-03");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // extra components
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01-03T22:22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not ending delimiter
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-01-");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019-");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not exits prefix
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("-01-03");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // without delimiter and not aligned
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("2019013");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readDate("20190");
    EXPECT_FALSE(result.ok()) << result.value();
  }
}

TEST(DatetimeReader, Time) {
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 22, 3, 233300), result.value());
  }
  // with offset
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.2333-03:30");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(1, 52, 3, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.2333-03:30");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(1, 52, 3, 233300), result.value());
  }
  // lack us
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 22, 3, 0), result.value());
  }
  // lack second
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 22, 0, 0), result.value());
  }
  // lack minute
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 0, 0, 0), result.value());
  }
  // without delimiter
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("222203.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 22, 3, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("2222.2333");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 22, 0, 233300), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("2222");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(22, 22, 0, 0), result.value());
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("2222+2200");
    ASSERT_TRUE(result.ok()) << result.status();
    EXPECT_EQ(nebula::Time(20, 22, 0, 0), result.value());
  }
}

TEST(DatetimeReader, TimeFailed) {
  // out of range offset
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.2333-03:60");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // unexpected character
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.2333x");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.2333\n");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("\t22:22:3.2333\n");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // extra components
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("2019-01-03T22:22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not ending delimiter
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not ending prefix
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22:22:3.");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // not exist prefix
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime(":22:3.2333");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  // without delimtier and not aligned
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("22223");
    EXPECT_FALSE(result.ok()) << result.value();
  }
  {
    auto parser = time::DatetimeReader();
    auto result = parser.readTime("222");
    EXPECT_FALSE(result.ok()) << result.value();
  }
}

}  // namespace nebula

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  folly::init(&argc, &argv, true);
  google::SetStderrLogging(google::INFO);
  auto result = nebula::time::Timezone::initializeGlobalTimezone();
  if (!result.ok()) {
    LOG(FATAL) << result;
  }

  DLOG(INFO) << "Timezone: " << nebula::time::Timezone::getGlobalTimezone().stdZoneName();
  DLOG(INFO) << "Timezone offset: " << nebula::time::Timezone::getGlobalTimezone().utcOffsetSecs();

  return RUN_ALL_TESTS();
}
