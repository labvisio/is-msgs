
#pragma once

#include <chrono>
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

namespace is {

using namespace std::chrono;

google::protobuf::Timestamp to_timestamp(system_clock::time_point const& tp);
google::protobuf::Duration to_duration(system_clock::duration const& duration);

system_clock::time_point to_system_clock(google::protobuf::Timestamp const& timestamp);
nanoseconds to_nanoseconds(google::protobuf::Duration const& duration);

}  // namespace is