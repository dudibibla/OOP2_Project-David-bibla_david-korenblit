#pragma once
#include "LevelManager.h"
#include "CollisionDetector.h"
#include "GameStateManager.h"
#include "EffectManager.h"
#include "MenuManager.h"
#include "ScoreTimeManager.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>

class Vehicle;

enum class GameState {
    LOADING,
	MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    YOU_WIN
};

class GameManager {
public:
    GameManager();
    ~GameManager();

    void initialize();
    void run();
    void shutdown();

private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::Font m_font;
    sf::Text m_uiText;

    std::unique_ptr<b2World> m_world;
    std::unique_ptr<LevelManager> m_levelManager;
    std::unique_ptr<CollisionDetector> m_collisionDetector;
    std::unique_ptr<GameStateManager> m_gameStateManager;
    std::unique_ptr<EffectManager> m_effectManager;
    std::unique_ptr<MenuManager> m_menuManager;
    std::unique_ptr<ScoreTimeManager> m_scoreTimeManager;


    bool m_isRunning;
    bool m_fontLoaded;
    GameState m_gameState;

    void processEvents();
    void handleInput(float deltaTime);
    void update(float deltaTime);
    void render();
    void initializeMenu();

    void initializePhysics();
    void initializeManagers();
    void loadLevel(const std::string& levelFile, int levelIndex);
    void initializeUI();
    void setupUIText();

    void handleWindowEvents(const sf::Event& event);
    void handleMenuEvents(const sf::Event& event);
    void handleGameplayEvents(const sf::Event& event);
    void handleEscapeKey();
    void updatePhysics(float deltaTime);
    void renderYouWinScreen();

    
    void updateUI();
    void updateGameState();
    void renderMenuScreen();
    std::string buildUIString(Vehicle* playerVehicle);

    void renderLoadingScreen();
    void renderGameplay();
    void renderPauseOverlay();
    void renderGameOverScreen();

    void togglePause();
    void restartLevel();
};
//
//#pragma once
//#include "LevelManager.h"
//#include "CollisionDetector.h"
//#include "GameStateManager.h"
//#include "EffectManager.h"
//#include "MenuManager.h"
//#include "ScoreTimeManager.h"
//#include <SFML/Graphics.hpp>
//#include <memory>
//
//class Vehicle;
//
//enum class GameState {
//    LOADING,
//    MENU,
//    PLAYING,
//    PAUSED,
//    GAME_OVER,
//    YOU_WIN
//};
//
//class GameManager {
//public:
//    GameManager();
//    ~GameManager();
//
//    void initialize();
//    void run();
//    void shutdown();
//
//private:
//    sf::RenderWindow m_window;
//    sf::Clock m_clock;
//    sf::Font m_font;
//    sf::Text m_uiText;
//
//    std::unique_ptr<LevelManager> m_levelManager;
//    std::unique_ptr<CollisionDetector> m_collisionDetector;
//    std::unique_ptr<GameStateManager> m_gameStateManager;
//    std::unique_ptr<EffectManager> m_effectManager;
//    std::unique_ptr<MenuManager> m_menuManager;
//    std::unique_ptr<ScoreTimeManager> m_scoreTimeManager;
//
//    bool m_isRunning;
//    bool m_fontLoaded;
//    GameState m_gameState;
//
//    void processEvents();
//    void handleInput(float deltaTime);
//    void update(float deltaTime);
//    void render();
//    void initializeMenu();
//
//    void initializeManagers();
//    void initializeUI();
//    void setupUIText();
//
//    void handleWindowEvents(const sf::Event& event);
//    void handleMenuEvents(const sf::Event& event);
//    void handleGameplayEvents(const sf::Event& event);
//    void handleEscapeKey();
//    void updatePhysics(float deltaTime);
//    void renderYouWinScreen();
//
//    void updateUI();
//    void updateGameState();
//    void renderMenuScreen();
//    std::string buildUIString(Vehicle* playerVehicle);
//
//    void renderLoadingScreen();
//    void renderGameplay();
//    void renderPauseOverlay();
//    void renderGameOverScreen();
//
//    void togglePause();
//};