#include "io.hpp"
#include "spdlog/fmt/fmt.h"

namespace is {

std::string file_extension(std::string const& filename) {
  auto last_dot = filename.find_last_of('.');
  return (last_dot < filename.size() && last_dot != std::string::npos)
             ? filename.substr(last_dot + 1)
             : "";
}

wire::Status load(std::string const& filename, google::protobuf::Message* message) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    return make_status(wire::StatusCode::NOT_FOUND,
                       fmt::format("Failed to open file {}", filename));
  }
  std::string buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  auto extension = file_extension(filename);
  if (extension == "json") {
    auto status = google::protobuf::util::JsonStringToMessage(buffer, message);
    return status.ok() ? make_status(wire::StatusCode::OK)
                       : make_status(wire::StatusCode::INTERNAL_ERROR, status.ToString());
  } else {
    return make_status(wire::StatusCode::FAILED_PRECONDITION,
                       fmt::format("Invalid file format '{}'", extension));
  }
}

wire::Status save(std::string const& filename, google::protobuf::Message const& message) {
  std::string data;
  auto extension = file_extension(filename);
  if (extension == "json") {
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    google::protobuf::util::MessageToJsonString(message, &data, options);
  } else {
    return make_status(wire::StatusCode::FAILED_PRECONDITION,
                       fmt::format("Invalid file format '{}'", extension));
  }

  std::ofstream file(filename);
  file << data;
  file.close();
  return make_status(wire::StatusCode::OK);
}

}  // namespace is