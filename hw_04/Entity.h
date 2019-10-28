#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum  EntityType { PLAYER, PLATFORM, ENEMY, BULLET, LETTER};
//enum  lastCollisionEntityType { PLAYER, PLATFORM, ENEMY, BULLET, LETTER};
enum AIState {IDLE, WALKING};
enum AIType {WALKER, RANDOM, HOMING};


class Entity {
public:
    
    EntityType entityType;
    AIState aiState;
    AIType aiType;
    EntityType lastCollisionType;
    
    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    //glm::vec3 acceleration;
    
    float width;
    float height;
    
    float speed;
    
    GLuint textureID;
    
    Entity();
    
    bool CheckCollision(Entity& other);
    
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
    
    void Update(float deltaTime, Entity player, Entity *objects, int objectCount, Entity *enemies, int enemyCount);
    void Render(ShaderProgram *program);
    
    void shoot(Entity player, Entity* bullets, float bullet_count);
    
    void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position);
    
    void AI(Entity player);
    void AIWalker(Entity player);
    void AIRandom(Entity player);
    void AIHoming(Entity player);
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
    
    
    float internalClock;
    std :: string description;
};



