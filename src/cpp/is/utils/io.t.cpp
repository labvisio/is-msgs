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
  EXPECT_THROW(is::load("hodor", &tensor), std::runtime_error);

  // Invalid Extensions
  std::ofstream os("hodor");
  os.close();
  EXPECT_THROW(is::load("hodor", &tensor), std::runtime_error);
  EXPECT_THROW(is::save("hodor", tensor), std::runtime_error);

  // Json back to back
  {
    is::common::Tensor other_tensor;
    is::common::SyncRequest sr;
    boost::filesystem::remove_all("hodor.json");

    EXPECT_NO_THROW(is::save("hodor.json", tensor));
    EXPECT_NO_THROW(is::load("hodor.json", &other_tensor));
    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor, other_tensor));
    EXPECT_THROW(is::load("hodor.json", &sr), std::runtime_error);
  }

  // Prototxt back to back
  {
    is::common::Tensor other_tensor;
    is::common::SyncRequest sr;
    boost::filesystem::remove_all("hodor.prototxt");

    EXPECT_NO_THROW(is::save("hodor.prototxt", tensor));
    EXPECT_NO_THROW(is::load("hodor.prototxt", &other_tensor));
    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor, other_tensor));
    EXPECT_THROW(is::load("hodor.prototxt", &sr), std::runtime_error);
  }

  // Multidirectory creation
  {
    // should fail, hodor is also a file
    EXPECT_THROW(is::save("hodor/hodor/hodor/hodor.prototxt", tensor), std::runtime_error);

    boost::filesystem::remove_all("hodor");
    EXPECT_NO_THROW(is::save("hodor/hodor/hodor/hodor.prototxt", tensor));
  }
}

TEST(IOTests, ProtobufWriterReader) {
  // back-to-back test
  {
    boost::filesystem::remove_all("tensors");

    // write messages on file
    is::ProtobufWriter writer("tensors");
    for (auto n = 0; n < 1000; n++) {
      is::common::Tensor tensor;
      tensor.add_doubles(1 * n);
      tensor.add_doubles(2 * n);
      tensor.add_doubles(3 * n);
      ASSERT_NO_THROW(writer.insert(tensor));
    }
    writer.close();

    // read messages from file and assert
    is::ProtobufReader reader("tensors");
    for (auto n = 0; n < 1000; n++) {
      is::common::Tensor tensor;
      ASSERT_TRUE(reader.next(&tensor));
      is::common::Tensor base_tensor;
      base_tensor.add_doubles(1 * n);
      base_tensor.add_doubles(2 * n);
      base_tensor.add_doubles(3 * n);
      ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(base_tensor, tensor));
    }

    is::common::Tensor tensor;
    ASSERT_FALSE(reader.next(&tensor));
    boost::filesystem::remove_all("tensors");
  }

  // reading empty file test
  {
    is::ProtobufWriter writer("tensors");
    writer.close();
    is::ProtobufReader reader("tensors");
    is::common::Tensor tensor;
    ASSERT_FALSE(reader.next(&tensor));
    boost::filesystem::remove_all("tensors");
  }

  // constructors test
  {
    // try to write on a invalid file
    EXPECT_THROW(is::ProtobufWriter{"/this/path/does/not/"}, std::runtime_error);

    // try to read from an invalid file
    EXPECT_THROW(is::ProtobufReader{"tensors"}, std::runtime_error);
  }
}

}  // namespace