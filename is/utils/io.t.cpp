#include <cstdio>
#include "gtest/gtest.h"
#include "io.hpp"
#include "is/msgs/common.pb.h"
#include "ostream.hpp"
#include "google/protobuf/util/message_differencer.h"

namespace {

TEST(IOTests, LoadAndSave) {
  is::common::Tensor tensor;
  tensor.add_doubles(1.2);
  tensor.add_doubles(2.3);
  tensor.add_doubles(3.4);

  std::remove("hodor");
  ASSERT_EQ(is::load("hodor", &tensor).code(), is::wire::StatusCode::NOT_FOUND);

  std::ofstream os("hodor");
  os.close();

  ASSERT_EQ(is::load("hodor", &tensor).code(), is::wire::StatusCode::FAILED_PRECONDITION);

  ASSERT_EQ(is::save("hodor", tensor).code(), is::wire::StatusCode::FAILED_PRECONDITION);

  std::remove("hodor.json");
  ASSERT_EQ(is::save("hodor.json", tensor).code(), is::wire::StatusCode::OK);

  is::common::Tensor tensor2;
  ASSERT_EQ(is::load("hodor.json", &tensor2).code(), is::wire::StatusCode::OK);

  ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor, tensor2));

  is::common::SyncRequest sr;
  ASSERT_EQ(is::load("hodor.json", &sr).code(), is::wire::StatusCode::INTERNAL_ERROR);
}

}  // namespace