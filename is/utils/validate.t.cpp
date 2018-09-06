#include "gtest/gtest.h"
#include "is/msgs/tests.pb.h"
#include "validate.hpp"

/*
message Foo {
  int32 myint = 1 [(validate.rules).int32 = {gte: -50, lte: 2000}];
  float myfloat = 2 [(validate.rules).float = {gt: 0.1, lt: 0.3}];
  double mydouble = 3 [(validate.rules).double = {lte: 33.333}];
};

message Bar {
  Foo myfoo = 1;
  float myfloat = 2 [(validate.rules).float = {gt: -1.0}];
};
*/

namespace {

TEST(ValidateTests, NumericValidation) {
  is::tests::Bar bar;
  bar.set_myfloat(-0.9999);
  EXPECT_NO_THROW(is::validate_message(bar));

  auto foo = bar.mutable_myfoo();
  EXPECT_ANY_THROW(is::validate_message(bar));

  foo->set_myint(-50);
  foo->set_myfloat(0.11);
  foo->set_mydouble(33.333);
  EXPECT_NO_THROW(is::validate_message(bar));

  bar.set_myfloat(-1.0);
  EXPECT_ANY_THROW(is::validate_message(bar));

  bar.set_myfloat(10.0);
  EXPECT_NO_THROW(is::validate_message(bar));

  foo->set_myint(2000);
  EXPECT_NO_THROW(is::validate_message(bar));

  foo->set_myint(2200);
  EXPECT_ANY_THROW(is::validate_message(bar));

  foo->set_mydouble(33.334);
  EXPECT_ANY_THROW(is::validate_message(bar));
}

/*
message Repeats {
  repeated Foo foos = 1;
  Foo foo2 = 2 [(validate.rules).message = {required: true}];
  repeated int32 int32s = 3 [(validate.rules).int32 = {lt: -10}];
  repeated uint32 uint32s = 4 [(validate.rules).uint32 = {gt: 10}];
  repeated float floats = 5 [(validate.rules).float = {lte: -10}];
  repeated double doubles = 6 [(validate.rules).double = {gte: -10}];
  repeated int64 int64s = 7 [(validate.rules).int64 = {lt: 10}];
  repeated uint64 uint64s = 8 [(validate.rules).uint64 = {gt: 100}];
}
*/

TEST(ValidateTests, RepeatedFields) {
  is::tests::Repeats r;
  // foo2 validation is required and by default is invalid => should fail
  EXPECT_ANY_THROW(is::validate_message(r));

  auto foo2 = r.mutable_foo2();
  foo2->set_myint(400);
  foo2->set_myfloat(0.222222222);
  foo2->set_mydouble(-300);
  EXPECT_NO_THROW(is::validate_message(r));

  auto foo = r.add_foos();
  EXPECT_ANY_THROW(is::validate_message(r));

  foo->set_myfloat(0.121212);
  EXPECT_NO_THROW(is::validate_message(r));

  r.add_int32s(-10);
  EXPECT_ANY_THROW(is::validate_message(r));
  r.clear_int32s();
  r.add_int32s(-11);
  EXPECT_NO_THROW(is::validate_message(r));

  r.add_uint32s(10);
  EXPECT_ANY_THROW(is::validate_message(r));
  r.clear_uint32s();
  r.add_uint32s(11);
  EXPECT_NO_THROW(is::validate_message(r));

  r.add_floats(-9.9999);
  EXPECT_ANY_THROW(is::validate_message(r));
  r.clear_floats();
  r.add_floats(-10);
  EXPECT_NO_THROW(is::validate_message(r));

  r.add_doubles(-10.000001);
  EXPECT_ANY_THROW(is::validate_message(r));
  r.clear_doubles();
  r.add_doubles(-10);
  EXPECT_NO_THROW(is::validate_message(r));

  r.add_int64s(10);
  EXPECT_ANY_THROW(is::validate_message(r));
  r.clear_int64s();
  r.add_int64s(9);
  EXPECT_NO_THROW(is::validate_message(r));

  r.add_uint64s(100);
  EXPECT_ANY_THROW(is::validate_message(r));
  r.clear_uint64s();
  r.add_uint64s(101);
  EXPECT_NO_THROW(is::validate_message(r));
}

}  // namespace
