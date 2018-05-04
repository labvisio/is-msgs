#pragma once

#include <google/protobuf/util/time_util.h>
#include <sys/time.h>
#include "wire.pb.h"

namespace is {

namespace pb {
using namespace google::protobuf::util;
using namespace google::protobuf;
}  // namespace pb

pb::Timestamp current_time();
wire::Status make_status(wire::StatusCode code, std::string const& why = "");

}  // namespace is
