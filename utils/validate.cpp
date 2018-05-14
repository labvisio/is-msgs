#include "validate.hpp"
#include "spdlog/fmt/fmt.h"

namespace is {

template <typename NumericLimits, typename T>
is::wire::Status validate_numeric_limits(std::string const& name, NumericLimits const& limits,
                                         T value) {
  if (limits.has_gt() && !(value > limits.gt())) {
    return is::make_status(
        is::wire::StatusCode::FAILED_PRECONDITION,
        fmt::format("Field {} should be > than {}. Given value: {}", name, limits.gt(), value));
  }
  if (limits.has_gte() && !(value >= limits.gte())) {
    return is::make_status(
        is::wire::StatusCode::FAILED_PRECONDITION,
        fmt::format("Field {} should be >= than {}. Given value: {}", name, limits.gte(), value));
  }
  if (limits.has_lt() && !(value < limits.lt())) {
    return is::make_status(
        is::wire::StatusCode::FAILED_PRECONDITION,
        fmt::format("Field {} should be < than {}. Given value: {}", name, limits.lt(), value));
  }
  if (limits.has_lte() && !(value <= limits.lte())) {
    return is::make_status(
        is::wire::StatusCode::FAILED_PRECONDITION,
        fmt::format("Field {} should be <= than {}. Given value: {}", name, limits.lte(), value));
  }
  return is::make_status(is::wire::StatusCode::OK);
}

is::wire::Status validate_field(google::protobuf::Message const& message,
                                google::protobuf::FieldDescriptor const* descriptor) {
  auto reflection = message.GetReflection();
  auto field_opts = descriptor->options();

  using FieldType = google::protobuf::FieldDescriptor::Type;
  auto type = descriptor->type();

  // message
  if (type == FieldType::TYPE_MESSAGE) {
    auto status = validate_message(reflection->GetMessage(message, descriptor));
    if (status.code() != is::wire::StatusCode::OK) return status;
  } else if (field_opts.HasExtension(validate::rules)) {
    auto ext = field_opts.GetExtension(validate::rules);
    // float
    if (type == FieldType::TYPE_FLOAT && ext.has_float_()) {
      auto status = validate_numeric_limits(descriptor->full_name(), ext.float_(),
                                            reflection->GetFloat(message, descriptor));
      if (status.code() != is::wire::StatusCode::OK) return status;
    }
    // double
    else if (type == FieldType::TYPE_DOUBLE && ext.has_double_()) {
      auto status = validate_numeric_limits(descriptor->full_name(), ext.double_(),
                                            reflection->GetDouble(message, descriptor));
      if (status.code() != is::wire::StatusCode::OK) return status;
    }
    // int32
    else if (type == FieldType::TYPE_INT32 && ext.has_int32()) {
      auto status = validate_numeric_limits(descriptor->full_name(), ext.int32(),
                                            reflection->GetInt32(message, descriptor));
      if (status.code() != is::wire::StatusCode::OK) return status;
    }
    // uint32
    else if (type == FieldType::TYPE_UINT32 && ext.has_uint32()) {
      auto status = validate_numeric_limits(descriptor->full_name(), ext.uint32(),
                                            reflection->GetUInt32(message, descriptor));
      if (status.code() != is::wire::StatusCode::OK) return status;
    }
    // int64
    else if (type == FieldType::TYPE_INT64 && ext.has_int64()) {
      auto status = validate_numeric_limits(descriptor->full_name(), ext.int64(),
                                            reflection->GetInt64(message, descriptor));
      if (status.code() != is::wire::StatusCode::OK) return status;
    }
    // uint64
    else if (type == FieldType::TYPE_UINT64 && ext.has_uint64()) {
      auto status = validate_numeric_limits(descriptor->full_name(), ext.uint64(),
                                            reflection->GetUInt64(message, descriptor));
      if (status.code() != is::wire::StatusCode::OK) return status;
    }
  }
  return is::make_status(is::wire::StatusCode::OK);
}

is::wire::Status validate_message(google::protobuf::Message const& message) {
  auto descriptor = message.GetDescriptor();
  for (int i = 0; i < descriptor->field_count(); ++i) {
    auto status = validate_field(message, descriptor->field(i));
    if (status.code() != is::wire::StatusCode::OK) return status;
  }
  return is::make_status(is::wire::StatusCode::OK);
}

}  // namespace is