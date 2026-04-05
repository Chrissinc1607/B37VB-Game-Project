#include <raylib.h>
#include <stdlib.h>
#include <math.h>

#define PLAYER_LIFES 5
#define FPS 60
#define SCORE 0
#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, PAUSED, ENDING } GameScreen; //Sets up different screens to be used 

typedef struct Player { //Sets up the type player  
    Vector2 position;
    Vector2 speed;
    int lifes;
    int score;
} Player;

typedef struct Shot { //Sets up the type shot
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Shot;

typedef struct Asteroid { //Sets up the type asteroid
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Asteroid;

float logoTimer = 0.0f; //To keep track on how long the logo has been showing
const float LOGO_TIME = 2.0f; //How long the logo should show for
Texture2D gamebg; //Initialises the background for the game

bool MyCheckCollisionCircles(Vector2 pos1, float radius1, Vector2 pos2, float radius2) //I wanted to try and make my own collision detection (It worked!)
{
    float dx = pos1.x - pos2.x; //The difference between the two x positions
    float dy = pos1.y - pos2.y; //The difference between the two y positions
    float distance = sqrtf(dx * dx + dy * dy); //Checks the distance between the centre of both circles
    return distance < (radius1 + radius2);
}

// Initialize game function
void InitGame(Player *player, Asteroid *asteroid, Shot *shot, Texture2D *ship) 
{
    *ship = LoadTexture("SpaceShip.png"); //Loads the png's
    gamebg = LoadTexture("BackGround.png");
    
    player->position = (Vector2){SCREENWIDTH/2, SCREENHEIGHT - 100}; //Sets all the values for the structures defined at the top
    player->speed = (Vector2){8.0f, 8.0f};
    player->lifes = PLAYER_LIFES;
    player->score = SCORE;

    asteroid->active = false;
    asteroid->radius = 30.0f;
    asteroid->speed.y = 2.0f;

    shot->active = false;
    shot->radius = 9.0f;
    
}

// Update asteroid function 
void UpdateAsteroid(Asteroid *asteroid) {
    if (!asteroid->active) 
    {
        if (rand() % 100 < 2) // 2% chance to spawn every second
        { 
            asteroid->active = true;
            asteroid->position.x = (float)(rand() % SCREENWIDTH);
            asteroid->position.y = -asteroid->radius; //Makes it spawn above screen
            asteroid->speed.y = 2.0f + (rand() % 3); //Sets the speed to 2-4
        }
    } else {
        asteroid->position.y += asteroid->speed.y; //Moves the asteroid
        if (asteroid->position.y > SCREENHEIGHT + asteroid->radius) //Checks if the asteroid goes off the bottom of the screen
            asteroid->active = false;
    }
}

//Update game logic function
void UpdateGame(Player *player, Asteroid *asteroid, Shot *shot, GameScreen *screen, bool *gamePaused) {
    if (*screen == GAMEPLAY && !(*gamePaused)) 
    {
        //Player movement
        if(IsKeyDown(KEY_A))
        {
            player->position.x -= player->speed.x;
        }
        if(IsKeyDown(KEY_D))
        {
            player->position.x += player->speed.x;
        }
        if(IsKeyDown(KEY_W))
        {
            player->position.y -= player->speed.y;
        }
        if(IsKeyDown(KEY_S))
        {
            player->position.y += player->speed.y;
        }
        
        //Collision with the screen edge (not perfect)
        if(player->position.x <= 0)
        {
            player->position.x = 10;
        }
        if(player->position.x >= SCREENWIDTH - 20)
        {
            player->position.x = SCREENWIDTH - 20;
        }
        if(player->position.y <= 0)
        {
            player->position.y = 0;
        }
        if(player->position.y >= SCREENHEIGHT - 20)
        {
            player->position.y = SCREENHEIGHT - 20;
        }

        //Shooting
        if (IsKeyPressed(KEY_SPACE) && !shot->active) 
        {
            shot->active = true;
            shot->position = player->position;
            shot->speed.y = 10.0f;
        }

        if (shot->active) 
        {
            shot->position.y -= shot->speed.y; //Moves the shot upwards
            if (shot->position.y < 0) //Checks if the shot is off the top of the screen
            {                
                shot->active = false;
            }
        }

        //Calling the UpdateAsteroid function to update the asteroid
        UpdateAsteroid(asteroid);

        //Collision detection
        if (asteroid->active) {
            if (shot->active && MyCheckCollisionCircles(shot->position, shot->radius, asteroid->position, asteroid->radius)) //Checks if the shot collides with the asteroid
            {
                shot->active = false;
                asteroid->active = false;
                player->score += 10;
            }

            if (MyCheckCollisionCircles(player->position, 25.0f, asteroid->position, asteroid->radius)) //Checks if the player collides with the asteroid
            { 
                asteroid->active = false;
                player->lifes -= 1;
                if (player->lifes <= 0) //If player has no more lifes the game ends
                    *screen = ENDING;
            }
        }
    }
}

//Draw asteroid function
void DrawAsteroid(Asteroid asteroid) 
{
    if (asteroid.active)
        DrawCircle(asteroid.position.x, asteroid.position.y, asteroid.radius, GRAY);
}

//Draw function
void DrawGame(Player player, Asteroid asteroid, Shot shot, Texture2D ship, GameScreen screen) {
    if (screen == GAMEPLAY) 
    {
        DrawText(TextFormat("Score: %d", player.score), 20, 60, 30, LIGHTGRAY);
        DrawText(TextFormat("Lives: %d", player.lifes), 20, 20, 30, GREEN);

        //Draws player
        float scale = 5.0f;
        DrawTexturePro(
            ship,
            (Rectangle){0, 0, ship.width, ship.height},
            (Rectangle){player.position.x, player.position.y, ship.width * scale, ship.height * scale},
            (Vector2){(ship.width * scale)/2, (ship.height * scale)/2},
            0.0f,
            GREEN
        );

        //Draws shot
        if (shot.active)
            DrawCircle(shot.position.x, shot.position.y, shot.radius, GREEN);

        //Draws asteroid
        DrawAsteroid(asteroid);
    }
    else if (screen == ENDING) 
    {
        DrawText("GAME OVER", SCREENWIDTH/2 - 150, SCREENHEIGHT/2 - 20, 60, RED);
        DrawText(TextFormat("Final Score: %d", player.score), SCREENWIDTH/2 - 150, SCREENHEIGHT/2 + 50, 40, LIGHTGRAY);
    }
}

// Unload resources
void UnloadGame(Texture2D ship, Texture2D gamebg) 
{
    UnloadTexture(ship); //unloads png to stop memory leaks 
    UnloadTexture(gamebg);
}

void UpdateGameScreen(GameScreen *screen, float *logoTimer, Player *player)
{
    switch (*screen) //Check which screen is being displayed and does the following 
    {
        case LOGO:
            *logoTimer += 1.0f / FPS;
            if (*logoTimer >= LOGO_TIME) 
            {
                *screen = TITLE;
            }
            break;

        case TITLE:
            if (IsKeyPressed(KEY_ENTER)) //Switches to gameplay screen if enter is pressed
            {
                *screen = GAMEPLAY;
            }
            player->score = 0; //This resets the players score so when starting the game it is 0 and when replaying the game it is also 0
            player->lifes = PLAYER_LIFES;
            player->position = (Vector2){SCREENWIDTH/2, SCREENHEIGHT - 100};
            break;

        case GAMEPLAY:
            if(IsKeyPressed(KEY_ENTER)) //Switches to the pause screen if enter is pressed
            {
                *screen = PAUSED;
            }

            break;
        case PAUSED:
            if(IsKeyPressed(KEY_ENTER)) //Switches back to the gameplay screen if enter is pressed
            {
                *screen = GAMEPLAY;
            }
            break;

        case ENDING:
            if (IsKeyPressed(KEY_ENTER)) //Restarts the game if enter is pressed
            {
                *screen = TITLE; 
            }
            if(IsKeyPressed(KEY_SPACE))
            {
                Unloadgame(ship, gamebg);//Unloads the png's used
                CloseWindow(); //Closes the window
            }
            break;

        default:
            break;
    }
}

void DrawScreen(GameScreen screen, Player player, Asteroid asteroid, Shot shot, Texture2D ship) //Draws all the screens and what is meant to be displayed on each screen
{
    switch(screen)
    {
        case LOGO:
            ClearBackground(BLACK);
            DrawText("Christopher Sinclair Game Project Praxis Programming", SCREENWIDTH/2 - 450, SCREENHEIGHT/2 , 35, RAYWHITE); //Draws text onto the screen
            break;

        case TITLE:
            ClearBackground(BLACK);
            DrawText("STARSHIP SHOOTER", SCREENWIDTH/2 - 300, SCREENHEIGHT/2 - 50, 60, RAYWHITE);
            DrawText("Press ENTER to Start", SCREENWIDTH/2 - 300, SCREENHEIGHT/2 + 50, 30, LIGHTGRAY);
            break;

        case GAMEPLAY:
            DrawTexturePro( //Draws the game background with scaling so it fits the window
                gamebg,
                (Rectangle){0, 0, gamebg.width, gamebg.height},
                (Rectangle){0, 0, SCREENWIDTH, SCREENHEIGHT},
                (Vector2){0, 0},
                0.0f,
                WHITE
                ); 
            DrawGame(player, asteroid, shot, ship, screen); //Draws everything 
            break;
            
        case PAUSED:
            ClearBackground(RED);
            DrawText("PAUSED", SCREENWIDTH/2 - 120, SCREENHEIGHT/2 - 30, 60, BLACK); //Draws the text for the pause screen
            DrawText("Press Enter To Resume", SCREENWIDTH/2 - 240, SCREENHEIGHT/2 + 80, 40, BLACK);
            break;

        case ENDING:
            ClearBackground(BLACK);
            DrawText("GAME OVER", SCREENWIDTH/2 - 180, SCREENHEIGHT/2 - 60, 60, RED); //Draws the game over screen
            DrawText(TextFormat("Final Score: %d", player.score), SCREENWIDTH/2 - 180, SCREENHEIGHT/2 + 50, 40, LIGHTGRAY);
            DrawText("Press ENTER to go to Title", SCREENWIDTH/2 - 180, SCREENHEIGHT/2 + 100, 25, LIGHTGRAY);
            DrawText("Press SPACE to exit", SCREENWIDTH/2 - 180, SCREENHEIGHT/2 + 140, 25, LIGHTGRAY);
            break;

        default:
            break;
    }
}
//Main section which runs everything
int main()
{ 
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Starship"); //Initialise the window
    SetTargetFPS(FPS); //Sets the Frames per Second
    
    //Making local variables
    Player player;
    Asteroid asteroid;
    Shot shot;
    Texture2D ship;
    GameScreen screen = LOGO;
    bool gamePaused = false;

    //Initialize the game 
    InitGame(&player, &asteroid, &shot, &ship);

    while (!WindowShouldClose()) //Runs while the window is open
    {
        
        UpdateGameScreen(&screen, &logoTimer, &player);
        
        if(screen == GAMEPLAY)
        {
            UpdateGame(&player, &asteroid, &shot, &screen, &gamePaused);
        }

        BeginDrawing(); 
        ClearBackground(RAYWHITE); 
        DrawScreen(screen, player, asteroid, shot, ship);
        EndDrawing();
    }
    
    return 0;
}