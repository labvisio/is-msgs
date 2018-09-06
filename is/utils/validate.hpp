
#pragma once

#include "is/msgs/validate.pb.h"

namespace is {

void validate_message(google::protobuf::Message const& message);

}  // namespace is