#include "base/Log.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::HasSubstr;
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;

#ifndef NDEBUG
#define DEBUG_TEXT "\ttest file:1\n"
#endif

TEST(LogTests, print_simpleString_outputIsValid) {
  CaptureStdout();

  CLOG->print("test file", 1, "test message");

  EXPECT_THAT(GetCapturedStdout(),
              HasSubstr("INFO: test message\n" DEBUG_TEXT));
}

TEST(LogTests, print_withArgs_outputIsValid) {
  CaptureStdout();

  CLOG->print("test file", 1, "test %d %.2f %s", 1, 1.234, "test arg");

  EXPECT_THAT(GetCapturedStdout(),
              HasSubstr("INFO: test 1 1.23 test arg\n" DEBUG_TEXT));
}

TEST(LogTests, print_withPrintLevel_outputIsValid) {
  CaptureStdout();

  CLOG->print(CLOG_PRINT "test message");

  EXPECT_THAT(GetCapturedStdout(), "test message\n");
}
