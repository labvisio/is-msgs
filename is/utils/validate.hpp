
#pragma once

#include "is/msgs/validate.pb.h"
#include "status.hpp"

namespace is {

wire::Status validate_message(google::protobuf::Message const& message);

}  // namespace is