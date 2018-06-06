#include <cstdio>
#include <fstream>
#include "boost/filesystem.hpp"
#include "google/protobuf/util/message_differencer.h"
#include "gtest/gtest.h"
#include "io.hpp"
#include "is/msgs/common.pb.h"

namespace {

TEST(IOTests, LoadAndSave) {
  is::common::Tensor tensor;
  tensor.add_doubles(1.2);
  tensor.add_doubles(2.3);
  tensor.add_doubles(3.4);

  boost::filesystem::remove_all("hodor");
  ASSERT_EQ(is::load("hodor", &tensor).code(), is::wire::StatusCode::NOT_FOUND);

  // Invalid Extensions
  std::ofstream os("hodor");
  os.close();
  ASSERT_EQ(is::load("hodor", &tensor).code(), is::wire::StatusCode::FAILED_PRECONDITION);

  ASSERT_EQ(is::save("hodor", tensor).code(), is::wire::StatusCode::FAILED_PRECONDITION);

  // Json back to back
  {
    is::common::Tensor other_tensor;
    is::common::SyncRequest sr;
    boost::filesystem::remove_all("hodor.json");
    ASSERT_EQ(is::save("hodor.json", tensor).code(), is::wire::StatusCode::OK);

    ASSERT_EQ(is::load("hodor.json", &other_tensor).code(), is::wire::StatusCode::OK);

    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor, other_tensor));

    ASSERT_EQ(is::load("hodor.json", &sr).code(), is::wire::StatusCode::INTERNAL_ERROR);
  }

  // Prototxt back to back
  {
    is::common::Tensor other_tensor;
    is::common::SyncRequest sr;
    boost::filesystem::remove_all("hodor.prototxt");

    ASSERT_EQ(is::save("hodor.prototxt", tensor).code(), is::wire::StatusCode::OK);

    ASSERT_EQ(is::load("hodor.prototxt", &other_tensor).code(), is::wire::StatusCode::OK);

    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor, other_tensor));

    ASSERT_EQ(is::load("hodor.prototxt", &sr).code(), is::wire::StatusCode::INTERNAL_ERROR);
  }

  // Multidirectory creation
  {
    // should fail, hodor is also a file
    ASSERT_EQ(is::save("hodor/hodor/hodor/hodor.prototxt", tensor).code(),
              is::wire::StatusCode::INTERNAL_ERROR);

    boost::filesystem::remove_all("hodor");
    ASSERT_EQ(is::save("hodor/hodor/hodor/hodor.prototxt", tensor).code(),
              is::wire::StatusCode::OK);
  }
}

}  // namespace