#pragma once
#include <vector>
#include <memory>
#include "CollisionResult.h"
#include "ScoreTimeManager.h"
#include "SoundManager.h"

class LevelManager;

class EffectManager {
private:
    LevelManager* m_levelManager;
    ScoreTimeManager* m_scoreTimeManager;
    bool m_needsPlayerReset;
    bool m_needsNextLevel;

public:
    EffectManager();
    ~EffectManager() = default;
    void setScoreTimeManager(ScoreTimeManager* scoreManager);
    void setLevelManager(LevelManager* levelManager);
    void processEffects(const std::vector<std::unique_ptr<CollisionResult>>& results);
    void processDelayedActions();
    void checkPlayerResetRequired();

private:
    void applyCollisionResult(const CollisionResult& result);
};