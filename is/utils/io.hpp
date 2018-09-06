#pragma once

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/message.h"

namespace is {
void load(std::string const& filename, google::protobuf::Message* message);
void save(std::string const& filename, google::protobuf::Message const& message);

/*
  Implementation of 'ProtobufWriter' and 'ProtobufReader' based on
  https://stackoverflow.com/questions/2340730/are-there-c-equivalents-for-the-protocol-buffers-delimited-i-o-functions-in-ja
*/

struct ProtobufWriter {
  std::ofstream file;
  google::protobuf::io::ZeroCopyOutputStream* raw_output;

  ProtobufWriter(std::string const& filename);
  ~ProtobufWriter();
  void insert(google::protobuf::Message const& message);
  void close();
};

struct ProtobufReader {
  std::ifstream file;
  google::protobuf::io::ZeroCopyInputStream* raw_input;

  ProtobufReader(std::string const& filename);
  ~ProtobufReader();
  bool next(google::protobuf::Message* message);
  void close();
};

}  // namespace is