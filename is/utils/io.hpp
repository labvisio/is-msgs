#pragma once

#include <fstream>
#include <string>
#include "google/protobuf/message.h"
#include "google/protobuf/util/json_util.h"
#include "status.hpp"

namespace is {
wire::Status load(std::string const& filename, google::protobuf::Message* message);
wire::Status save(std::string const& filename, google::protobuf::Message const& message);
}  // namespace is