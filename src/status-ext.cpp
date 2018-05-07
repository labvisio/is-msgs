#include "status-ext.hpp"

namespace is {

wire::Status make_status(wire::StatusCode code, std::string const& why) {
  wire::Status status;
  status.set_code(code);
  status.set_why(why);
  return status;
}

}  // namespace is