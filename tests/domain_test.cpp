#include <gtest/gtest.h>
#include "oBase.h"
#include "oDataContainer.h"
#include "oEvent.h"
#include "gdioutput.h"

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
