#include <gtest/gtest.h>
#include "TimeStamp.h"

TEST(UtilTest, TimeStampBasic) {
    TimeStamp ts;
    // Basic check that it doesn't crash
    EXPECT_GE(ts.getModificationTime(), 0);
}
