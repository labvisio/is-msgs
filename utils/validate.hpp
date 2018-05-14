
#pragma once

#include "status.hpp"
#include "validate.pb.h"

namespace is {

wire::Status validate_message(google::protobuf::Message const& message);


}  // namespace is