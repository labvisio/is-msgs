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

  // ProtobufWriter and ProtobufReader
  {
    is::common::Tensor tensor1;
    tensor.add_doubles(1.2);
    tensor.add_doubles(2.3);
    tensor.add_doubles(3.4);
    
    is::common::Tensor tensor2;
    tensor.add_doubles(-1.2);
    tensor.add_doubles(-2.3);
    tensor.add_doubles(-3.4);

    // write messages on file
    is::ProtobufWriter writer("tensors");
    writer.insert(tensor1);
    writer.insert(tensor2);
    writer.close();

    // read messages from file and assert
    is::ProtobufReader reader("tensors");
    auto maybe_tensor1 = reader.next<is::common::Tensor>();
    ASSERT_TRUE(maybe_tensor1);
    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor1, *maybe_tensor1));
    
    auto maybe_tensor2 = reader.next<is::common::Tensor>();
    ASSERT_TRUE(maybe_tensor2);
    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(tensor2, *maybe_tensor2));
    
    // test end of file, must not return a message
    auto maybe_tensor3 = reader.next<is::common::Tensor>();
    ASSERT_TRUE(!maybe_tensor3);

    boost::filesystem::remove_all("tensors");
  }
}

}  // namespace