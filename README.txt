# Parking 2D Game - End of Semester B Project
## Student Details
David korenblit 213871734
David Bible 207402850
## General Project Description
We were required to create a parking game, including textures, sounds, and multiple levels. The game allows the player to control a vehicle and park it in designated spots while avoiding collisions with static and dynamic obstacles. The game features realistic physics, time-based scoring, and progressive difficulty across multiple levels.
## System Architecture
### Game Managers
1. GameManager:
   - Main class managing overall game behavior
   - Responsible for game loop, physics integration, state management
   - Contains multiple sub-managers: Level, Collision, Effect, Menu, ScoreTime
2. LevelManager:
   - Handles level loading and management
   - Manages player vehicle spawning and updates
   - Controls level progression and transitions
3. MenuManager:
   - Handles main menu and instructions display
   - Manages user interaction in menu states
   - Controls game initialization from menu
4. SoundManager:
   - Manages all game sounds and music
   - Supports background loops and sound effects
   - Implements volume control and muting
5. TextureManager:
   - Handles texture loading and management
   - Ensures efficient resource usage with caching
   - Manages transparency and texture optimization
6. CollisionDetector:
   - Manages collision detection between all objects
   - Handles boundary collision detection
   - Implements AABB collision algorithms
7. EffectManager:
   - Processes collision results and applies effects
   - Manages game state changes from collisions
   - Handles delayed actions and level resets
8. ScoreTimeManager:
   - Manages scoring system and time limits
   - Handles level completion bonuses
   - Controls time-based game over conditions
### Object Hierarchy
1. GameObject (Base class):
   
   A. Vehicle (Physics-enabled):
   - Car:
     - Player-controlled vehicle with Box2D physics
     - Handles keyboard input and movement
     - Manages health, shields, and power-ups
     - Includes realistic vehicle dynamics
   - MovingCar:
     - AI-controlled moving obstacles
     - Follows predefined paths with respawn mechanics
     - Creates dynamic challenges for the player
   
   B. StaticObstacle:
   - ParkedCar: Static vehicle obstacles that block paths
   - StaticObstacle: Traffic cones and barriers
   - ParkingSpot: Target areas for successful parking
   
   C. DynamicObstacle (Base class):
   - MovingCar: Vehicles that move along set trajectories
   - Includes respawn timers and off-screen detection
## File List
### Header Files (.h):
GameManager.h, LevelManager.h, Car.h, Vehicle.h, CollisionDetector.h, GameObject.h, EffectManager.h, MenuManager.h, ScoreTimeManager.h, TextureManager.h, SoundManager.h, LevelLoader.h, ObjectFactory.h, CollisionResult.h, Level.h, GameStateManager.h, Constants.h, GameException.h, Button.h, StaticObstacle.h, DynamicObstacle.h, MovingCar.h, ParkedCar.h, ParkingSpot.h

### Implementation Files (.cpp):
main.cpp, GameManager.cpp, Car.cpp, CollisionDetector.cpp, LevelManager.cpp, GameObject.cpp, EffectManager.cpp, MenuManager.cpp, CollisionResult.cpp, Level.cpp, LevelLoader.cpp, Button.cpp, GameException.cpp, GameStateManager.cpp, DynamicObstacle.cpp, MovingCar.cpp, ObjectFactory.cpp, ParkedCar.cpp, ParkingSpot.cpp, ScoreTimeManager.cpp, SoundManager.cpp, StaticObstacle.cpp, TextureManager.cpp, Vehicle.cpp
## Main Data Structures
- std::vector<std::unique_ptr<GameObject>> for game object management
- std::map<std::string, sf::Texture> for texture caching
- std::map<std::string, sf::SoundBuffer> for audio resource management
- std::vector<sf::FloatRect> for level boundary definitions
- b2World for Box2D physics simulation
- Smart pointers (unique_ptr) for safe memory management
## Notable Algorithms
- AABB collision detection with expanded bounds for vehicles
- Box2D physics integration for realistic vehicle movement
- Visitor pattern implementation for collision handling between different object types
- Resource management using Singleton pattern for TextureManager and SoundManager
- JSON parsing system for level loading with custom lightweight parser
- State machine for game state management (Menu, Playing, Paused, Game Over)
- Time-based scoring algorithm with performance bonuses
## Known Bugs
None
## Additional Notes
Due to the difficult and complicated situation, we were forced to abandon elements that were originally planned. Several classes have incomplete implementations due to the same security complexities and ongoing attacks we experienced, and all their code remains in comments.

- Game designed with realistic physics simulation using Box2D engine
- Effort to maintain SOLID principles and clean code architecture
- Design patterns used: Singleton (for resource managers), Factory (for object creation), Visitor (for collision handling)
- Exception handling system with custom hierarchy for robust error management
- Code organized into small, focused functions for improved readability and maintenance
- Complete integration of graphics (SFML) and physics (Box2D) engines