#include "score_handler.h"
#include "game.h"
#include "asset_loader.h"
#include "renderer.h"
#include "camera.h"
#include "raymath.h"

int medalsUnlocked = 0;
int medalsDisplayed = 0;
double medalGotTime[MEDAL_COUNT];
Vector2 medalTexSize = {15, 32};
Vector2 medalOffset = {4, 4};

const int medalScores[MEDAL_COUNT] = {M_BRONZE, M_SILVER, M_GOLD, M_PLATINUM};

void ScoreHandlerUnlockMedals(int score)
{
    medalsUnlocked = 0;
    medalsDisplayed = 0;

    for (int i = 1; i <= MEDAL_COUNT; i++)
    {
        if (score >= medalScores[i-1]) medalsUnlocked++;
    }
}

void ScoreHandlerRevealMedals()
{
    if (medalsDisplayed >= medalsUnlocked || frameCounter % 20 != 0) return;

    medalsDisplayed++;
    medalGotTime[(int)fmax(0, medalsDisplayed-1)] = GetTime();
    CameraSetShake(2, 1.f, .2f);
    PlaySound(gameAudio.medalGot);
    if (medalsDisplayed == 4) PlaySound(gameAudio.postBallClarity);
}

void ScoreHandlerDrawMedals()
{
    // Backdrop
    for (int i = 1; i <= MEDAL_COUNT; i++)
    {

        Rectangle source = {0, 0, medalTexSize.x, medalTexSize.y};
        float halfTexW = roundf(medalTexSize.x / 2);
        Rectangle dest = {
                VIRTUAL_SCREEN_WIDTH - (halfTexW + medalOffset.x + (float)(i - 1) * (medalTexSize.x + medalOffset.x)),
                medalOffset.y,
                medalTexSize.x,
                medalTexSize.y};
        Vector2 origin = {halfTexW, 0};

        if (i == MEDAL_COUNT && medalsUnlocked < MEDAL_COUNT - 1) break; // Only reveal the last medal when you're one away

        DrawTexturePro(gameTextures.medals, source, dest, origin, 0, GRAY);
    }

    // Medal
    for (int i = 1; i <= medalsDisplayed; i++)
    {
        Rectangle source = {(float)gameTextures.medals.width - (float)i * medalTexSize.x,
                            0,
                            medalTexSize.x,
                            medalTexSize.y};

        float halfTexW = roundf(medalTexSize.x / 2);
        Rectangle dest = {
                VIRTUAL_SCREEN_WIDTH - (halfTexW + medalOffset.x + (float)(i - 1) * (medalTexSize.x + medalOffset.x)),
                medalOffset.y,
                medalTexSize.x,
                medalTexSize.y};

        Vector2 origin = {halfTexW, 0};

        // Chad version
        // float rotation = (float)sin((medalGotTime[i] / GetTime()) * 20) * (float)((medalGotTime[i] / (GetTime() * 10)) * 100);

        // Nerd version
        float elapsedTime = GetTime() - medalGotTime[i-1];
        float decayRate = 0.2f;
        float initialAmplitude = 40.0f;
        float frequency = 4.0f;

        float damping = exp(-decayRate * elapsedTime);
        float rotation = initialAmplitude * damping * sin(frequency * elapsedTime);

        DrawTexturePro(gameTextures.medals, source, dest, origin, rotation, WHITE);
    }
}

bool ScoreHandlerMedalRevealFinished()
{
    if (medalsUnlocked == 0) return true;
    if (medalsDisplayed >= medalsUnlocked)
    {
        if (GetTime() > medalGotTime[(int)fmax(0, medalsDisplayed-1)] + 2) return true;
    }

    return false;
}