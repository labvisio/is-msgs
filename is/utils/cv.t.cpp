#include "cv.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CVTests, Conversions) {
  {
    cv::Mat mat(2, 3, CV_8U, cv::Scalar::all(std::numeric_limits<uint8_t>::max()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    mat2.convertTo(mat2, CV_8U);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
  {
    cv::Mat mat(2, 3, CV_8S, cv::Scalar::all(std::numeric_limits<int8_t>::min()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    mat2.convertTo(mat2, CV_8S);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
  {
    cv::Mat mat(2, 3, CV_16U, cv::Scalar::all(std::numeric_limits<uint16_t>::min()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    mat2.convertTo(mat2, CV_16U);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
  {
    cv::Mat mat(2, 3, CV_16S, cv::Scalar::all(std::numeric_limits<int16_t>::min()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    mat2.convertTo(mat2, CV_16S);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
  {
    cv::Mat mat(2, 3, CV_32S, cv::Scalar::all(std::numeric_limits<int32_t>::min()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
  {
    cv::Mat mat(2, 3, CV_32F, cv::Scalar::all(std::numeric_limits<float>::min()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
  {
    cv::Mat mat(2, 3, CV_64F, cv::Scalar::all(std::numeric_limits<double>::min()));
    auto tensor = is::to_tensor(mat);
    auto mat2 = is::to_mat(tensor);
    ASSERT_TRUE(cv::countNonZero(mat != mat2) == 0);
  }
}

}  // namespace