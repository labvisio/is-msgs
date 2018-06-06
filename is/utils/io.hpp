#pragma once

#include <string>
#include "google/protobuf/message.h"
#include "status.hpp"

namespace is {
wire::Status load(std::string const& filename, google::protobuf::Message* message);
wire::Status save(std::string const& filename, google::protobuf::Message const& message);
}  // namespace is