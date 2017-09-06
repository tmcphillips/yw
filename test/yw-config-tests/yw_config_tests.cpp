#include "yw_config_tests.h"

using namespace yw;
using namespace yw::config;
using namespace yw::test;

YW_TEST_FIXTURE(Configuration)

    Configuration configuration;

YW_TEST_SET

    YW_TEST(Configuration, BeforeInsertingAnySettingConfigurationSizeIsZero)
    {
        Assert::AreEqual(0, configuration.size());
    }

    YW_TEST(Configuration, AfterInsertingOneSettingConfigurationSizeIsOne)
    {
        configuration.insert(Setting{ "key", "value" });
        Assert::AreEqual(1, configuration.size());
    }

    YW_TEST(Configuration, AfterInsertingTwoSettingsWithDifferentKeysConfigurationSizeIsTwo)
    {
        configuration.insert(Setting{ "key1", "value1" });
        configuration.insert(Setting{ "key2", "value2" });
        Assert::AreEqual(2, configuration.size());
    }

    YW_TEST(Configuration, AfterInsertingTwoSettingsWithSameKeyConfigurationSizeIsOne)
    {
        configuration.insert(Setting{ "key1", "value1" });
        configuration.insert(Setting{ "key1", "value2" });
        Assert::AreEqual(1, configuration.size());
    }

    YW_TEST(Configuration, AfterInsertingOneSettingGetReturnsSettingWIthInsertedKeyAndValue)
    {
        configuration.insert(Setting{ "key", "value" });
        Assert::AreEqual("key", configuration.get("key").key);
        Assert::AreEqual("value", configuration.get("key").value);
    }

    YW_TEST(Configuration, AfterInsertingTwoSettingsWithDifferentKeysGetOnSecondKeyReturnsSettingWithSecondInsertedKeyAndValue)
    {
        configuration.insert(Setting{ "key1", "value1" });
        configuration.insert(Setting{ "key2", "value2" });
        Expect::AreEqual(2, configuration.size());
        Assert::AreEqual("key2", configuration.get("key2").key);
        Assert::AreEqual("value2", configuration.get("key2").value);
    }

    YW_TEST(Configuration, AfterInsertingTwoSettingsWithSameKeyAndUnspecifiedSourceGetOReturnsSettingWithSecondInsertedValue)
    {
        configuration.insert(Setting{ "key1", "value1" });
        configuration.insert(Setting{ "key1", "value2" });
        Expect::AreEqual(1, configuration.size());
        Expect::AreEqual("key1", configuration.get("key1").key);
        Assert::AreEqual("value2", configuration.get("key1").value);
    }

YW_TEST_END
