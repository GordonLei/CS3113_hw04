#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool acceptsInput = true;


ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


#define PLATFORM_COUNT 80
#define ENEMIES_COUNT 4
#define BULLET_COUNT 2

struct GameState {
    Entity player;
    Entity platforms[PLATFORM_COUNT];
    
    Entity enemies[ENEMIES_COUNT];
    
    //float enemies_alive = ENEMIES_COUNT;
    
    // You can improve the stuttering of shooting bullets by having 2 more arrays
    // one array holds the indexes of unusued bullets
    // one array holds the indexes of used bullets (or those that are traveling)
    Entity bullets[BULLET_COUNT];
    
    Entity message_fail;
    Entity message_success;
};

GameState state;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.width = 1.0f;
    state.player.position = glm::vec3(-8.5, 6, 0);
    //state.player.acceleration = glm::vec3(0, -9.81f, 0);
    state.player.textureID = LoadTexture("me.png");
    
    
    GLuint evilTextureID = LoadTexture("evil.png");
    
    GLuint laserTextureID = LoadTexture("orange.png");
    
    GLuint dirtTextureID = LoadTexture("dirt.png");
    //GLuint grassTextureID = LoadTexture("grass.png");
    
    GLuint fontTextureID = LoadTexture("font1.png");
    
    std::string message_fail = "Game Over";
    std::string message_success = "You Win";
    
    state.message_fail.entityType = LETTER;
    state.message_fail.textureID = fontTextureID;
    state.message_fail.description = message_fail;
    state.message_fail.isActive = false;
    state.message_fail.position = glm::vec3(-3.5,0,0);
    
    state.message_success.entityType = LETTER;
    state.message_success.textureID = fontTextureID;
    state.message_success.description = message_success;
    state.message_success.isActive = false;
    state.message_success.position = glm::vec3(-2.5,0,0);
    
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = evilTextureID;
    state.enemies[0].isStatic = false;
    state.enemies[0].position = glm::vec3(3,-2.25,0);
    state.enemies[0].aiState = IDLE;
    state.enemies[0].aiType = WALKER;
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = evilTextureID;
    state.enemies[1].isStatic = false;
    state.enemies[1].position = glm::vec3(-2,-2.25,0);
    state.enemies[1].aiState = IDLE;
    state.enemies[1].aiType = RANDOM;
    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = evilTextureID;
    state.enemies[2].isStatic = false;
    state.enemies[2].position = glm::vec3(-2,-2.25,0);
    state.enemies[2].aiState = WALKING;
    state.enemies[2].aiType = HOMING;
    
    for(int i = 3; i < ENEMIES_COUNT; i++){
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = evilTextureID;
        state.enemies[i].isStatic = false;
        state.enemies[i].position = glm::vec3(
                                              (9.0 - (float)(rand() % 18)),
                                              (6.5 - (float) (rand() % 13)),
                                              0);
        
        int randInt = (rand() % 3);
        
        if(randInt == 0){
            state.enemies[i].aiState = IDLE;
            state.enemies[i].aiType = WALKER;
        }
        else if(randInt == 1){
            state.enemies[i].aiState = IDLE;
            state.enemies[i].aiType = RANDOM;
        }
        else if(randInt == 2){
            state.enemies[i].aiState = WALKING;
            state.enemies[i].aiType = HOMING;
        }
        
        
    }
    
    
    
    int curr_platform = 0;
    float curr_xvalue = -8.5f;
    float curr_yvalue = -7.00f;
    
    
    //make the bottom row
    //curr_xvalue = -8.5f;
    
    for(int counter = 0; counter < 18; ++counter){
        state.platforms[curr_platform].textureID = dirtTextureID;
        state.platforms[curr_platform].position = glm::vec3(curr_xvalue, -7.0f, 0);
        ++curr_platform;
        curr_xvalue += 1.0f;
    }
    
    curr_xvalue = -8.5f;
    //make top row
    for(int counter = 0; counter < 18; ++counter){
        state.platforms[curr_platform].textureID = dirtTextureID;
        state.platforms[curr_platform].position = glm::vec3(curr_xvalue, 7.0f, 0);
        ++curr_platform;
        curr_xvalue += 1.0f;
    }
    
    //make the lefthand column
    //curr_xvalue = -9.5f;
    
    for(int counter = 0; counter < 15; ++counter){
        state.platforms[curr_platform].textureID = dirtTextureID;
        state.platforms[curr_platform].position = glm::vec3(-9.5f, curr_yvalue, 0);
        ++curr_platform;
        curr_yvalue += 1.0f;
    }
    
    //make the righthand column
    //curr_xvalue = 9.5f;
    curr_yvalue = -7.0f;
    
    for(int counter = 0; counter < 15; ++counter){
        state.platforms[curr_platform].textureID = dirtTextureID;
        state.platforms[curr_platform].position = glm::vec3(9.5f, curr_yvalue, 0);
        ++curr_platform;
        curr_yvalue += 1.0f;
    }

    //random platforms. make sure that it does not make them on where players or enemies are
    for (int i = curr_platform; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].textureID = dirtTextureID;
        state.platforms[i].position = glm::vec3(
                                                (8.0 - (float)(rand() % 16)),
                                                (5.5 - (float) (rand() % 11)),
                                                0);
        // x = -10 to 10
        // y = -7.5 to 7.5
        
        // THIS COMMENTED CODE WAS SUPPOSE TO MAKE SURE THAT DIRT WONT BE PLACED
        // WHERE ENEMIES OR PLAYERS ARE AT.
        
        /*
        glm::vec3 dummy_position = glm::vec3(
                  (9.0 - (float)(rand() % 18)),
                  (6.5 - (float) (rand() % 13)),
                  0);
        state.platforms[i].position = dummy_position;
        
        bool share_space_with_entity = true;
        while(share_space_with_entity){
            
            bool share_space_with_nothing = true;
            
            while(share_space_with_nothing){
                for(int i = 0; i < ENEMIES_COUNT; ++i){
                    if(state.platforms[i].CheckCollision(state.enemies[i])){
                        share_space_with_nothing = false;
                        break;
                    }
                }
                if(!share_space_with_nothing){
                    share_space_with_nothing = true;
                    state.platforms[i].position = glm::vec3(
                                                            (9.0 - (float)(rand() % 18)),
                                                            (6.5 - (float) (rand() % 13)),
                                                            0);
                }
                else{
                    share_space_with_nothing = false;
                }
                
            }
            
            share_space_with_nothing = true;
            while(share_space_with_nothing){
                if(state.platforms[i].CheckCollision(state.player)){
                    share_space_with_nothing = false;
                    break;
                }
                if(!share_space_with_nothing){
                    share_space_with_nothing = true;
                    state.platforms[i].position = glm::vec3(
                                                            (9.0 - (float)(rand() % 18)),
                                                            (6.5 - (float) (rand() % 13)),
                                                            0);
                }
                else{
                    share_space_with_nothing = false;
                }
            }
            
            share_space_with_entity = false;
            
            
            */
            /*
            if(dummy_position != state.player.position){
                share_space_with_entity = false;
                break;
            }
            else{
                for(int i = 0; i < ENEMIES_COUNT; ++i){
                    if((dummy_position != state.enemies[i].position) != share_space_with_entity){
                        share_space_with_entity = (dummy_position != state.enemies[i].position) ;
                    }
                }
            }
            if(share_space_with_entity){
                dummy_position = glm::vec3(
                                          (8.5 - (float)(rand() % 17)),
                                          (6.0 - (float) (rand() % 12)),
                                          0);
            }
            
            
        }
    */
        
        //state.platforms[i].position = dummy_position;
    

    }
    
    //correct stuff if needed?
    /*
    for (int i = 0; i < ENEMIES_COUNT; i++){
        
        state.enemies[i].CheckCollisionsX(state.platforms, PLATFORM_COUNT);
        state.enemies[i].CheckCollisionsY(state.platforms, PLATFORM_COUNT);
    }
    state.player.CheckCollisionsX(state.platforms, PLATFORM_COUNT);
    state.player.CheckCollisionsY(state.platforms, PLATFORM_COUNT);
    */
    
    for (int i = 0; i < BULLET_COUNT; i++){
        
        state.bullets[i].entityType = BULLET;
        state.bullets[i].textureID = laserTextureID;
        state.bullets[i].isStatic = false;
        state.bullets[i].isActive = false;
        state.bullets[i].position = glm::vec3(0,0,0);
        
    }

    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    //projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    projectionMatrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
    
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        state.player.shoot(state.player, state.bullets, BULLET_COUNT);
                        break;
                        
                }
                break;
        }
    }
    
    state.player.velocity.x = 0;
    state.player.velocity.y = 0;
    
    
    // Check for pressed/held keys below
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_A])
    {
        state.player.velocity.x = -1.5f;
    }
    else if  (keys[SDL_SCANCODE_D])
    {
        state.player.velocity.x = 1.5f;
    }
    else if  (keys[SDL_SCANCODE_W])
    {
        state.player.velocity.y = 1.5f;
    }
    else if  (keys[SDL_SCANCODE_S])
    {
        state.player.velocity.y = -1.5f;
    }
    if (glm::length(state.player.velocity) > 1.0f)
    {
        state.player.velocity = glm::normalize(state.player.velocity);
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player.Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMIES_COUNT);
        
        for(int i = 0; i < ENEMIES_COUNT; i++){
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT,state.enemies, ENEMIES_COUNT);
        }
        
        for(int i = 0; i < BULLET_COUNT; i++){
            state.bullets[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT,state.enemies, ENEMIES_COUNT);
        }
        
        
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player.Render(&program);
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        state.platforms[i].Render(&program);
    }
    
    for (int i = 0; i < ENEMIES_COUNT; i++)
    {
        if(state.enemies[i].isActive){
            state.enemies[i].Render(&program);
        }
        
    }
    
    for (int i = 0; i < BULLET_COUNT; i++)
    {
        if(state.bullets[i].isActive){
            state.bullets[i].Render(&program);
            
        }
    }
    
    if(state.message_success.isActive || state.message_fail.isActive){
        state.message_fail.Render(&program);
        state.message_success.Render(&program);
    }
    
    
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

void checkGameStatus(){
    bool all_enemies_dead = true;
    
    for (int i = 0; i < ENEMIES_COUNT; i++)
    {
        if(state.enemies[i].isActive){
            all_enemies_dead = false;
        }
        
    }
    
    if(all_enemies_dead){
        //std :: cout << "WE DID IT" << std :: endl;
        state.player.velocity = glm::vec3(0, 0, 0);
        state.message_success.isActive = true;
        acceptsInput = false;
        
    }
    
    if (state.player.lastCollisionType == ENEMY){
        state.player.velocity = glm::vec3(0, 0, 0);
        for (int i = 0; i < ENEMIES_COUNT; i++)
        {
            //std :: cout << "STOP MOVING" << std :: endl;
            state.enemies[i].velocity = glm::vec3(0,0,0);
            state.enemies[i].isStatic = true;
            
            
        }
        //state.player.acceleration = glm::vec3(0, 0, 0);
        
        //std :: cout << "WE DID NOT DO IT" << std :: endl;
        
        state.message_fail.isActive = true;
        acceptsInput = false;
    }
}


int main(int argc, char* argv[]) {
    Initialize();
    while (gameIsRunning) {
        if(acceptsInput){
            ProcessInput();

            checkGameStatus();
        
        }

        Update();
        Render();
    }
    Shutdown();
    return 0;
}
