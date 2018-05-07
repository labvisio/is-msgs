
#include "ostream-ext.hpp"

namespace google {
namespace protobuf {
std::ostream& operator<<(std::ostream& os, google::protobuf::Message const& m) {
  return os << m.ShortDebugString();
}

}  // namespace protobuf
}  // namespace google

namespace std {
namespace chrono {
std::ostream& operator<<(std::ostream& os, time_point<system_clock> const& time_point) {
  const auto nanos = time_point.time_since_epoch().count();
  google::protobuf::Timestamp timestamp;
  timestamp.set_seconds(nanos / 1000000000);
  timestamp.set_nanos(nanos % 1000000000);
  return os << google::protobuf::util::TimeUtil::ToString(timestamp);
}

}  // namespace chrono
}  // namespace std
