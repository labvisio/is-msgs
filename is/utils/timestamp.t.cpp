#include <chrono>
#include "gtest/gtest.h"
#include "utils.hpp"

namespace {

TEST(TimestampTests, Conversions) {
  using namespace std::chrono;

  auto now = std::chrono::system_clock::now();
  ASSERT_EQ(is::to_system_clock(is::to_timestamp(now)), now);

  auto duration = now - std::chrono::system_clock::now() - std::chrono::seconds(60);
  ASSERT_EQ(is::to_nanoseconds(is::to_duration(duration)), duration_cast<nanoseconds>(duration));
}

}  // namespace