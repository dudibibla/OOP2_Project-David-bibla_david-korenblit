//#include "LevelLoader.h"
//#include "Level.h"
//#include "ObjectFactory.h"
//#include "GameException.h"
//#include <iostream>
//#include <fstream>
//#include <sstream>
//
//std::unique_ptr<Level> LevelLoader::loadFromFile(const std::string& filename, int levelIndex) {
//    if (filename.empty()) {
//        throw InvalidLevelException(filename, "Empty filename provided");
//    }
//
//    ObjectFactory::initialize();
//
//    std::string jsonContent = readJsonFile(filename);
//    auto jsonData = parseSimpleJson(jsonContent);
//
//    return createLevelFromJson(jsonData, levelIndex);
//}
//
//std::string LevelLoader::readJsonFile(const std::string& filename) {
//    std::ifstream file(filename);
//    if (!file.is_open()) {
//        throw InvalidLevelException(filename, "Failed to open JSON file");
//    }
//
//    std::stringstream buffer;
//    buffer << file.rdbuf();
//    std::string content = buffer.str();
//
//    if (content.empty()) {
//        throw InvalidLevelException(filename, "JSON file is empty");
//    }
//
//    return content;
//}
//
//std::vector<std::map<std::string, std::string>> LevelLoader::parseSimpleJson(const std::string& jsonContent) {
//    if (jsonContent.empty()) {
//        throw InvalidLevelException("JsonParsing", "Empty JSON content provided");
//    }
//
//    std::vector<std::map<std::string, std::string>> objects;
//    std::istringstream stream(jsonContent);
//    std::string line;
//    std::map<std::string, std::string> currentObject;
//    bool insideObject = false;
//
//    while (std::getline(stream, line)) {
//        line.erase(0, line.find_first_not_of(" \t\n\r"));
//        line.erase(line.find_last_not_of(" \t\n\r,") + 1);
//
//        if (line == "{") {
//            insideObject = true;
//            currentObject.clear();
//        }
//        else if (line == "}" || line == "},") {
//            if (insideObject && !currentObject.empty()) {
//                objects.push_back(currentObject);
//                currentObject.clear();
//            }
//            insideObject = false;
//        }
//        else if (insideObject && line.find(":") != std::string::npos) {
//            size_t colonPos = line.find(':');
//            std::string key = extractQuotedString(line.substr(0, colonPos));
//            std::string value = extractValue(line.substr(colonPos + 1));
//
//            if (!key.empty() && !value.empty()) {
//                currentObject[key] = value;
//            }
//        }
//    }
//
//    return objects;
//}
//
//std::unique_ptr<Level> LevelLoader::createLevelFromJson(const std::vector<std::map<std::string, std::string>>& jsonData, int levelIndex) {
//    if (jsonData.empty()) {
//        throw InvalidLevelException("JsonLevel", "No objects found in JSON data");
//    }
//    PlayerSpawn spawn;
//    spawn.position = sf::Vector2f(0, 0);
//    spawn.angle = 90.0f;
//
//    for (const auto& objectData : jsonData) {
//        auto typeIt = objectData.find("type");
//        if (typeIt == objectData.end()) continue;
//
//        std::string type = typeIt->second;
//
//        if (type == "player_spawn") {
//            float x = getFloatFromMap(objectData, "x", 670.0f);
//            float y = getFloatFromMap(objectData, "y", 210.0f);
//            spawn.position = sf::Vector2f(x, y);
//            spawn.angle = getFloatFromMap(objectData, "angle", 90.0f);
//        }
//    }
//
//    std::string levelName = "Level " + std::to_string(levelIndex);
//    std::string bgTexture = "level_" + std::to_string(levelIndex - 1);
//
//    auto level = std::make_unique<Level>(levelName, bgTexture, spawn);
//
//    for (const auto& objectData : jsonData) {
//        auto typeIt = objectData.find("type");
//        if (typeIt == objectData.end()) continue;
//
//        std::string type = typeIt->second;
//
//        if (type == "player_spawn") {
//            continue;
//        }
//        if (type == "border") {
//            float x = getFloatFromMap(objectData, "x", 0.0f);
//            float y = getFloatFromMap(objectData, "y", 0.0f);
//            float width = getFloatFromMap(objectData, "width", 1.0f);
//            float height = getFloatFromMap(objectData, "height", 1.0f);
//            std::cout << "📦 Loading boundary: (" << x << ", " << y
//                << ") size: " << width << "x" << height << std::endl;
//            level->addBoundary(sf::FloatRect(x, y, width, height));
//            continue;
//        }
//        auto obj = ObjectFactory::createFromJsonData(type, objectData);
//        if (obj) {
//            level->addObject(std::move(obj));
//        }
//    }
//
//    return level;
//}
//
//std::string LevelLoader::extractQuotedString(const std::string& str) {
//    std::string trimmed = str;
//    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
//    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
//
//    if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
//        return trimmed.substr(1, trimmed.length() - 2);
//    }
//    return trimmed;
//}
//
//std::string LevelLoader::extractValue(const std::string& str) {
//    std::string trimmed = str;
//    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
//    trimmed.erase(trimmed.find_last_not_of(" \t\n\r,") + 1);
//
//    if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
//        return trimmed.substr(1, trimmed.length() - 2);
//    }
//    return trimmed;
//}
//
//float LevelLoader::getFloatFromMap(const std::map<std::string, std::string>& data, const std::string& key, float defaultValue) {
//    auto it = data.find(key);
//    if (it != data.end()) {
//        try {
//            return std::stof(it->second);
//        }
//        catch (const std::exception& e) {
//            throw InvalidLevelException("FloatConversion", "Failed to convert " + key + " to float: " + it->second);
//        }
//    }
//    return defaultValue;
//}


#include "LevelLoader.h"
#include "Level.h"
#include "ObjectFactory.h"
#include "GameException.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::unique_ptr<Level> LevelLoader::loadFromFile(const std::string& filename, int levelIndex) {
    if (filename.empty()) {
        throw InvalidLevelException(filename, "Empty filename provided");
    }

    ObjectFactory::initialize();

    std::string jsonContent = readJsonFile(filename);
    auto jsonData = parseSimpleJson(jsonContent);

    return createLevelFromJson(jsonData, levelIndex);
}

std::string LevelLoader::readJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw InvalidLevelException(filename, "Failed to open JSON file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    if (content.empty()) {
        throw InvalidLevelException(filename, "JSON file is empty");
    }

    return content;
}

std::vector<std::map<std::string, std::string>> LevelLoader::parseSimpleJson(const std::string& jsonContent) {
    if (jsonContent.empty()) {
        throw InvalidLevelException("JsonParsing", "Empty JSON content provided");
    }

    std::vector<std::map<std::string, std::string>> objects;
    std::istringstream stream(jsonContent);
    std::string line;
    std::map<std::string, std::string> currentObject;
    bool insideObject = false;

    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r,") + 1);

        if (line == "{") {
            insideObject = true;
            currentObject.clear();
        }
        else if (line == "}" || line == "},") {
            if (insideObject && !currentObject.empty()) {
                objects.push_back(currentObject);
                currentObject.clear();
            }
            insideObject = false;
        }
        else if (insideObject && line.find(":") != std::string::npos) {
            size_t colonPos = line.find(':');
            std::string key = extractQuotedString(line.substr(0, colonPos));
            std::string value = extractValue(line.substr(colonPos + 1));

            if (!key.empty() && !value.empty()) {
                currentObject[key] = value;
            }
        }
    }

    return objects;
}

std::unique_ptr<Level> LevelLoader::createLevelFromJson(const std::vector<std::map<std::string, std::string>>& jsonData, int levelIndex) {
    if (jsonData.empty()) {
        throw InvalidLevelException("JsonLevel", "No objects found in JSON data");
    }

    PlayerSpawn spawn;
    spawn.position = sf::Vector2f(0, 0);
    spawn.angle = 90.0f;

    for (const auto& objectData : jsonData) {
        auto typeIt = objectData.find("type");
        if (typeIt == objectData.end()) continue;

        std::string type = typeIt->second;

        if (type == "player_spawn") {
            spawn = ObjectFactory::createPlayerSpawn(objectData);
        }
    }

    std::string levelName = "Level " + std::to_string(levelIndex);
    std::string bgTexture = "level_" + std::to_string(levelIndex - 1);

    auto level = std::make_unique<Level>(levelName, bgTexture, spawn);

    for (const auto& objectData : jsonData) {
        auto typeIt = objectData.find("type");
        if (typeIt == objectData.end()) continue;

        std::string type = typeIt->second;

        if (type == "player_spawn") {
            continue;
        }
        if (type == "border") {
            level->addBoundary(ObjectFactory::createBoundary(objectData));
            continue;
        }

        auto obj = ObjectFactory::createFromJsonData(type, objectData);
        if (obj) {
            level->addObject(std::move(obj));
        }
    }

    return level;
}

std::string LevelLoader::extractQuotedString(const std::string& str) {
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

    if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
        return trimmed.substr(1, trimmed.length() - 2);
    }
    return trimmed;
}

std::string LevelLoader::extractValue(const std::string& str) {
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r,") + 1);

    if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
        return trimmed.substr(1, trimmed.length() - 2);
    }
    return trimmed;
}

float LevelLoader::getFloatFromMap(const std::map<std::string, std::string>& data, const std::string& key, float defaultValue) {
    auto it = data.find(key);
    if (it != data.end()) {
        try {
            return std::stof(it->second);
        }
        catch (const std::exception& e) {
            throw InvalidLevelException("FloatConversion", "Failed to convert " + key + " to float: " + it->second);
        }
    }
    return defaultValue;
}