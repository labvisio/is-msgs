
#include "ostream.hpp"

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
  return os << google::protobuf::util::TimeUtil::ToString(is::to_timestamp(time_point));
}

}  // namespace chrono
}  // namespace std
