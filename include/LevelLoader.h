#pragma once
#include "Level.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

class LevelLoader {
public:
    static std::unique_ptr<Level> loadFromFile(const std::string& filename, int levelIndex = 1);

private:
    static std::string readJsonFile(const std::string& filename);
    static std::vector<std::map<std::string, std::string>> parseSimpleJson(const std::string& jsonContent);
    static std::unique_ptr<Level> createLevelFromJson(const std::vector<std::map<std::string, std::string>>& jsonData, int levelIndex);
    static std::string extractQuotedString(const std::string& str);
    static std::string extractValue(const std::string& str);
    static float getFloatFromMap(const std::map<std::string, std::string>& data, const std::string& key, float defaultValue = 0.0f);
};