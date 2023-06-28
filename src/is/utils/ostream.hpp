
#pragma once

#include <chrono>
#include <ostream>
#include "timestamp.hpp"

namespace google {
namespace protobuf {
std::ostream& operator<<(std::ostream& os, google::protobuf::Message const& m);
}  // namespace protobuf
}  // namespace google

namespace std {
namespace chrono {
std::ostream& operator<<(std::ostream& os, time_point<system_clock> const& time_point);

template <typename rep, typename period>
std::ostream& operator<<(std::ostream& os, duration<rep, period> const& d) {
  return os << google::protobuf::util::TimeUtil::ToString(is::to_duration(d));
}
}  // namespace chrono
}  // namespace std
