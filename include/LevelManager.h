
#pragma once
#include "Level.h"
#include "Vehicle.h"
#include "ScoreTimeManager.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include <vector>

class LevelManager {
private:
    std::unique_ptr<Level> m_currentLevel;
    std::unique_ptr<Vehicle> m_playerVehicle;
    sf::RectangleShape m_background;
    b2World* m_world;
    int m_currentLevelIndex;
    ScoreTimeManager* m_scoreTimeManager;

    void createBackground();
    void spawnPlayerVehicle(b2World* world);
    void updateObjects(float deltaTime);
    void renderObjects(sf::RenderWindow& window);
    bool levelFileExists(int levelNumber);
    float calculateTimeForLevel(int levelNumber);

public:
    LevelManager();
    ~LevelManager() = default;

    void setScoreTimeManager(ScoreTimeManager* scoreTimeManager);
    bool setLevel(std::unique_ptr<Level> level, b2World* world);
    void loadNextLevel();
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void handlePlayerInput(float deltaTime);
    void makePlayerVehicleSound();
    void resetPlayerVehicle();
    void resetLevel();
    void restartCurrentLevel();
    Level* getCurrentLevel() const { return m_currentLevel.get(); }
    std::vector<GameObject*> getAllObjects() const;
    Vehicle* getPlayerVehicle() const { return m_playerVehicle.get(); }
};