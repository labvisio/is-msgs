#pragma once

#include <string>
#include <fstream>
#include "google/protobuf/message.h"
#include "status.hpp"
#include "boost/optional.hpp"

namespace google {
namespace protobuf {
namespace io {
class ZeroCopyOutputStream;
class CodedOutputStream;
class CodedInputStream;
}
}
}

namespace is {
wire::Status load(std::string const& filename, google::protobuf::Message* message);
wire::Status save(std::string const& filename, google::protobuf::Message const& message);

struct ProtobufWriter {
  std::ofstream file;
  google::protobuf::io::ZeroCopyOutputStream *raw_output;
  google::protobuf::io::CodedOutputStream *coded_output;

  ProtobufWriter(std::string const &filename);
  ~ProtobufWriter();
  void close();

  template <typename T> bool insert(T const &message) {
    coded_output->WriteVarint32(message.ByteSize());
    return message.SerializeToCodedStream(coded_output);
  }
};

struct ProtobufReader {
  std::ifstream file;
  google::protobuf::io::ZeroCopyInputStream *raw_input;
  google::protobuf::io::CodedInputStream *coded_input;

  ProtobufReader(std::string const &filename);
  ~ProtobufReader();
  void close();

  template <typename T> boost::optional<T> next() {
    boost::optional<T> optional;
    uint32_t size;
    if (!coded_input->ReadVarint32(&size)) {
      this->close();
      return optional;
    }
    google::protobuf::io::CodedInputStream::Limit msg_limit = coded_input->PushLimit(size);
    T msg;
    if (msg.ParseFromCodedStream(coded_input)) {
      coded_input->PopLimit(msg_limit);
      optional = msg;
    }
    return optional;
  }
};

}  // namespace is