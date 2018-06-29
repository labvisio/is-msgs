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

TEST(CVTests, ViewIsShared) {
  cv::Mat mat(2, 3, CV_32S, cv::Scalar::all(10));
  auto tensor = is::to_tensor(mat);
  tensor.set_ints32(0, 50);

  ASSERT_EQ(tensor.ints32(0), 50);
  ASSERT_EQ(mat.at<int>(0, 0), 10);
  ASSERT_NE(mat.at<int>(0, 0), tensor.ints32(0));

  auto view = is::to_mat_view(&tensor);
  view.at<int>(0, 0) = 100;

  ASSERT_EQ(view.at<int>(0, 0), 100);
  ASSERT_EQ(view.at<int>(0, 0), tensor.ints32(0));
}

TEST(CVTests, OperationEquivalence) {
  auto A = is::to_tensor((cv::Mat_<double>(2, 4) << 10, 5, 2, 1, 3, 2, 3, 1));
  auto B = is::to_tensor((cv::Mat_<double>(4, 1) << 11, 6, 3, 2));
  auto C = is::to_tensor((cv::Mat_<double>(1, 3) << 3.5, 15.5, 20.2));
  auto R = (cv::Mat_<double>(2, 5) << 518, 2294, 2989.6, 196, 868, 1131.2);

  auto with_copy = is::to_mat(A) * is::to_mat(B) * is::to_mat(C);
  std::cout << with_copy << std::endl;
  ASSERT_TRUE(cv::countNonZero(with_copy == R) == 0);

  auto with_view = is::to_mat_view(&A) * is::to_mat_view(&B) * is::to_mat_view(&C);
  std::cout << with_view << std::endl;
  ASSERT_TRUE(cv::countNonZero(with_view == R) == 0);
}

}  // namespace