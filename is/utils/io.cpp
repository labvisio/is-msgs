#include "io.hpp"
#include "boost/filesystem.hpp"
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/json_util.h"
#include "spdlog/fmt/fmt.h"

namespace is {

namespace fs = boost::filesystem;
namespace gio = google::protobuf::io;

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

ProtobufWriter::ProtobufWriter(std::string const& filename) try
    : file(filename, std::ios::out | std::ios::binary) {
  this->raw_output = new gio::OstreamOutputStream(&file);
} catch (std::exception const& e) {
  auto why = fmt::format("Can't open file \'{}\'\n{}", filename, e.what());
  throw make_status(wire::StatusCode::FAILED_PRECONDITION, why);
}

ProtobufWriter::~ProtobufWriter() {
  this->close();
}

wire::Status ProtobufWriter::insert(google::protobuf::Message const& message) {
  auto size = message.ByteSize();
  // We create a new coded stream for each message.  Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream coded_output(this->raw_output);
  coded_output.WriteVarint32(size);
  uint8_t* buffer = coded_output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != nullptr) {
    // Optimization:  The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&coded_output);
    if (coded_output.HadError()) {
      auto why = "Can not write serialized message on buffer.";
      return make_status(wire::StatusCode::INTERNAL_ERROR, why);
    }
  }
  return make_status(wire::StatusCode::OK);
}

void ProtobufWriter::close() {
  if (file.is_open()) {
    delete raw_output;
    file.close();
  }
}

ProtobufReader::ProtobufReader(std::string const& filename) try
    : file(filename, std::ios::in | std::ios::binary) {
  raw_input = new gio::IstreamInputStream(&file);
} catch (std::exception const& e) {
  auto why = fmt::format("Can't open file \'{}\'\n{}", filename, e.what());
  throw make_status(wire::StatusCode::FAILED_PRECONDITION, why);
}

ProtobufReader::~ProtobufReader() {
  this->close();
}

wire::Status ProtobufReader::next(google::protobuf::Message* message) {
  // We create a new coded stream for each message.  Don't worry, this is fast,
  // and it makes sure the 64MB total size limit is imposed per-message rather
  // than on the whole stream.  (See the CodedInputStream interface for more
  // info on this limit.)
  google::protobuf::io::CodedInputStream coded_input(this->raw_input);

  uint32_t size;
  if (!coded_input.ReadVarint32(&size)) {
    this->close();
    return make_status(wire::StatusCode::OUT_OF_RANGE, "End of file");
  }
  // Tell the stream not to read beyond that size.
  google::protobuf::io::CodedInputStream::Limit msg_limit = coded_input.PushLimit(size);
  if (!message->MergeFromCodedStream(&coded_input)) {
    return make_status(wire::StatusCode::INTERNAL_ERROR, "Can't deserialize message");
  } else if (!coded_input.ConsumedEntireMessage()) {
    return make_status(wire::StatusCode::INTERNAL_ERROR, "");
  }
  // Release the limit
  coded_input.PopLimit(msg_limit);
  return make_status(wire::StatusCode::OK);
}

void ProtobufReader::close() {
  if (file.is_open()) {
    delete raw_input;
    file.close();
  }
}

}  // namespace is