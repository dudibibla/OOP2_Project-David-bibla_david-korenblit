
#pragma once
#include "GameObject.h"
#include "Level.h"
#include <memory>
#include <map>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>

struct PlayerSpawn;

class ObjectFactory {
public:
    using DataMap = std::map<std::string, std::string>;

private:
    static sf::Vector2f extractPosition(const DataMap& data);
    static sf::Vector2f extractDirection(const DataMap& data);
    static std::string extractTexture(const DataMap& data, const std::string& defaultTexture);
    static float extractFloat(const DataMap& data, const std::string& key, float defaultValue = 0.0f);

    template<typename T>
    static std::unique_ptr<GameObject> createBasicObject(const DataMap& data, const std::string& defaultTexture);

    template<typename T>
    static std::unique_ptr<GameObject> createMovingObject(const DataMap& data, float defaultSpeed, float defaultRespawn);

    static std::unique_ptr<GameObject> createParkedCarWithRandomTexture(const DataMap& data);

public:
    static void initialize();
    static std::unique_ptr<GameObject> create(const std::string& type, float x, float y, const std::string& texture = "");
    static std::unique_ptr<GameObject> createMoving(const std::string& type, float startX, float startY,
        float dirX, float dirY, float speed, float respawnDelay, const std::string& texture = "");
    static std::unique_ptr<GameObject> createFromJsonData(const std::string& type, const DataMap& jsonData);

    static sf::FloatRect createBoundary(const DataMap& jsonData);
    static PlayerSpawn createPlayerSpawn(const DataMap& jsonData);
};