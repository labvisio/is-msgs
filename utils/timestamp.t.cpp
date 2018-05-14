#include <chrono>
#include "utils.hpp"

int main() {
  using namespace std::chrono;

  auto now = std::chrono::system_clock::now();
  if(is::to_system_clock(is::to_timestamp(now)) != now) return -1;

  auto duration = now - std::chrono::system_clock::now() - std::chrono::seconds(60);
  if(is::to_nanoseconds(is::to_duration(duration)) != duration_cast<nanoseconds>(duration)) return -1;
}