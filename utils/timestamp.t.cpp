
#include <cassert>
#include <chrono>
#include "utils.hpp"

int main() {
  auto now = std::chrono::system_clock::now();
  assert(is::to_system_clock(is::to_timestamp(now)) == now);

  auto duration = now - std::chrono::system_clock::now() - std::chrono::seconds(60);
  assert(is::to_nanoseconds(is::to_duration(duration)) == duration_cast<nanoseconds>(duration));
}