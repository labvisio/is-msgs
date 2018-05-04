#include "utils.hpp"

namespace is {

pb::Timestamp current_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  pb::Timestamp timestamp;
  timestamp.set_seconds(tv.tv_sec);
  timestamp.set_nanos(tv.tv_usec * 1000);
  return timestamp;
}

wire::Status make_status(wire::StatusCode code, std::string const& why) {
  wire::Status status;
  status.set_code(code);
  status.set_why(why);
  return status;
}

}  // namespace is