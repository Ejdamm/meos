#include <gtest/gtest.h>

#define protected public
#define private public
#include "oBase.h"
#include "oDataContainer.h"
#include "oEvent.h"
#include "oControl.h"
#include "oPunch.h"
#include "gdioutput.h"
#undef protected
#undef private

// Minimal oBase implementation for testing
class TestObject : public oBase {
    static const int dataSize = 256;
    alignas(16) uint8_t data[dataSize];
    alignas(16) uint8_t oldData[dataSize];
    vector<vector<wstring>> strData;
    oDataContainer &dc;
public:
    TestObject(oEvent* oe, oDataContainer &dc_) : oBase(oe), dc(dc_) {
        memset(data, 0, dataSize);
        memset(oldData, 0, dataSize);
        strData.resize(1);
        strData[0].resize(100); // Support some dynamic strings
    }
    wstring getInfo() const override { return L"TestObject"; }
    void changedObject() override {}
    void remove() override {}
    bool canRemove() const override { return true; }
    void merge(const oBase& input, const oBase* base) override {}
    oDataContainer& getDataBuffers(pvoid& d, pvoid& od, pvectorstr& sd) const override {
        d = (pvoid)data;
        od = (pvoid)oldData;
        sd = (pvectorstr)&strData;
        return dc;
    }
    int getDISize() const override { return dataSize; }
};

TEST(oBaseTest, BasicProperties) {
    oDataContainer dc(256);
    dc.addVariableInt("ExtId", oDataContainer::oIS64, "External Identifier");
    
    TestObject obj(nullptr, dc);
    obj.setExtIdentifier(12345);
    EXPECT_EQ(obj.getExtIdentifier(), 12345);
    EXPECT_EQ(obj.getInfo(), L"TestObject");
}

TEST(oDataContainerTest, IntAccess) {
    oDataContainer dc(256);
    dc.addVariableInt("TestInt", oDataContainer::oIS32, "Test Description");
    
    TestObject obj(nullptr, dc);
    obj.getDI().setInt("TestInt", 42);
    EXPECT_EQ(obj.getDCI().getInt("TestInt"), 42);
}

TEST(oDataContainerTest, StringAccess) {
    oDataContainer dc(256);
    dc.addVariableString("TestString", 20, "Test String Description");
    
    TestObject obj(nullptr, dc);
    obj.getDI().setString("TestString", L"Hello MeOS");
    EXPECT_EQ(obj.getDCI().getString("TestString"), L"Hello MeOS");
}

TEST(oControlTest, BasicProperties) {
    gdioutput gdi("test", 1.0);
    oEvent oe(gdi);
    
    // oControlData needs to be initialized with variables used by oControl
    oe.oControlData->addVariableInt("TimeAdjust", oDataContainer::oIS32, "Time Adjustment");
    oe.oControlData->addVariableInt("MinTime", oDataContainer::oIS32, "Minimum Time");
    oe.oControlData->addVariableInt("Rogaining", oDataContainer::oIS32, "Rogaining Points");
    oe.oControlData->addVariableInt("Radio", oDataContainer::oIS32, "Radio Control");

    oControl ctrl(&oe, 101);
    ctrl.setName(L"Control 101");
    EXPECT_EQ(ctrl.getName(), L"Control 101");
    EXPECT_EQ(ctrl.getId(), 101);
    
    ctrl.setNumbers(L"101,102");
    vector<int> numbers;
    ctrl.getNumbers(numbers);
    ASSERT_EQ(numbers.size(), 2);
    EXPECT_EQ(numbers[0], 101);
    EXPECT_EQ(numbers[1], 102);
}

TEST(oPunchTest, BasicProperties) {
    gdioutput gdi("test", 1.0);
    oEvent oe(gdi);
    
    oPunch punch(&oe);
    punch.setPunchUnit(31);
    punch.type = oPunch::PunchStart;
    punch.setTimeInt(1000, false);
    
    EXPECT_EQ(punch.getPunchUnit(), 31);
    EXPECT_EQ(punch.getTypeCode(), oPunch::PunchStart);
    EXPECT_EQ(punch.getTimeInt(), 1000);
}
