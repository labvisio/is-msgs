#include "io.hpp"
#include <fstream>
#include "boost/filesystem.hpp"
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/json_util.h"
#include "spdlog/fmt/fmt.h"

namespace is {

namespace fs = boost::filesystem;

wire::Status load(std::string const& filename, google::protobuf::Message* message) {
  auto path = fs::path{filename};
  auto file = std::ifstream{filename};
  if (!file.is_open()) {
    return make_status(wire::StatusCode::NOT_FOUND,
                       fmt::format("Failed to open file {}", filename));
  }
  std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  auto extension = path.extension().string();
  if (extension == ".json") {
    auto status = google::protobuf::util::JsonStringToMessage(buffer, message);
    return status.ok() ? make_status(wire::StatusCode::OK)
                       : make_status(wire::StatusCode::INTERNAL_ERROR, status.ToString());
  } else if (extension == ".prototxt") {
    auto ok = google::protobuf::TextFormat::ParseFromString(buffer, message);
    return ok ? make_status(wire::StatusCode::OK)
              : make_status(wire::StatusCode::INTERNAL_ERROR, "Failed to parse prototxt");
  } else {
    return make_status(wire::StatusCode::FAILED_PRECONDITION,
                       fmt::format("Invalid file format '{}'", extension));
  }
}

wire::Status save(std::string const& filename, google::protobuf::Message const& message) {
  std::string data;
  auto path = fs::path{filename};
  auto extension = path.extension().string();
  if (extension == ".json") {
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    google::protobuf::util::MessageToJsonString(message, &data, options);
  } else if (extension == ".prototxt") {
    google::protobuf::TextFormat::PrintToString(message, &data);
  } else {
    return make_status(wire::StatusCode::FAILED_PRECONDITION,
                       fmt::format("Invalid file format '{}'", extension));
  }

  auto parent_path = path.parent_path();
  boost::system::error_code error;
  if (!parent_path.string().empty()) { fs::create_directories(path.parent_path(), error); }
  if (error) {
    return make_status(wire::StatusCode::INTERNAL_ERROR,
                       fmt::format("Failed to create directories '{}': {}", parent_path.string(),
                                   error.message()));
  }

  auto file = std::ofstream{filename};
  if (!file.is_open()) {
    return make_status(wire::StatusCode::NOT_FOUND,
                       fmt::format("Failed to open file {}", filename));
  }
  file << data;
  file.close();
  return make_status(wire::StatusCode::OK);
}

}  // namespace is