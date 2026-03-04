#include <gtest/gtest.h>
#include "localizer.h"
#include <fstream>

class LocalizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        lang.init();
        
        // Create a temporary lng file for testing
        std::ofstream fout("test_lang.lng");
        fout << "Hello = World" << std::endl;
        fout << "MeOS = MeOS Software" << std::endl;
        fout << "Test X = Test result X" << std::endl;
        fout.close();
    }

    void TearDown() override {
        lang.unload();
        remove("test_lang.lng");
    }
};

TEST_F(LocalizerTest, BasicTranslation) {
    lang.get().addLangResource(L"Test", L"test_lang.lng");
    lang.get().loadLangResource(L"Test");

    EXPECT_EQ(lang.tl(L"Hello"), L"World");
    EXPECT_EQ(lang.tl(L"MeOS"), L"MeOS Software");
}

TEST_F(LocalizerTest, Substitution) {
    lang.get().addLangResource(L"Test", L"test_lang.lng");
    lang.get().loadLangResource(L"Test");

    EXPECT_EQ(lang.tl(L"Test X#Result"), L"Test result Result");
}

TEST_F(LocalizerTest, Untranslated) {
    lang.get().addLangResource(L"Test", L"test_lang.lng");
    lang.get().loadLangResource(L"Test");

    EXPECT_EQ(lang.tl(L"Unknown"), L"Unknown");
}

TEST_F(LocalizerTest, SpecialPrefixes) {
    lang.get().addLangResource(L"Test", L"test_lang.lng");
    lang.get().loadLangResource(L"Test");

    EXPECT_EQ(lang.tl(L"#Literal"), L"Literal");
    EXPECT_EQ(lang.tl(L"@Raw"), L"Raw");
}

TEST_F(LocalizerTest, NumericPrefixSuffix) {
    lang.get().addLangResource(L"Test", L"test_lang.lng");
    lang.get().loadLangResource(L"Test");

    // "1. Hello" should be translated as "1. " + tl("Hello") = "1. World"
    EXPECT_EQ(lang.tl(L"1. Hello"), L"1. World");
    
    // "Hello:" should be translated as tl("Hello") + ":" = "World:"
    EXPECT_EQ(lang.tl(L"Hello:"), L"World:");
}
