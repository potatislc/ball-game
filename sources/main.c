#include "raylib.h"
#include "ball.h"
#include "player.h"
#include "global.h"

#define WINDOW_TITLE "Ball Game"

#define NBR_OF_BALLS (2)

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

    Vector2 vScreenCenter = {VIRTUAL_SCREEN_WIDTH / 2.0f, VIRTUAL_SCREEN_HEIGHT / 2.0f };

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);

    Camera2D worldSpaceCamera = { 0 };
    worldSpaceCamera.zoom = 1.0f;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = {-screenRatio, -screenRatio, SCREEN_WIDTH + (screenRatio * 2), SCREEN_HEIGHT + (screenRatio * 2) };

    Vector2 origin = { 0.0f, 0.0f };

    Texture2D floorTexture = LoadTexture("../assets/floor.png");

    Player player;
    init_player(&player, vScreenCenter);

    Ball balls[NBR_OF_BALLS];

    // ToggleFullscreen(); -- Wtfff

    for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
    {
        float testRadius = 32.0f;
        init_ball(&balls[i],
                  (Vector2) {testRadius + testRadius * (float) i * 2.0f, testRadius + testRadius * (float) i * 2.0f},
                  testRadius);
    }

    bool pause = 0;

    SetTargetFPS(60);
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------
        // if (IsKeyPressed(KEY_SPACE)); // Spawn Ball

        if (!freezeBalls)
        {
            for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
            {
                ball_collision_ball(&balls[i], balls, NBR_OF_BALLS);
                move_ball(&balls[i]);
                screen_collision_ball(&balls[i]);
            }
        }

        update_player(&player, balls, NBR_OF_BALLS);
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginTextureMode(target);
            ClearBackground((Color){ 121, 121, 121, 255});

            // Draw Game World
            BeginMode2D(worldSpaceCamera);
                // Draw floor
                DrawTexture(floorTexture, 0, 0, WHITE);

                // Draw Shadows
                draw_player_shadow(player);

                for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
                {
                    draw_shadow_ball(balls[i]);
                }

                // Draw black header
                //DrawRectangle(0, 0, VIRTUAL_SCREEN_WIDTH, 16, BLACK);

                // Draw characters
                for (int i = 0; i < sizeof(balls) / sizeof(balls[0]); i++)
                {
                    draw_ball(balls[i]);
                }

                draw_player(player);
            EndMode2D();
        EndTextureMode();

        // Draw to Screen
        BeginDrawing();
            ClearBackground(DARKGRAY);

            // Draw world camera to screen
            DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);

            DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------

        // Toggle Fullscreen
        //-----------------------------------------------------
        // check for alt + enter
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
        {
            // see what display we are on right now
            int display = GetCurrentMonitor();

            if (IsWindowFullscreen())
            {
                // if we are full screen, then go back to the windowed size
                SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
            }
            else
            {
                // if we are not full screen, set the window size to match the monitor we are on
                SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            }

            // toggle the state
            ToggleFullscreen();
        }
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //----------------------------------------------------------

    return 0;
}
