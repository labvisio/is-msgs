#include "io.hpp"
#include "boost/filesystem.hpp"
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/json_util.h"
#include "fmt/format.h"

namespace is {

namespace fs = boost::filesystem;
namespace gio = google::protobuf::io;

void load(std::string const& filename, google::protobuf::Message* message) {
  auto path = fs::path{filename};
  auto file = std::ifstream{filename};

  if (!file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open file {}", filename)};
  }

  std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  auto extension = path.extension().string();

  if (extension == ".json") {
    auto status = google::protobuf::util::JsonStringToMessage(buffer, message);
    if (!status.ok()) { throw std::runtime_error{status.ToString()}; }
    return;
  }

  if (extension == ".prototxt") {
    auto ok = google::protobuf::TextFormat::ParseFromString(buffer, message);
    if (!ok) { throw std::runtime_error{"Failed to parse prototxt"}; }
    return;
  }

  throw std::runtime_error{fmt::format("Invalid file format '{}'", extension)};
}

void save(std::string const& filename, google::protobuf::Message const& message) {
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
    throw std::runtime_error{fmt::format("Invalid file format '{}'", extension)};
  }

  auto parent_path = path.parent_path();
  boost::system::error_code error;
  if (!parent_path.string().empty()) { fs::create_directories(path.parent_path(), error); }
  if (error) {
    throw std::runtime_error{fmt::format("Failed to create directories '{}': {}",
                                         parent_path.string(), error.message())};
  }

  auto file = std::ofstream{filename};
  if (!file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open file {}", filename)};
  }

  file << data;
  file.close();
}

ProtobufWriter::ProtobufWriter(std::string const& filename) : raw_output(nullptr) {
  file.open(filename, std::ios::out | std::ios::binary);
  if (file.fail()) {
    throw std::runtime_error{fmt::format("Failed to open file \'{}\'", filename)};
  }
  raw_output = new gio::OstreamOutputStream(&file);
}

ProtobufWriter::~ProtobufWriter() {
  close();
}

void ProtobufWriter::insert(google::protobuf::Message const& message) {
  auto size = message.ByteSizeLong();
  // We create a new coded stream for each message.  Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream coded_output(raw_output);
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
      throw std::runtime_error{"Can not write serialized message on buffer."};
    }
  }
}

void ProtobufWriter::close() {
  if (raw_output != nullptr) {
    delete raw_output;
    raw_output = nullptr;
  }
  if (file.is_open()) { file.close(); }
}

ProtobufReader::ProtobufReader(std::string const& filename) : raw_input(nullptr) {
  file.open(filename, std::ios::in | std::ios::binary);
  if (file.fail()) {
    throw std::runtime_error{fmt::format("Failed to open file \'{}\'", filename)};
  }
  raw_input = new gio::IstreamInputStream(&file);
}

ProtobufReader::~ProtobufReader() {
  close();
}

bool ProtobufReader::next(google::protobuf::Message* message) {
  // We create a new coded stream for each message.  Don't worry, this is fast,
  // and it makes sure the 64MB total size limit is imposed per-message rather
  // than on the whole stream.  (See the CodedInputStream interface for more
  // info on this limit.)
  google::protobuf::io::CodedInputStream coded_input(raw_input);

  uint32_t size;
  if (!coded_input.ReadVarint32(&size)) { return false; }

  // Tell the stream not to read beyond that size.
  google::protobuf::io::CodedInputStream::Limit msg_limit = coded_input.PushLimit(size);
  if (!message->MergeFromCodedStream(&coded_input)) {
    throw std::runtime_error("Can't deserialize message");
  } else if (!coded_input.ConsumedEntireMessage()) {
    throw std::runtime_error("Failed to consume message");
  }

  // Release the limit
  coded_input.PopLimit(msg_limit);
  return true;
}

void ProtobufReader::close() {
  if (raw_input != nullptr) {
    delete raw_input;
    raw_input = nullptr;
  }
  if (file.is_open()) { file.close(); }
}

}  // namespace is