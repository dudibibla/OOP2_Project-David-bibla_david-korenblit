#include "EffectManager.h"
#include "LevelManager.h"
#include "GameException.h"
#include <iostream>

EffectManager::EffectManager() : m_levelManager(nullptr), m_needsPlayerReset(false), m_needsNextLevel(false), m_scoreTimeManager(nullptr) {
}

void EffectManager::setScoreTimeManager(ScoreTimeManager* scoreManager)
{
    m_scoreTimeManager = scoreManager;
}

void EffectManager::setLevelManager(LevelManager* levelManager) {
    if (!levelManager) {
        throw EffectProcessingException("Cannot set null LevelManager");
    }
    m_levelManager = levelManager;
}

void EffectManager::processEffects(const std::vector<std::unique_ptr<CollisionResult>>& results) {
    if (!m_levelManager) {
        throw EffectProcessingException("No LevelManager set for effect processing");
    }

    for (const auto& result : results) {
        if (!result) {
            throw EffectProcessingException("Null collision result found in effect processing");
        }
        applyCollisionResult(*result);
    }
}

void EffectManager::processDelayedActions() {
    if (!m_levelManager) {
        throw EffectProcessingException("No LevelManager available for delayed actions");
    }

    if (m_needsPlayerReset) {
        m_levelManager->resetLevel();
        m_needsPlayerReset = false;
    }

    if (m_needsNextLevel) {
        m_levelManager->loadNextLevel();
        m_needsNextLevel = false;
    }
}

void EffectManager::checkPlayerResetRequired() {
    if (m_needsPlayerReset) {
        throw EffectProcessingException("Player reset is required but not yet processed");
    }
}

void EffectManager::applyCollisionResult(const CollisionResult& result) {
    if (m_scoreTimeManager && result.getScoreChange() != 0) {
        m_scoreTimeManager->addScore(result.getScoreChange());
        if (result.getEffectType() == EffectType::CRASH) {
            SoundManager::getInstance().stopAllBackgroundSounds();
            SoundManager::getInstance().playSound("crash");
            if (result.shouldRestartLevel()) {
                m_needsPlayerReset = true;
            }

            if (result.getType() == CollisionType::GOAL_REACHED) {
                m_needsNextLevel = true;
            }
        }
    }

    if (result.shouldRestartLevel()) {
        m_needsPlayerReset = true;
    }

    if (result.getType() == CollisionType::GOAL_REACHED) {
        m_needsNextLevel = true;
    }
}