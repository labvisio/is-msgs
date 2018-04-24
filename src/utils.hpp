#pragma once

#include <google/protobuf/util/time_util.h>
#include <sys/time.h>
#include "common.pb.h"

namespace is {

namespace pb {
using namespace google::protobuf::util;
using namespace google::protobuf;
}  // namespace pb

pb::Timestamp current_time();
common::Status make_status(common::StatusCode code, std::string const& why = "");

}  // namespace is
