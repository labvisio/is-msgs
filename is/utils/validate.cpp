#include "validate.hpp"
#include "google/protobuf/reflection.h"
#include "fmt/format.h"

namespace is {

template <typename NumericLimits, typename T>
void validate_numeric_limits(std::string const& name, NumericLimits const& limits, T value) {
  if (limits.has_gt() && !(value > limits.gt())) {
    throw std::runtime_error{
        fmt::format("Field {} should be > than {}. Given value: {}", name, limits.gt(), value)};
  }
  if (limits.has_gte() && !(value >= limits.gte())) {
    throw std::runtime_error{
        fmt::format("Field {} should be >= than {}. Given value: {}", name, limits.gte(), value)};
  }
  if (limits.has_lt() && !(value < limits.lt())) {
    throw std::runtime_error{
        fmt::format("Field {} should be < than {}. Given value: {}", name, limits.lt(), value)};
  }
  if (limits.has_lte() && !(value <= limits.lte())) {
    throw std::runtime_error{
        fmt::format("Field {} should be <= than {}. Given value: {}", name, limits.lte(), value)};
  }
}

template <typename Limits, typename T>
void validate_numeric_limits(std::string const& name, Limits const& limits,
                             google::protobuf::RepeatedFieldRef<T> const& repeated) {
  auto first = repeated.begin();
  auto last = repeated.end();
  while (first != last) {
    validate_numeric_limits(name, limits, *first++);
  }
}

inline bool message_validation_is_required(google::protobuf::FieldOptions const& options) {
  if (options.HasExtension(validate::rules)) {
    auto ext = options.GetExtension(validate::rules);
    return ext.has_message() && ext.message().required();
  }
  return false;
}

void validate_field(google::protobuf::Message const& msg,
                    google::protobuf::FieldDescriptor const* field) {
  namespace pb = google::protobuf;
  using FieldType = google::protobuf::FieldDescriptor::Type;

  auto refl = msg.GetReflection();
  auto opts = field->options();
  auto type = field->type();

  if (type == FieldType::TYPE_MESSAGE) {
    if (field->is_repeated()) {
      for (int n = 0; n < refl->FieldSize(msg, field); ++n) {
        validate_message(refl->GetRepeatedMessage(msg, field, n));
      }
    } else if (refl->HasField(msg, field) || message_validation_is_required(opts)) {
      validate_message(refl->GetMessage(msg, field));
    }
  } else if (opts.HasExtension(validate::rules)) {
    auto ext = opts.GetExtension(validate::rules);
    auto name = field->full_name();

    // float validation
    if (type == FieldType::TYPE_FLOAT && ext.has_float_()) {
      if (field->is_repeated()) {
        validate_numeric_limits(name, ext.float_(), refl->GetRepeatedFieldRef<float>(msg, field));
      } else {
        validate_numeric_limits(name, ext.float_(), refl->GetFloat(msg, field));
      }
    }
    // double validation
    else if (type == FieldType::TYPE_DOUBLE && ext.has_double_()) {
      if (field->is_repeated()) {
        validate_numeric_limits(name, ext.double_(), refl->GetRepeatedFieldRef<double>(msg, field));
      } else {
        validate_numeric_limits(name, ext.double_(), refl->GetDouble(msg, field));
      }
    }
    // int32 validation
    else if (type == FieldType::TYPE_INT32 && ext.has_int32()) {
      if (field->is_repeated()) {
        validate_numeric_limits(name, ext.int32(),
                                refl->GetRepeatedFieldRef<pb::int32>(msg, field));
      } else {
        validate_numeric_limits(name, ext.int32(), refl->GetInt32(msg, field));
      }
    }
    // uint32 validation
    else if (type == FieldType::TYPE_UINT32 && ext.has_uint32()) {
      if (field->is_repeated()) {
        validate_numeric_limits(name, ext.uint32(),
                                refl->GetRepeatedFieldRef<pb::uint32>(msg, field));

      } else {
        validate_numeric_limits(name, ext.uint32(), refl->GetUInt32(msg, field));
      }
    }
    // int64 validation
    else if (type == FieldType::TYPE_INT64 && ext.has_int64()) {
      if (field->is_repeated()) {
        validate_numeric_limits(name, ext.int64(),
                                refl->GetRepeatedFieldRef<pb::int64>(msg, field));

      } else {
        validate_numeric_limits(name, ext.int64(), refl->GetInt64(msg, field));
      }
    }
    // uint64 validation
    else if (type == FieldType::TYPE_UINT64 && ext.has_uint64()) {
      if (field->is_repeated()) {
        validate_numeric_limits(name, ext.uint64(),
                                refl->GetRepeatedFieldRef<pb::uint64>(msg, field));

      } else {
        validate_numeric_limits(name, ext.uint64(), refl->GetUInt64(msg, field));
      }
    }
  }
}

void validate_message(google::protobuf::Message const& message) {
  auto descriptor = message.GetDescriptor();
  for (int i = 0; i < descriptor->field_count(); ++i) {
    validate_field(message, descriptor->field(i));
  }
}

}  // namespace is