#include <gtest/gtest.h>
#include "oControl.h"
#include "oPunch.h"
#include "oEvent.h"
#include "oDataContainer.h"

class DomainTest : public ::testing::Test {
protected:
    oEvent oe;
    oDataContainer controlData;

    DomainTest() : controlData(64) {
        oe.oControlData = &controlData;
    }
};

TEST_F(DomainTest, ControlBasic) {
    oControl ctrl(&oe, 101);
    ctrl.setNumbers(L"101;102");
    EXPECT_EQ(ctrl.getId(), 101);
    EXPECT_TRUE(ctrl.hasNumber(101));
    EXPECT_TRUE(ctrl.hasNumber(102));
    EXPECT_FALSE(ctrl.hasNumber(103));
}

TEST_F(DomainTest, PunchBasic) {
    oPunch punch(&oe);
    // decodeString expects format type-time@unit#origin;
    // time is in seconds, but stored in 1/10s (timeConstSecond = 10)
    punch.decodeString("31-3600@1#123;");
    EXPECT_EQ(punch.getTypeCode(), 31);
    EXPECT_EQ(punch.getTimeInt(), 36000);
    EXPECT_EQ(punch.getPunchUnit(), 1);
}

TEST_F(DomainTest, SpecialPunches) {
    oPunch start(&oe);
    start.decodeString("1-1000;");
    EXPECT_TRUE(start.isStart());
    EXPECT_FALSE(start.isFinish());

    oPunch finish(&oe);
    finish.decodeString("2-2000;");
    EXPECT_TRUE(finish.isFinish());
    EXPECT_FALSE(finish.isStart());
}
