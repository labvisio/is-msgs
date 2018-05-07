
#pragma once

#include <chrono>
#include <ostream>
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

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
  const auto nanos = duration_cast<nanoseconds>(d).count();
  const auto duration = google::protobuf::util::TimeUtil::NanosecondsToDuration(nanos);
  return os << google::protobuf::util::TimeUtil::ToString(duration);
}
}  // namespace chrono
}  // namespace std
