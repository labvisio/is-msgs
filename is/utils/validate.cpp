#include "validate.hpp"
#include "google/protobuf/reflection.h"
#include "spdlog/fmt/fmt.h"

#define is_assert_ok(failable)                                    \
  do {                                                            \
    auto status = failable;                                       \
    if (status.code() != is::wire::StatusCode::OK) return status; \
  } while (0)

namespace is {

wire::Status make_error(std::string&& message) {
  return is::make_status(is::wire::StatusCode::FAILED_PRECONDITION, message);
}

template <typename NumericLimits, typename T>
wire::Status validate_numeric_limits(std::string const& name, NumericLimits const& limits,
                                     T value) {
  if (limits.has_gt() && !(value > limits.gt())) {
    return make_error(
        fmt::format("Field {} should be > than {}. Given value: {}", name, limits.gt(), value));
  }
  if (limits.has_gte() && !(value >= limits.gte())) {
    return make_error(
        fmt::format("Field {} should be >= than {}. Given value: {}", name, limits.gte(), value));
  }
  if (limits.has_lt() && !(value < limits.lt())) {
    return make_error(
        fmt::format("Field {} should be < than {}. Given value: {}", name, limits.lt(), value));
  }
  if (limits.has_lte() && !(value <= limits.lte())) {
    return make_error(
        fmt::format("Field {} should be <= than {}. Given value: {}", name, limits.lte(), value));
  }
  return is::make_status(is::wire::StatusCode::OK);
}

template <typename Limits, typename T>
wire::Status validate_numeric_limits(std::string const& name, Limits const& limits,
                                     google::protobuf::RepeatedFieldRef<T> const& repeated) {
  auto first = repeated.begin();
  auto last = repeated.end();
  while (first != last) {
    is_assert_ok(validate_numeric_limits(name, limits, *first++));
  }
  return is::make_status(is::wire::StatusCode::OK);
}

inline bool message_validation_is_required(google::protobuf::FieldOptions const& options) {
  if (options.HasExtension(validate::rules)) {
    auto ext = options.GetExtension(validate::rules);
    return ext.has_message() && ext.message().required();
  }
  return false;
}

wire::Status validate_field(google::protobuf::Message const& msg,
                            google::protobuf::FieldDescriptor const* field) {
  namespace pb = google::protobuf;
  using FieldType = google::protobuf::FieldDescriptor::Type;

  auto refl = msg.GetReflection();
  auto opts = field->options();
  auto type = field->type();

  if (type == FieldType::TYPE_MESSAGE) {
    if (field->is_repeated()) {
      for (int n = 0; n < refl->FieldSize(msg, field); ++n) {
        is_assert_ok(validate_message(refl->GetRepeatedMessage(msg, field, n)));
      }
    } else if (refl->HasField(msg, field) || message_validation_is_required(opts)) {
      is_assert_ok(validate_message(refl->GetMessage(msg, field)));
    }
  } else if (opts.HasExtension(validate::rules)) {
    auto ext = opts.GetExtension(validate::rules);
    auto name = field->full_name();
    if (type == FieldType::TYPE_FLOAT && ext.has_float_()) {
      if (field->is_repeated()) {
        is_assert_ok(validate_numeric_limits(name, ext.float_(),
                                             refl->GetRepeatedFieldRef<float>(msg, field)));
      } else {
        is_assert_ok(validate_numeric_limits(name, ext.float_(), refl->GetFloat(msg, field)));
      }
    } else if (type == FieldType::TYPE_DOUBLE && ext.has_double_()) {
      if (field->is_repeated()) {
        is_assert_ok(validate_numeric_limits(name, ext.double_(),
                                             refl->GetRepeatedFieldRef<double>(msg, field)));
      } else {
        is_assert_ok(validate_numeric_limits(name, ext.double_(), refl->GetDouble(msg, field)));
      }
    } else if (type == FieldType::TYPE_INT32 && ext.has_int32()) {
      if (field->is_repeated()) {
        is_assert_ok(validate_numeric_limits(name, ext.int32(),
                                             refl->GetRepeatedFieldRef<pb::int32>(msg, field)));
      } else {
        is_assert_ok(validate_numeric_limits(name, ext.int32(), refl->GetInt32(msg, field)));
      }
    } else if (type == FieldType::TYPE_UINT32 && ext.has_uint32()) {
      if (field->is_repeated()) {
        is_assert_ok(validate_numeric_limits(name, ext.uint32(),
                                             refl->GetRepeatedFieldRef<pb::uint32>(msg, field)));

      } else {
        is_assert_ok(validate_numeric_limits(name, ext.uint32(), refl->GetUInt32(msg, field)));
      }
    } else if (type == FieldType::TYPE_INT64 && ext.has_int64()) {
      if (field->is_repeated()) {
        is_assert_ok(validate_numeric_limits(name, ext.int64(),
                                             refl->GetRepeatedFieldRef<pb::int64>(msg, field)));

      } else {
        is_assert_ok(validate_numeric_limits(name, ext.int64(), refl->GetInt64(msg, field)));
      }
    } else if (type == FieldType::TYPE_UINT64 && ext.has_uint64()) {
      if (field->is_repeated()) {
        is_assert_ok(validate_numeric_limits(name, ext.uint64(),
                                             refl->GetRepeatedFieldRef<pb::uint64>(msg, field)));

      } else {
        is_assert_ok(validate_numeric_limits(name, ext.uint64(), refl->GetUInt64(msg, field)));
      }
    }
  }
  return is::make_status(is::wire::StatusCode::OK);
}

wire::Status validate_message(google::protobuf::Message const& message) {
  auto descriptor = message.GetDescriptor();
  for (int i = 0; i < descriptor->field_count(); ++i) {
    is_assert_ok(validate_field(message, descriptor->field(i)));
  }
  return is::make_status(is::wire::StatusCode::OK);
}

}  // namespace is