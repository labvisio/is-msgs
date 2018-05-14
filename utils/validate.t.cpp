#include "tests.pb.h"
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

void assert_eq(is::wire::Status status, is::wire::StatusCode code) {
  if (status.code() != code) {
    status.PrintDebugString();
    exit(-1);
  }
}

int main() {
  is::tests::Bar bar;
  bar.set_myfloat(-0.9999);
  auto foo = bar.mutable_myfoo();
  foo->set_myint(-50);
  foo->set_myfloat(0.11);
  foo->set_mydouble(33.333);
  assert_eq(is::validate_message(bar), is::wire::StatusCode::OK);

  bar.set_myfloat(-1.0);
  assert_eq(is::validate_message(bar), is::wire::StatusCode::FAILED_PRECONDITION);

  bar.set_myfloat(10.0);
  assert_eq(is::validate_message(bar), is::wire::StatusCode::OK);

  foo->set_myint(2000);
  assert_eq(is::validate_message(bar), is::wire::StatusCode::OK);

  foo->set_myint(2200);
  assert_eq(is::validate_message(bar), is::wire::StatusCode::FAILED_PRECONDITION);

  foo->set_mydouble(33.334);
  assert_eq(is::validate_message(bar), is::wire::StatusCode::FAILED_PRECONDITION);
}