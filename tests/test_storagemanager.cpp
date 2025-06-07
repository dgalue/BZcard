#include <gtest/gtest.h>
#define private public
#include "StorageManager.h"
#undef private

TEST(StorageManagerTest, GetDefaultSettingsReturnsExpectedValues) {
    StorageManager manager;
    UserSettings defaults = manager.getDefaultSettings();
    EXPECT_EQ(defaults.brightness, 128);
    EXPECT_EQ(defaults.displayTimeout, 60000u);
    EXPECT_EQ(defaults.sleepTimeout, 120000u);
    EXPECT_EQ(defaults.rtcWakeInterval, 5);
    EXPECT_TRUE(defaults.valid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
