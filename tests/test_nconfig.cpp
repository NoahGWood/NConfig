/**
 * @file test_nconfig.cpp
 * @author Noah G. Wood (@NoahGWood)
 * @brief Tests for the NConfig library.
 * @version 0.1
 * @date 2025-05-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "NTest.h"
#include "NConfig.h"
#include <filesystem>
#include <fstream>

TEST(NConfigSetGetBasic) {
    NConfig cfg;
    cfg.set("str", std::string("hello"));
    cfg.set("int", 123);
    cfg.set("float", 3.14f);
    cfg.set("bool", true);

    ASSERT_EQ(cfg.get("str", std::string("")), "hello");
    ASSERT_EQ(cfg.get("int", 0), 123);
    ASSERT_EQ(cfg.get("float", 0.f), 3.14f);
    ASSERT_EQ(cfg.get("bool", false), true);
}

TEST(NConfigVectors) {
    NConfig cfg;
    cfg.set("ints", std::vector<int>{1, 2, 3});
    cfg.set("bools", std::vector<bool>{true, false, true});
    cfg.set("strs", std::vector<std::string>{"a", "b", "c"});

    auto ints = cfg.get<std::vector<int>>("ints", {});
    auto bools = cfg.get<std::vector<bool>>("bools", {});
    auto strs = cfg.get<std::vector<std::string>>("strs", {});

    ASSERT_EQ(ints.size(), 3);
    ASSERT_EQ(bools[1], false);
    ASSERT_EQ(strs[2], "c");
}

TEST(NConfigListParsingAndTrimming) {
    NConfig cfg;
    cfg.set<std::string>("raw", " 10 , 20 ,  30 ");
    auto vals = cfg.get_list<int>("raw");
    ASSERT_EQ(vals.size(), 3);
    ASSERT_EQ(vals[0], 10);
    ASSERT_EQ(vals[2], 30);
}

TEST(NConfigFallbacks) {
    NConfig cfg;
    ASSERT_EQ(cfg.get("unset.string", std::string("default")), "default");
    ASSERT_EQ(cfg.get("unset.int", 999), 999);
    ASSERT_EQ(cfg.get("unset.bool", true), true);
}

TEST(NConfigSaveLoadRoundTrip) {
    std::string path = (std::filesystem::temp_directory_path() / "nconfig_test.conf").string();

    {
        NConfig cfg;
        cfg.set("main.version", 7);
        cfg.set("main.name", std::string("VehiTech"));
        cfg.set("flags.enabled", true);
        cfg.set("items", std::vector<int>{4,5,6});
        cfg.save(path);
    }

    {
        NConfig loaded;
        ASSERT_TRUE(loaded.load(path));
        ASSERT_EQ(loaded.get("main.version", 0), 7);
        ASSERT_EQ(loaded.get("main.name", std::string("")), "VehiTech");
        ASSERT_EQ(loaded.get("flags.enabled", false), true);

        auto items = loaded.get<std::vector<int>>("items",{});
        ASSERT_EQ(items.size(), 3);
        ASSERT_EQ(items[1], 5);
    }

    std::filesystem::remove(path);
}

TEST(NConfigSectionedKeys) {
    NConfig cfg;
    cfg.set("ui.width", 1920);
    cfg.set("ui.height", 1080);
    cfg.set("audio.volume", 75);

    ASSERT_EQ(cfg.get("ui.width", 0), 1920);
    ASSERT_EQ(cfg.get("audio.volume", 0), 75);
}

TEST(NConfigKeysAndSections) {
    NConfig cfg;
    cfg.set("graphics.width", 1920);
    cfg.set("graphics.height", 1080);
    cfg.set("audio.volume", 75);
    cfg.set("audio.enabled", true);

    auto keys = cfg.keys();
    ASSERT_TRUE(keys.size() == 4);
    ASSERT_TRUE(std::find(keys.begin(), keys.end(), "graphics.width") != keys.end());
    ASSERT_TRUE(cfg.has_key("audio.volume"));
    ASSERT_TRUE(!cfg.has_key("network.enabled"));

    auto sections = cfg.sections();
    ASSERT_TRUE(sections.size() == 2);
    ASSERT_TRUE(std::find(sections.begin(), sections.end(), "graphics") != sections.end());

    auto audio_keys = cfg.keys_in("audio");
    ASSERT_TRUE(audio_keys.size() == 2);
    ASSERT_TRUE(std::find(audio_keys.begin(), audio_keys.end(), "volume") != audio_keys.end());
    ASSERT_TRUE(std::find(audio_keys.begin(), audio_keys.end(), "enabled") != audio_keys.end());
}
