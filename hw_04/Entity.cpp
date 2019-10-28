#include "Entity.h"

Entity::Entity()
{
    entityType = PLATFORM;
    isStatic = true;
    isActive = true;
    position = glm::vec3(0);
    speed = 0;
    width = 1;
    height = 1;
    internalClock = 0.0;
    description = "temporary description";
}

bool Entity::CheckCollision(Entity& other)
{
    if (isStatic == true) return false;
    if (isActive == false || other.isActive == false) return false;

    
    float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
    float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
        // std :: cout << other.entityType << std :: endl;
        if(entityType == PLAYER && other.entityType == PLATFORM){
            lastCollisionType = PLATFORM;
        }
        else if (entityType == PLAYER && other.entityType == ENEMY){
            lastCollisionType = ENEMY;
        }
        
        if(entityType == BULLET && other.entityType == PLATFORM){
            isActive = false;
            isStatic = true;
        }
        else if(entityType == BULLET && other.entityType == ENEMY){
            // std :: cout << "killed" << std :: endl;
            other.isActive = false;
            other.isStatic = true;
            
            isActive = false;
            isStatic = true;
            
            
        }
        
        
        
        /*
        if (entityType == PLAYER && other.entityType == COIN)
        {
            other.isActive = false;
        }
        */
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        //Entity object = objects[i];
        
        if (CheckCollision(objects[i]))
        {
            float ydist = fabs(position.y - objects[i].position.y);
            float penetrationY = fabs(ydist - (height / 2) - (objects[i].height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        //Entity object = objects[i];
        
        if (CheckCollision(objects[i]))
        {
            float xdist = fabs(position.x - objects[i].position.x);
            float penetrationX = fabs(xdist - (width / 2) - (objects[i].width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}


void Entity::shoot(Entity player, Entity* bullets, float bullet_count)
{
    //std :: cout << "shot a bullet" << std:: endl;
    bool shot_one_new_bullet = false;
    for(size_t index = 0; index < bullet_count; ++index){
        if (!shot_one_new_bullet){
            if(bullets[index].isActive == false){
                shot_one_new_bullet = true;
                
                bullets[index].isActive = true;
                bullets[index].isStatic = false;
                if(player.velocity.y > 0){
                    bullets[index].position = glm::vec3(player.position.x,
                                                        player.position.y + .1f,
                                                        0);
                    bullets[index].velocity.y = 2.0f;
                    bullets[index].velocity.x = 0.0f;
                }
                else if (player.velocity.y < 0){
                    bullets[index].position = glm::vec3(player.position.x,
                                                        player.position.y - .1f,
                                                        0);
                    bullets[index].velocity.y = -2.0f;
                    bullets[index].velocity.x = 0.0f;
                }
                else if(player.velocity.x >= 0){
                    bullets[index].position = glm::vec3(player.position.x + .1f,
                                                        player.position.y,
                                                        0);
                    bullets[index].velocity.x = 2.0f;
                    bullets[index].velocity.y = 0.0f;
                }
                else if(player.velocity.x < 0){
                    bullets[index].position = glm::vec3(player.position.x - .1f,
                                                        player.position.y,
                                                        0);
                    bullets[index].velocity.x = -2.0f;
                    bullets[index].velocity.y = 0.0f;
                }
                
            }
        }
        /*
        if (bullets[index].isActive){
            if(bullets[index].internalClock >= 20.0f){
                bullets[index].velocity.x = 0;
                bullets[index].velocity.y = 0;
                bullets[index].internalClock = 0.0f;
                bullets[index].isActive = false;
                bullets[index].isStatic = true;
            }
            else{
                bullets[index].internalClock += 1.0f;
            }
            
        }
    */
    }
}

void Entity::AIWalker(Entity player){
    switch (aiState){
        case IDLE:
            if (glm::distance(position, player.position) < 3.0f){
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if (glm::distance(position, player.position) > 3.0f){
                aiState = IDLE;
                velocity.x = 0.0f;
                velocity.y = 0.0f;
            }
            else if (player.position.x > position.x){
                velocity.x = 1.0f;

                if(player.position.y > position.y){
                    velocity.y = 1.0f;
                }
                else{
                    velocity.y = -1.0f;
                }
                if (glm::length(velocity) > 1.0f)
                {
                    velocity = glm::normalize(velocity);
                }
            }
            else{
                velocity.x = -1.0f;
                
                if(player.position.y > position.y){
                    velocity.y = 1.0f;
                }
                else{
                    velocity.y = -1.0f;
                }
                if (glm::length(velocity) > 1.0f)
                {
                    velocity = glm::normalize(velocity);
                }
            }
            
            break;
    }
}


void Entity::AIRandom(Entity player){
    switch (aiState){
        //not really idle but it will do random motion in idle
        case IDLE:
            if (glm::distance(position, player.position) < 3.0f){
                aiState = WALKING;
            }
            else{
                if(internalClock == 0.0f || internalClock >= 50.0f){
                    velocity.x = 1 - ((float) (rand() % 3));
                    velocity.y = 1 - ((float) (rand() % 3));
                    internalClock = 0.1f;
                }
                else{
                    internalClock += 1.0f;
                    //std :: cout << internalClock << std:: endl;
                }
            }
            break;
            
        case WALKING:
            if (player.position.x > position.x){
                velocity.x = 1.0f;
                
                if(player.position.y > position.y){
                    velocity.y = 1.0f;
                }
                else{
                    velocity.y = -1.0f;
                }
                if (glm::length(velocity) > 1.0f)
                {
                    velocity = glm::normalize(velocity);
                }
            }
            else{
                velocity.x = -1.0f;
                
                if(player.position.y > position.y){
                    velocity.y = 1.0f;
                }
                else{
                    velocity.y = -1.0f;
                }
                if (glm::length(velocity) > 1.0f)
                {
                    velocity = glm::normalize(velocity);
                }
            }
            
            break;
    }
}

void Entity::AIHoming(Entity player){
    switch (aiState){
        case IDLE:
            break;
        case WALKING:
            if (player.position.x > position.x){
                velocity.x = 1.0f;
                
                if(player.position.y > position.y){
                    velocity.y = 1.0f;
                }
                else{
                    velocity.y = -1.0f;
                }
                if (glm::length(velocity) > 1.0f)
                {
                    velocity = glm::normalize(velocity);
                }
            }
            else{
                velocity.x = -1.0f;
                
                if(player.position.y > position.y){
                    velocity.y = 1.0f;
                }
                else{
                    velocity.y = -1.0f;
                }
                if (glm::length(velocity) > 1.0f)
                {
                    velocity = glm::normalize(velocity);
                }
            }
            
            break;
    }
}


void Entity::AI(Entity player){
    switch (aiType){
        case WALKER:
            AIWalker(player);
            break;
        case RANDOM:
            AIRandom(player);
            break;
        case HOMING:
            AIHoming(player);
            break;
    }
    
}



void Entity::Update(float deltaTime, Entity player, Entity *objects, int objectCount, Entity *enemies, int enemyCount)
{
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    //velocity += acceleration * deltaTime;
    
    
    if(entityType == ENEMY){
        if(CheckCollision(player)){
            for (int i = 0; i < enemyCount; i++)
            {
                velocity = glm::vec3(0,0,0);
                isStatic = true;
                
            }
        }
        else if(!isStatic){
            AI(player);
        }
        else{
            velocity = glm::vec3(0,0,0);
            isStatic = true;
        }
        
    }
    
    if(entityType == BULLET){
        if (isActive){
            if(internalClock >= 50.0f){
                velocity.x = 0;
                velocity.y = 0;
                internalClock = 0.0f;
                isActive = false;
                isStatic = true;
            }
            else{
                internalClock += 1.0f;
            }
            
        }
        
    }
    
    
    position.y += velocity.y * deltaTime;
    position.x += velocity.x * deltaTime;
    CheckCollisionsY(objects, objectCount);    // Fix if needed
    
    
    if (entityType == PLAYER){
        CheckCollisionsX(enemies, enemyCount);
        CheckCollisionsY(enemies, enemyCount);
    }
    
    /*
    else if(entityType == BULLET){
        CheckCollisionsX(enemies, enemyCount);
        CheckCollisionsY(enemies, enemyCount);
    }
    */
    
    if(entityType == BULLET){
        CheckCollisionsX(enemies, enemyCount);
        CheckCollisionsY(enemies, enemyCount);
    }
    
    position.x += velocity.x * deltaTime;        // Move on X
    position.y += velocity.y * deltaTime;
    CheckCollisionsX(objects, objectCount);    // Fix if needed
}

void Entity::DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position){
    float width = 1.0f / 16.0f;
    float height = 1.0f/ 16.0f;
    std::vector <float> vertices;
    std::vector <float> texCoords;
    for(int i = 0; i < text.size(); i++){
        int index = (int)text[i];
        //std::cout << index << std :: endl;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        texCoords.insert(texCoords.end(), {u,v + height, u + width, v+height, u+width, v, u, v+ height, u + width, v, u, v});
        float offset = (size + spacing) * i;
        vertices.insert(vertices.end(), {offset + (-0.5f * size), (-0.5f * size),
            offset + (0.5f * size), (-0.5f * size),
            offset + (0.5f * size), (0.5f * size),
            offset + (-0.5f * size), (-0.5f * size),
            offset + (0.5f * size), (0.5f * size),
            offset + (-0.5f * size), (0.5f * size),
        });
    }
    
    //std::cout << text.size() << " " << vertices.size() << " " << texCoords.size() << std::endl;
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program -> SetModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2.0f);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}


void Entity::Render(ShaderProgram *program) {
    if(!isActive){
        return;
    }
    
    else if(entityType == LETTER){
        //std::cout << description << std::endl;
        DrawText(program, textureID, description, 1.0f, 0.0025f, position);
        
    }
    
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

