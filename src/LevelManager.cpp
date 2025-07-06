#include "LevelManager.h"
#include "TextureManager.h"
#include "Constants.h"
#include "GameException.h"
#include "Car.h"
#include "LevelLoader.h"
#include <iostream>
#include <fstream>
#include <cmath>

LevelManager::LevelManager() : m_world(nullptr), m_currentLevelIndex(1), m_scoreTimeManager(nullptr) {
}

bool LevelManager::setLevel(std::unique_ptr<Level> level, b2World* world) {
    if (!level) {
        throw LevelOperationException("setLevel", "Cannot set null level");
    }

    if (!world) {
        throw LevelOperationException("setLevel", "Cannot set level with null physics world");
    }

    try {
        m_currentLevel = std::move(level);
        m_world = world;

        createBackground();
        spawnPlayerVehicle(world);

        return true;
    }
    catch (const std::exception& e) {
        throw LevelOperationException("setLevel", "Failed to initialize level: " + std::string(e.what()));
    }
}

void LevelManager::loadNextLevel() {
    m_currentLevelIndex++;
    std::string nextLevelFile = "level" + std::to_string(m_currentLevelIndex) + ".json";

    if (levelFileExists(m_currentLevelIndex)) {
        auto nextLevel = LevelLoader::loadFromFile(nextLevelFile, m_currentLevelIndex);
        setLevel(std::move(nextLevel), m_world);
        if (m_scoreTimeManager) {
            float levelTime = calculateTimeForLevel(m_currentLevelIndex);
            m_scoreTimeManager->startLevel(m_currentLevelIndex, levelTime);
        }
    }
    else {
        throw GameStateException("GameWon", "All levels completed successfully!");
    }
}


void LevelManager::resetPlayerVehicle() {
    if (!m_currentLevel) {
        throw LevelOperationException("resetPlayerVehicle", "No current level available for player reset");
    }

    if (!m_world) {
        throw LevelOperationException("resetPlayerVehicle", "No physics world available for player reset");
    }

    try {
        if (m_playerVehicle) {
            m_playerVehicle->resetToStartPosition();
        }
    }
    catch (const std::exception& e) {
        throw LevelOperationException("resetPlayerVehicle", "Failed to reset player vehicle: " + std::string(e.what()));
    }
}

void LevelManager::update(float deltaTime) {
    if (!m_playerVehicle) {
        throw LevelOperationException("update", "No player vehicle available for update");
    }

    try {
        m_playerVehicle->update(deltaTime);

        if (m_currentLevel) {
            updateObjects(deltaTime);
        }
    }
    catch (const std::exception& e) {
        throw LevelOperationException("update", "Failed to update level: " + std::string(e.what()));
    }
}

void LevelManager::render(sf::RenderWindow& window) {
    try {
        window.draw(m_background);

        if (!m_playerVehicle) {
            throw LevelOperationException("render", "No player vehicle available for rendering");
        }

        m_playerVehicle->draw(window);

        if (m_currentLevel) {
            renderObjects(window);
        }
    }
    catch (const std::exception& e) {
        throw LevelOperationException("render", "Failed to render level: " + std::string(e.what()));
    }
}

void LevelManager::handlePlayerInput(float deltaTime) {
    if (!m_playerVehicle) {
        throw LevelOperationException("handlePlayerInput", "No player vehicle available for input handling");
    }

    try {
        m_playerVehicle->handleInput(deltaTime);
    }
    catch (const std::exception& e) {
        throw LevelOperationException("handlePlayerInput", "Failed to handle player input: " + std::string(e.what()));
    }
}

void LevelManager::makePlayerVehicleSound() {
    if (!m_playerVehicle) {
        throw LevelOperationException("makePlayerVehicleSound", "No player vehicle available for sound");
    }

    try {
        m_playerVehicle->makeSound();
    }
    catch (const std::exception& e) {
        throw LevelOperationException("makePlayerVehicleSound", "Failed to make player vehicle sound: " + std::string(e.what()));
    }
}

std::vector<GameObject*> LevelManager::getAllObjects() const {
    std::vector<GameObject*> allObjects;

    if (!m_playerVehicle) {
        throw LevelOperationException("getAllObjects", "No player vehicle available for object collection");
    }

    allObjects.push_back(m_playerVehicle.get());

    if (m_currentLevel) {
        auto levelObjects = m_currentLevel->getAllObjectPointers();
        allObjects.insert(allObjects.end(), levelObjects.begin(), levelObjects.end());
    }

    return allObjects;
}

void LevelManager::createBackground() {
    if (!m_currentLevel) {
        throw LevelOperationException("createBackground", "No current level for background creation");
    }

    try {
        const sf::Texture& bgTexture = TextureManager::getInstance().getTexture(m_currentLevel->getBackgroundTexture());
        m_background.setSize(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
        m_background.setTexture(&bgTexture);

        if (bgTexture.getSize().x <= 1) {
            m_background.setFillColor(sf::Color(60, 60, 60));
        }
    }
    catch (const std::exception& e) {
        throw LevelOperationException("createBackground", "Failed to create background: " + std::string(e.what()));
    }
}

//void LevelManager::spawnPlayerVehicle(b2World* world) {
//    if (!m_currentLevel) {
//        throw LevelOperationException("spawnPlayerVehicle", "No current level for player spawn");
//    }
//
//    if (!world) {
//        throw LevelOperationException("spawnPlayerVehicle", "No physics world for player spawn");
//    }
//
//    try {
//        const PlayerSpawn& spawn = m_currentLevel->getPlayerSpawn();
//        m_playerVehicle = std::make_unique<Car>(spawn.position, "car_player");
//        m_playerVehicle->createPhysicsBody(world, spawn.position);
//
//        if (spawn.angle != 0.0f) {
//            m_playerVehicle->setRotation(spawn.angle);
//        }
//    }
//    catch (const std::exception& e) {
//        throw LevelOperationException("spawnPlayerVehicle", "Failed to spawn player vehicle: " + std::string(e.what()));
//    }
//}

void LevelManager::spawnPlayerVehicle(b2World* world) {
    const PlayerSpawn& spawn = m_currentLevel->getPlayerSpawn();
    m_playerVehicle = std::make_unique<Car>(spawn.position, "car_player", spawn.angle);
    m_playerVehicle->createPhysicsBody(world, spawn.position, spawn.angle);
}

void LevelManager::updateObjects(float deltaTime) {
    const auto& objects = m_currentLevel->getAllObjects();
    for (const auto& obj : objects) {
        if (!obj) {
            throw LevelOperationException("updateObjects", "Null object found in container");
        }
        obj->update(deltaTime);
    }
}

void LevelManager::renderObjects(sf::RenderWindow& window) {
    const auto& objects = m_currentLevel->getAllObjects();
    for (const auto& obj : objects) {
        if (!obj) {
            throw LevelOperationException("renderObjects", "Null object found in container");
        }
        obj->draw(window);
    }
}
void LevelManager::resetLevel() {
    if (!m_currentLevel || !m_world) {
        throw LevelOperationException("resetLevel", "Cannot reset - level or world not available");
    }

    const PlayerSpawn& spawn = m_currentLevel->getPlayerSpawn();
    m_playerVehicle = std::make_unique<Car>(spawn.position, "car_player", spawn.angle); 
    m_playerVehicle->createPhysicsBody(m_world, spawn.position, spawn.angle);          
}
void LevelManager::setScoreTimeManager(ScoreTimeManager* scoreTimeManager) {
    m_scoreTimeManager = scoreTimeManager;
}
bool LevelManager::levelFileExists(int levelNumber) {
    std::string filename = "level" + std::to_string(levelNumber) + ".json";
    std::ifstream file(filename);
    return file.good();
}

float LevelManager::calculateTimeForLevel(int levelNumber) {
    return 60.0f * std::pow(1.5f, levelNumber - 1);
}

void LevelManager::restartCurrentLevel()
{
    float levelTime = calculateTimeForLevel(m_currentLevelIndex);
    if (m_scoreTimeManager) {
        m_scoreTimeManager->startLevel(m_currentLevelIndex, levelTime);
    }
    resetLevel();
}
