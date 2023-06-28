
#include "timestamp.hpp"

namespace is {

google::protobuf::Timestamp to_timestamp(system_clock::time_point const& tp) {
  google::protobuf::Timestamp timestamp;
  auto nanos = duration_cast<nanoseconds>(tp.time_since_epoch()).count();
  timestamp.set_seconds(nanos / 1000000000);
  timestamp.set_nanos(nanos % 1000000000);
  return timestamp;
}

google::protobuf::Duration to_duration(system_clock::duration const& d) {
  google::protobuf::Duration duration;
  auto nanos = duration_cast<nanoseconds>(d).count();
  duration.set_seconds(nanos / 1000000000);
  duration.set_nanos(nanos % 1000000000);
  return duration;
}

system_clock::time_point to_system_clock(google::protobuf::Timestamp const& timestamp) {
  long long nanos = timestamp.seconds() * 1000000000 + timestamp.nanos();
  return system_clock::time_point{} + nanoseconds{nanos};
}

nanoseconds to_nanoseconds(google::protobuf::Duration const& duration) {
  long long nanos = duration.seconds() * 1000000000 + duration.nanos();
  return nanoseconds{nanos};
}

}  // namespace is
