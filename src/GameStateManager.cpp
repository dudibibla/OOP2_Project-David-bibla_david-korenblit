#include "GameStateManager.h"
#include "GameException.h"

GameStateManager::GameStateManager() : m_gameWon(false) {
}

void GameStateManager::processCollisionResults(std::vector<std::unique_ptr<CollisionResult>>& results) {
   /* for (auto& result : results) {
        if (result->getType() == CollisionType::GOAL_REACHED) {
            m_gameWon = true;
        }
    }*/
}

void GameStateManager::resetGameState() {
    m_gameWon = false;
}

void GameStateManager::checkGameWonCondition() {
    if (m_gameWon) {
        throw GameStateException("GameWon", "Player has achieved victory condition");
    }
}