#pragma once
#include <vector>
#include <memory>
#include "CollisionResult.h"

class GameStateManager {
private:
    bool m_gameWon;

public:
    GameStateManager();
    ~GameStateManager() = default;

    void processCollisionResults(std::vector<std::unique_ptr<CollisionResult>>& results);
    void resetGameState();
    void checkGameWonCondition();

    bool isGameWon() const { return m_gameWon; }
};