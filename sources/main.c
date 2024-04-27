#include "raylib.h"
#include "ball.h"
#include "player.h"

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (1280)
#define VIRTUAL_SCREEN_WIDTH (256)
#define VIRTUAL_SCREEN_HEIGHT (256)

#define WINDOW_TITLE "Ball Game"

#define NBR_OF_BALLS (6)

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

    Vector2 vScreenCenter = {VIRTUAL_SCREEN_WIDTH / 2.0f, VIRTUAL_SCREEN_HEIGHT / 2.0f };

    const float virtualRatio = (float)SCREEN_WIDTH/(float)VIRTUAL_SCREEN_WIDTH;

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);

    Camera2D worldSpaceCamera = { 0 };
    worldSpaceCamera.zoom = 1.0f;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = { -virtualRatio, -virtualRatio, SCREEN_WIDTH + (virtualRatio*2), SCREEN_HEIGHT + (virtualRatio*2) };

    Vector2 origin = { 0.0f, 0.0f };

    struct Player player;
    initPlayer(&player, vScreenCenter);

    struct Ball balls[NBR_OF_BALLS];

    for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
    {
        float testRadius = 32.0f;
        initBall(&balls[i],(Vector2){ testRadius + testRadius * (float)i * 2.0f, testRadius + testRadius * (float)i * 2.0f },testRadius);
    }

    Vector2 shadowOffset = { 4.0f, 4.0f};
    float shadowRadius = 1;

    bool pause = 0;
    int framesCounter = 0;

    SetTargetFPS(60);
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;

        if (!pause)
        {
            for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
            {
                ballCollisionBall(&balls[i], balls, NBR_OF_BALLS);
                moveBall(&balls[i]);
                screenCollisionBall(&balls[i]);
            }

            updatePlayer(&player);
        }
        else framesCounter++;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginTextureMode(target);
            ClearBackground(DARKGRAY);

            // Draw Game World
            BeginMode2D(worldSpaceCamera);
                for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
                {
                    drawBall(balls[i], shadowOffset, shadowRadius);
                }

                drawPlayer(player);
            EndMode2D();
        EndTextureMode();

        // Draw to Screen
        BeginDrawing();
            ClearBackground(DARKGRAY);

            // Draw world camera to screen
            DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);

            //DrawText("PRESS SPACE to PAUSE BALL MOVEMENT", 10, GetScreenHeight() - 25, 20, LIGHTGRAY);

            // On pause, we draw a blinking message
            if (pause && ((framesCounter/30)%2)) DrawText("PAUSED", 350, 200, 30, GRAY);

            DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}
