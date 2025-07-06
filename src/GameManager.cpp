#include "GameManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "Constants.h"
#include "GameException.h"
#include <iostream>
#include <cmath>
#include "LevelLoader.h"

GameManager::GameManager()
    : m_window(sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT), Constants::WINDOW_TITLE)
    , m_isRunning(false)
    , m_fontLoaded(false)
    , m_gameState(GameState::MENU)
{
    m_window.setFramerateLimit(Constants::FPS);
}

GameManager::~GameManager() {
    shutdown();
}

void GameManager::initialize() {
    try {
        TextureManager::getInstance().loadAllGameTextures();

        SoundManager::getInstance().loadAllGameSounds();
        SoundManager::getInstance().playBackgroundLoop("game_start");


        initializeUI();
        m_scoreTimeManager = std::make_unique<ScoreTimeManager>();
        initializeMenu();
        m_isRunning = true;
        m_gameState = GameState::MENU;
    }
    catch (const GameException& e) {
        std::cerr << "Game initialization failed: " << e.getFullMessage() << std::endl;
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error during initialization: " << e.what() << std::endl;
        throw ManagerInitializationException("GameManager", "Unexpected initialization error");
    }
}

void GameManager::initializeMenu() {
    if (m_fontLoaded) {
        m_menuManager = std::make_unique<MenuManager>(&m_font);
        m_menuManager->initializeMenu();
    }
}

void GameManager::run() {
    while (m_isRunning && m_window.isOpen()) {
        sf::Time deltaTime = m_clock.restart();
        float dt = deltaTime.asSeconds();

        try {
            processEvents();

            if (m_gameState == GameState::PLAYING) {
                handleInput(dt);
                update(dt);
            }

            if (m_gameState == GameState::MENU && m_menuManager) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
                m_menuManager->update(sf::Vector2f(mousePos.x, mousePos.y));
            }

            render();
        }
        catch (const GameStateException& e) {
            if (e.getStateName() == "GameWon") {
                m_gameState = GameState::YOU_WIN;
            }
            else if (e.getStateName() == "GameRestart") {
                restartLevel();
            }
            else {
                std::cerr << "Game state error: " << e.getFullMessage() << std::endl;
            }
        }
        catch (const GameException& e) {
            std::cerr << "Game error: " << e.getFullMessage() << std::endl;
            m_gameState = GameState::GAME_OVER;
        }
        catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            m_gameState = GameState::GAME_OVER;
        }
    }
}

void GameManager::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        handleWindowEvents(event);

        if (m_gameState == GameState::PLAYING || m_gameState == GameState::YOU_WIN) {
            handleGameplayEvents(event);
        }

        if (m_gameState == GameState::MENU) {
            handleMenuEvents(event);
        }
    }
}

void GameManager::handleInput(float deltaTime) {
    if (!m_levelManager) {
        throw ManagerInitializationException("LevelManager", "LevelManager not available for input handling");
    }

    if (m_gameState == GameState::PLAYING) {
        m_levelManager->handlePlayerInput(deltaTime);
    }
}

void GameManager::update(float deltaTime) {
    if (m_gameState != GameState::PLAYING) return;
    SoundManager::getInstance().update();
    m_scoreTimeManager->update();

    if (m_scoreTimeManager->isTimeUp()) {
        m_gameState = GameState::GAME_OVER;
        return;
    }

    updatePhysics(deltaTime);
    if (!m_levelManager) {
        throw ManagerInitializationException("LevelManager", "LevelManager not available for update");
    }
    m_levelManager->update(deltaTime);
    if (!m_collisionDetector || !m_gameStateManager || !m_effectManager) {
        throw ManagerInitializationException("CollisionSystem", "Collision system components not initialized");
    }

    auto collisionResults = m_collisionDetector->detectCollisions(m_levelManager->getAllObjects());

    if (m_levelManager->getCurrentLevel()) {
        auto boundaryResult = m_collisionDetector->checkPlayerBoundaryCollision(
            m_levelManager->getPlayerVehicle(),
            m_levelManager->getCurrentLevel()->getBoundaries()
        );

        if (boundaryResult->hasCollision()) {
            collisionResults.push_back(std::move(boundaryResult));
        }
    }

    std::vector<std::unique_ptr<CollisionResult>> resultsCopy;
    for (const auto& result : collisionResults) {
        if (result) {
            resultsCopy.push_back(std::make_unique<CollisionResult>(*result));
        }
    }

    m_gameStateManager->processCollisionResults(collisionResults);
    m_effectManager->processEffects(resultsCopy);
    m_effectManager->processDelayedActions();
    m_gameStateManager->checkGameWonCondition();
    updateUI();
    updateGameState();
}

void GameManager::render() {
    m_window.clear(sf::Color(40, 40, 40));

    switch (m_gameState) {
    case GameState::MENU:
        renderMenuScreen();
        break;
    case GameState::LOADING:
        renderLoadingScreen();
        break;
    case GameState::PLAYING:
        renderGameplay();
        break;
    case GameState::PAUSED:
        renderGameplay();
        renderPauseOverlay();
        break;
    case GameState::GAME_OVER:
        renderGameplay();
        renderGameOverScreen();
        break;
    case GameState::YOU_WIN:
        renderGameplay();
        renderYouWinScreen();
        break;
    }

    m_window.display();
}

void GameManager::shutdown() {
    SoundManager::shutdown();
    m_scoreTimeManager.reset();
    m_levelManager.reset();
    m_collisionDetector.reset();
    m_gameStateManager.reset();
    m_effectManager.reset();
    m_world.reset();
    m_menuManager.reset();

    TextureManager::shutdown();
}

void GameManager::initializePhysics() {
    m_world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
    if (!m_world) {
        throw ManagerInitializationException("PhysicsWorld", "Failed to create Box2D world");
    }
}

void GameManager::initializeManagers() {
    try {
        m_levelManager = std::make_unique<LevelManager>();
        m_collisionDetector = std::make_unique<CollisionDetector>();
        m_gameStateManager = std::make_unique<GameStateManager>();
        m_effectManager = std::make_unique<EffectManager>();
        m_effectManager->setLevelManager(m_levelManager.get());
        m_effectManager->setScoreTimeManager(m_scoreTimeManager.get());

    }
    catch (const std::exception& e) {
        std::cerr << "Exception in initializeManagers: " << e.what() << std::endl;
        throw;
    }
}

void GameManager::loadLevel(const std::string& levelFile, int levelIndex) {
    auto level = LevelLoader::loadFromFile(levelFile, levelIndex);
    if (!level) {
        throw InvalidLevelException(levelFile, "Failed to load level from file");
    }

    if (!m_levelManager->setLevel(std::move(level), m_world.get())) {
        throw LevelOperationException("setLevel", "Failed to set level in LevelManager");
    }
}

void GameManager::initializeUI() {
    m_fontLoaded = m_font.loadFromFile(Constants::Assets::RETRO_FONT);
    if (m_fontLoaded) {
        setupUIText();
    }
}

void GameManager::setupUIText() {
    m_uiText.setFont(m_font);
    m_uiText.setCharacterSize(16);
    m_uiText.setFillColor(Constants::Colors::WHITE);
    m_uiText.setPosition(10, 10);
}

void GameManager::handleWindowEvents(const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        m_isRunning = false;
        m_window.close();
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            handleEscapeKey();
        }
    }
}

void GameManager::handleMenuEvents(const sf::Event& event) {
    if (!m_menuManager) return;

    MenuOption option = m_menuManager->handleMenuInput(event);

    switch (option) {
    case MenuOption::START_GAME:
    {
        SoundManager::getInstance().stopAllBackgroundSounds();
        initializePhysics();
        initializeManagers();
        loadLevel("level1.json", 1);
        float level1Time = 60.0f * std::pow(1.5f, 0);
        m_scoreTimeManager->startLevel(1, level1Time);
        m_levelManager->setScoreTimeManager(m_scoreTimeManager.get());

        m_gameState = GameState::PLAYING;
        break;
    }
    case MenuOption::EXIT:
        m_isRunning = false;
        m_window.close();
        break;
    case MenuOption::INSTRUCTIONS:
        break;

    default:
        break;

    }
}

void GameManager::handleGameplayEvents(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::H:
            if (!m_levelManager) {
                throw ManagerInitializationException("LevelManager", "LevelManager not available for sound");
            }
            m_levelManager->makePlayerVehicleSound();
            break;
        case sf::Keyboard::P:
            if (m_gameState == GameState::PLAYING) {
                togglePause();
            }
            break;
        case sf::Keyboard::R:
            restartLevel();
            break;
        case sf::Keyboard::M:
            if (m_gameState == GameState::GAME_OVER) {
                m_gameState = GameState::MENU;
                SoundManager::getInstance().stopAllSounds();
                SoundManager::getInstance().playBackgroundLoop("game_start");
            }
            break;
        default:
            break;
        }
    }
}

void GameManager::handleEscapeKey() {
    switch (m_gameState) {
    case GameState::PLAYING:
        m_gameState = GameState::PAUSED;
        break;
    case GameState::PAUSED:
        m_gameState = GameState::PLAYING;
        break;
    default:
        m_isRunning = false;
        m_window.close();
        break;
    }
}

void GameManager::updatePhysics(float deltaTime) {
    if (!m_world) {
        throw ManagerInitializationException("PhysicsWorld", "Physics world not available for update");
    }

    float timeStep = deltaTime;
    if (timeStep > 1.0f / 30.0f) {
        timeStep = 1.0f / 30.0f;
    }

    const int velocityIterations = 6;
    const int positionIterations = 2;

    m_world->Step(timeStep, velocityIterations, positionIterations);
}

void GameManager::updateUI() {
    if (!m_fontLoaded || !m_levelManager || !m_levelManager->getPlayerVehicle()) {
        return;
    }

    auto playerVehicle = m_levelManager->getPlayerVehicle();
    std::string uiInfo = buildUIString(playerVehicle);
    m_uiText.setString(uiInfo);
}

std::string GameManager::buildUIString(Vehicle* playerVehicle) {
    return m_scoreTimeManager->getUIString();

}

void GameManager::updateGameState() {
}

void GameManager::renderMenuScreen() {
    m_window.clear(sf::Color(30, 30, 30));

    if (m_menuManager) {
        m_menuManager->renderMenu(m_window);
    }
}

void GameManager::renderLoadingScreen() {
    if (m_fontLoaded) {
        sf::Text loadingText("Loading...", m_font, 32);
        loadingText.setFillColor(sf::Color::White);
        loadingText.setPosition(Constants::WINDOW_WIDTH / 2 - 100, Constants::WINDOW_HEIGHT / 2);
        m_window.draw(loadingText);
    }
}

void GameManager::renderGameplay() {
    if (!m_levelManager) {
        throw ManagerInitializationException("LevelManager", "LevelManager not available for rendering");
    }

    m_levelManager->render(m_window);

    if (m_fontLoaded) {
        m_window.draw(m_uiText);
    }
}

void GameManager::renderPauseOverlay() {
    sf::RectangleShape overlay(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    m_window.draw(overlay);

    if (m_fontLoaded) {
        sf::Text pauseText("PAUSED - Press ESC to continue", m_font, 24);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setPosition(Constants::WINDOW_WIDTH / 2 - 200, Constants::WINDOW_HEIGHT / 2);
        m_window.draw(pauseText);
    }
}

void GameManager::renderGameOverScreen() {
    TextureManager& tm = TextureManager::getInstance();
    const sf::Texture& gameOverTexture = tm.getTexture("gameover");

    sf::Sprite gameOverSprite;
    gameOverSprite.setTexture(gameOverTexture);

    sf::Vector2u textureSize = gameOverTexture.getSize();
    sf::Vector2f windowSize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    gameOverSprite.setScale(
        windowSize.x / textureSize.x,
        windowSize.y / textureSize.y
    );

    m_window.draw(gameOverSprite);

    static bool gameOverSoundPlayed = false;
    if (!gameOverSoundPlayed) {
        SoundManager::getInstance().stopAllSounds();
        SoundManager::getInstance().playSound("gameover");
        gameOverSoundPlayed = true;
    }
}

void GameManager::renderYouWinScreen() {
    TextureManager& tm = TextureManager::getInstance();
    const sf::Texture& winTexture = tm.getTexture("youwin");

    sf::Sprite winSprite;
    winSprite.setTexture(winTexture);

    sf::Vector2u textureSize = winTexture.getSize();
    sf::Vector2f windowSize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    winSprite.setScale(
        windowSize.x / textureSize.x,
        windowSize.y / textureSize.y
    );

    m_window.draw(winSprite);

    if (!SoundManager::getInstance().isCurrentlyPlaying("victory")) {
        SoundManager::getInstance().stopAllSounds();
        SoundManager::getInstance().playSound("victory");
    }
}

void GameManager::togglePause() {
    if (m_gameState == GameState::PLAYING) {
        m_gameState = GameState::PAUSED;
    }
    else if (m_gameState == GameState::PAUSED) {
        m_gameState = GameState::PLAYING;
    }
}

void GameManager::restartLevel() {
    try {
        if (!m_gameStateManager) {
            throw ManagerInitializationException("GameStateManager", "GameStateManager not available for restart");
        }

        m_gameStateManager->resetGameState();
        m_gameState = GameState::LOADING;
        loadLevel("level1.json", 1);
        float level1Time = 60.0f * std::pow(1.5f, 0);
        m_scoreTimeManager->startLevel(1, level1Time);
        m_gameState = GameState::PLAYING;
    }
    catch (const GameException& e) {
        std::cerr << "Failed to restart level: " << e.getFullMessage() << std::endl;
        m_gameState = GameState::GAME_OVER;
    }
}