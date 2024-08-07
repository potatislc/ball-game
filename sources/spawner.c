#include "spawner.h"
#include "raylib.h"
#include "raymath.h"
#include "renderer.h"
#include "player.h"
#include "ball.h"
#include "global.h"
#include "asset_loader.h"
#include "game.h"
#include "score_handler.h"

double spawnDelay = BALL_SPAWN_DELAY_FIRST;
double timeSinceLastSpawn;
ListNode *spawnQueueHead = NULL;

int secondBallLimit = 2;
int thirdBallLimit = 3;
int fourthBallLimit = 0;

int wavesSinceLastOrange = 0;

void SpawnerPlaceBallSpawnPoint(float radius, bool avoidPlayer, int type)
{
    const Vector2 minInitPos = {radius, radius};
    const Vector2 maxInitPos = {VIRTUAL_SCREEN_WIDTH - radius, VIRTUAL_SCREEN_HEIGHT - radius};
    Ball *newBall = BallInit(
            Vector2RandomRange(minInitPos, maxInitPos),
            radius, type);

    if (avoidPlayer)
    {
        float distanceToPlayer = Vector2Distance(playerRef->position, newBall->position);
        const float collisionMargin = radius + playerRef->radius;
        // Theoretically, in an alternate universe, the game could freeze here for a while
        while (distanceToPlayer <= collisionMargin)
        {
            BallSetPosition(newBall, Vector2RandomRange(minInitPos, maxInitPos));
            distanceToPlayer = Vector2Distance(playerRef->position, newBall->position);
        }
    }

    double spawnTime = (gameState == GAME_PLAY) ? gameplaySpawnTime : tutorialSpawnTime;
    BallSpawnPoint *newBallSpawn = BallSpawnPointInit(newBall, spawnTime);
    ListNodePush(&ballSpawnPointHead, newBallSpawn);

    Sound spawnSfx = (newBall->type == TYPE_MELON) ? gameAudio.ballSpawn : gameAudio.orangeSpawn;
    SoundPanToWorld(spawnSfx, newBall->position, DEFAULT_SOUND_PAN_INTENSITY);
    if(IsBallClearingFinished()) PlaySound(spawnSfx);
}

void SpawnFromQueue()
{
    if (spawnQueueHead == NULL) return;
    SpawnData *headData = (SpawnData*)spawnQueueHead->data;
    SpawnerPlaceBallSpawnPoint(headData->radius, headData->avoidPlayer, headData->type);
    ListNodeRemove(&spawnQueueHead, spawnQueueHead->data);
}

void AddBallToQueue(float radius, bool avoidPlayer, int type)
{
    timeSinceLastSpawn = GetTime();

    SpawnData *spawnData = (SpawnData*)malloc(sizeof(SpawnData));
    spawnData->radius = radius;
    spawnData->avoidPlayer = avoidPlayer;
    spawnData->type = type;

    ListNodePush(&spawnQueueHead, spawnData);
}

void UpdateBallLimits()
{
    if (ballNbrCount_All.spawned > 60)
    {
        if (ballNbrCount_All.spawned % 20 == 0 || ballNbrCount_All.spawned % 30 == 0) secondBallLimit++;

        if (ballNbrCount_All.spawned % 30 == 0) thirdBallLimit++;
    }

    if (ballNbrCount_All.spawned > 170)
    {
        if (ballNbrCount_All.spawned % 30 == 0) fourthBallLimit++;
    }
}

void AddBallPreset(unsigned int choice)
{
    switch (choice) {
        default:
            AddBallToQueue(RADIUS_LARGE, false, TYPE_MELON);
            break;
        case 1:
            AddBallToQueue(RADIUS_MEDIUM, false, TYPE_MELON);
            AddBallToQueue(RADIUS_MEDIUM, false, TYPE_MELON);
            break;
        case 2:
            AddBallToQueue(RADIUS_SMALL, false, TYPE_MELON);
            AddBallToQueue(RADIUS_SMALL, false, TYPE_MELON);
            AddBallToQueue(RADIUS_SMALL, false, TYPE_MELON);
            AddBallToQueue(RADIUS_SMALL, false, TYPE_MELON);
            break;
    }
}

void AddBallsToQueue()
{
    UpdateBallLimits();

    if (GetTime() < timeSinceLastSpawn+spawnDelay) return;

    if (ScoreHandlerGetScore() + ScoreHandlerGetComboScore() * ScoreHandlerGetComboMultiplier() > 3000)
    {
        secondBallLimit = 1000;
        thirdBallLimit = 1000;
        fourthBallLimit = 1000;
    }

    // Spawn Wave -----------------

    wavesSinceLastOrange++;

    if (ballNbrCount_All.spawned == 0)
    {
        AddBallToQueue(RADIUS_LARGE, true, TYPE_MELON);
        spawnDelay = BALL_SPAWN_DELAY_LONG;
    }

    if (ballNbrCount_All.destroyed > 6)
    {
        AddBallPreset((NbrOfRadiusBallsOnScreen(RADIUS_LARGE) < 4) ? 0 : 1 + (rand() % 2));
    }

    if (ballNbrCount_All.destroyed > 20 && spawnDelay == BALL_SPAWN_DELAY_LONG)
    {
        if (NbrOfBallsOnScreen(ballNbrCount_All) <= secondBallLimit)
        {
            AddBallPreset((NbrOfRadiusBallsOnScreen(RADIUS_LARGE) < 4) ? 0 : rand() % 2);

            if (NbrOfBallsOnScreen(ballNbrCount_All) <= thirdBallLimit && ballNbrCount_All.destroyed > 50)
            {
                AddBallPreset(rand() % 3);
            }

            if (NbrOfBallsOnScreen(ballNbrCount_All) <= fourthBallLimit && ballNbrCount_All.destroyed > 70)
            {
                AddBallToQueue(RADIUS_LARGE, false, TYPE_MELON);
            }
        }
    }

    if (NbrOfBallsOnScreen(ballNbrCount_All) > 5 && wavesSinceLastOrange > 1)
    {
        AddBallToQueue(RADIUS_MEDIUM, false, TYPE_ORANGE);
        wavesSinceLastOrange = 0;
    }
}

void SpawnerInit()
{
    timeSinceLastSpawn = GetTime();
    ballNbrCount_All.spawned = 0;
    ballNbrCount_All.destroyed = 0;
}

void SpawnerUpdate()
{
    if (GetTime() > timeSinceLastSpawn+spawnDelay)
    {
        AddBallsToQueue();
    }
}