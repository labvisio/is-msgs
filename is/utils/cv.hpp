#pragma once

#include "is/msgs/common.pb.h"
#include "opencv2/core.hpp"

namespace is {

cv::Mat to_mat_view(common::Tensor& tensor) {
  auto shape = tensor.shape();
  if (shape.dims_size() != 2 || shape.dims(0).name() != "rows") { return cv::Mat(); }
  auto rows = shape.dims(0).size();
  auto cols = shape.dims(1).size();

  if (tensor.type() == is::common::DataType::FLOAT_TYPE)
    return cv::Mat(rows, cols, CV_32F, tensor.mutable_floats()->mutable_data());
  if (tensor.type() == is::common::DataType::DOUBLE_TYPE)
    return cv::Mat(rows, cols, CV_64F, tensor.mutable_doubles()->mutable_data());
  if (tensor.type() == is::common::DataType::INT32_TYPE)
    return cv::Mat(rows, cols, CV_32S, tensor.mutable_ints32()->mutable_data());
  return cv::Mat();
}

cv::Mat to_mat(common::Tensor& tensor) {
  return to_mat_view(tensor).clone();
}

common::Tensor to_tensor(cv::Mat& mat) {
  is::common::Tensor tensor;

  if (mat.rows == 0 || mat.cols == 0) { return tensor; }

  // cv is row major
  auto rows = tensor.mutable_shape()->add_dims();
  rows->set_size(mat.rows);
  rows->set_name("rows");

  auto cols = tensor.mutable_shape()->add_dims();
  cols->set_size(mat.cols);
  cols->set_name("cols");

  auto type = mat.depth();
  if (type == CV_8U || type == CV_8S || type == CV_16U || type == CV_16S || type == CV_32S) {
    tensor.set_type(is::common::DataType::INT32_TYPE);
    auto ints32 = tensor.mutable_ints32();
    ints32->Reserve(mat.rows * mat.cols);
    auto back_inserter = google::protobuf::RepeatedFieldBackInserter(ints32);
    if (type == CV_8U) {
      std::copy(mat.begin<uint8_t>(), mat.end<uint8_t>(), back_inserter);
    } else if (type == CV_8S) {
      std::copy(mat.begin<int8_t>(), mat.end<int8_t>(), back_inserter);
    } else if (type == CV_16U) {
      std::copy(mat.begin<uint16_t>(), mat.end<uint16_t>(), back_inserter);
    } else if (type == CV_16S) {
      std::copy(mat.begin<int16_t>(), mat.end<int16_t>(), back_inserter);
    } else if (type == CV_32S) {
      std::copy(mat.begin<int32_t>(), mat.end<int32_t>(), back_inserter);
    }
  } else if (type == CV_32F) {
    tensor.set_type(is::common::DataType::FLOAT_TYPE);
    auto floats = tensor.mutable_floats();
    floats->Reserve(mat.rows * mat.cols);
    std::copy(mat.begin<float>(), mat.end<float>(),
              google::protobuf::RepeatedFieldBackInserter(floats));
  } else if (type == CV_64F) {
    tensor.set_type(is::common::DataType::DOUBLE_TYPE);
    auto doubles = tensor.mutable_doubles();
    doubles->Reserve(mat.rows * mat.cols);
    std::copy(mat.begin<double>(), mat.end<double>(),
              google::protobuf::RepeatedFieldBackInserter(doubles));
  }

  return tensor;
}

}  // namespace is