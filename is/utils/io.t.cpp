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
    // back-to-back test
    {
      // write messages on file
      is::ProtobufWriter writer("tensors");
      for (auto n = 0; n < 1000; n++) {
        is::common::Tensor tensor;
        tensor.add_doubles(static_cast<float>(1 * n));
        tensor.add_doubles(static_cast<float>(2 * n));
        tensor.add_doubles(static_cast<float>(3 * n));
        ASSERT_EQ(writer.insert(tensor).code(), is::wire::StatusCode::OK);
      }
      writer.close();

      // read messages from file and assert
      is::ProtobufReader reader("tensors");
      for (auto n = 0; n < 1001; n++) {
        is::common::Tensor tensor;
        auto status = reader.next(&tensor);
        if (n < 1000) {
          ASSERT_EQ(status.code(), is::wire::StatusCode::OK);
          is::common::Tensor base_tensor;
          base_tensor.add_doubles(static_cast<float>(1 * n));
          base_tensor.add_doubles(static_cast<float>(2 * n));
          base_tensor.add_doubles(static_cast<float>(3 * n));
          ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(base_tensor, tensor));
        }
        else {
          ASSERT_NE(status.code(), is::wire::StatusCode::OK);
        }
      }

      boost::filesystem::remove_all("tensors");
    }
    
    // reading empty file test
    {
      is::ProtobufWriter writer("tensors");
      writer.close();
      is::ProtobufReader reader("tensors");
      is::common::Tensor tensor;
      auto status = reader.next(&tensor);
      ASSERT_EQ(status.code(), is::wire::StatusCode::OUT_OF_RANGE);
      boost::filesystem::remove_all("tensors");
    }
    
    // constructors test
    {
      // try to write on a invalid file
      try {
        is::ProtobufWriter writer("/");
      } catch (is::wire::Status const& e) {
        ASSERT_EQ(e.code(), is::wire::StatusCode::FAILED_PRECONDITION);
      }
      
      // try to read from an invalid file
      try {
        is::ProtobufReader reader("tensors");
      } catch (is::wire::Status const& e) {
        ASSERT_EQ(e.code(), is::wire::StatusCode::FAILED_PRECONDITION);
      }
    }
  }
}

}  // namespace