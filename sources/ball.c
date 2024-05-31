#include "ball.h"
#include <math.h>
#include "global.h"
#include "raymath.h"
#include "asset_loader.h"

#define sign(a) ((a > 0) ? 1 : -1)

ListNode *ballHead = NULL;
ListNode *ballSpawnPointHead = NULL;
float ballSpeed = .5f;
bool freezeBalls = false;
const double ballSpawnTime = .8;

static Vector2 RandomPosition(Vector2 minPos, Vector2 maxPos)
{
    if (minPos.x == maxPos.x && minPos.y == maxPos.y) return minPos;

    Vector2 spawnAreaSize = Vector2Subtract(maxPos, minPos);
    return Vector2Round((Vector2){minPos.x + ((float)rand()/(float)(RAND_MAX)) * spawnAreaSize.x,
                     minPos.y + ((float)rand()/(float)(RAND_MAX)) * spawnAreaSize.y});
}

static Vector2 RandomDirection()
{
    // num = (rand() % (upper – lower + 1)) + lower
    // srand(time(0));

    int randDir = rand() % 4;

    Vector2 directionVector = Vector2Zero();
    switch (randDir)
    {
        case 0:
            directionVector = (Vector2){1.0f, 1.0f};
            break;
        case 1:
            directionVector = (Vector2){-1.0f, 1.0f};
            break;
        case 2:
            directionVector = (Vector2){-1.0f, -1.0f};
            break;
        case 3:
            directionVector = (Vector2){1.0f, -1.0f};
            break;
    }

    return directionVector;
}

Ball *BallInit( Vector2 minInitPos, Vector2 maxInitPos, float radius)
{
    Ball *ball = (Ball *)malloc(sizeof(Ball));

    ball->stateExecute = STATE_EXECUTE_BALL_MOVE;
    ball->speed = ballSpeed;
    ball->velocity = RandomDirection();
    ball->velocity = (Vector2){ball->velocity.x * ball->speed, ball->velocity.y * ball->speed};

    ball->radius = radius;
    ball->shadowRadius = radius;
    ball->shadowOffset = (Vector2){ 4.0f, 4.0f };

    ball->colliding = false;
    float colliderScale = .8f;
    ball->collisionBox = (Rectangle){ 0, 0, ball->radius * 2 * colliderScale, ball->radius * 2 * colliderScale};

    BallSetPosition(ball, RandomPosition(minInitPos, maxInitPos));

    ball->texture = (radius > BALL_TOO_SMALL_FOR_CLEAN_SPLIT) ? &melonBig : &melonSmall;
    ball->textureScale = (ball->radius * 2) / (float)ball->texture->width;
    ball->textureOffset = (Vector2){ ((float)ball->texture->width / 2.0f) * ball->textureScale,
                                     ((float)ball->texture->height / 2.0f) * ball->textureScale };

    return ball;
}

void BallUpdate(Ball *ball)
{
    BallCollisionBall(ball);
    ball->stateExecute(ball);
    BallCollisionScreen(ball);
}

void BallSetPosition(Ball *ball, Vector2 pos)
{
    ball->position = pos;
    ball->collisionBox.x = pos.x - ball->radius;
    ball->collisionBox.y = pos.y - ball->radius;
}

void BallStateMove(Ball *ball)
{
    BallSetPosition(ball, (Vector2) {ball->position.x + ball->velocity.x, ball->position.y + ball->velocity.y});
}

void BallCollisionScreen(Ball *ball)
{
    if ((ball->position.x >= (VIRTUAL_SCREEN_WIDTH - ball->radius)) || (ball->position.x <= ball->radius)) ball->velocity.x *= -1.0f;
    if ((ball->position.y >= (VIRTUAL_SCREEN_HEIGHT - ball->radius)) || (ball->position.y <= ball->radius)) ball->velocity.y *= -1.0f;
    ball->position.x = fmaxf(ball->radius, fminf(ball->position.x, VIRTUAL_SCREEN_WIDTH - ball->radius));
    ball->position.y = fmaxf(ball->radius, fminf(ball->position.y, VIRTUAL_SCREEN_HEIGHT - ball->radius));
}

void BallCollisionBall(Ball *ball)
{
    ListNode *currentBallNode = ballHead;

    while(currentBallNode->next != NULL)
    {
        Ball *currentBall = (Ball*)currentBallNode->data;
        currentBallNode = currentBallNode->next;

        if (ball == (Ball*)currentBall)
        {
            continue;
        }

        Vector2 dist =
                {
                        ball->position.x - currentBall->position.x,
                        ball->position.y - currentBall->position.y
                };
        Vector2 signDist =
                {
                        sign(dist.x),
                        sign(dist.y)
                };

        if (CheckCollisionRecs(ball->collisionBox, currentBall->collisionBox))
        {
            BallSetPosition(ball, (Vector2) {ball->position.x + signDist.x, ball->position.y + signDist.y});
            BallSetPosition(currentBall, (Vector2) {currentBall->position.x - signDist.x, currentBall->position.y - signDist.y});

            if (fabsf(dist.x) > fabsf(dist.y))
            {
                ball->velocity.x = signDist.x * fabsf(ball->velocity.x);
                currentBall->velocity.x = -signDist.x * fabsf(ball->velocity.x);
            }
            else
            {
                ball->velocity.y = signDist.y * fabsf(ball->velocity.y);
                currentBall->velocity.y = -signDist.y * fabsf(ball->velocity.y);
            }

            ball->colliding = true;
            return;
        }
    }

    ball->colliding = false;
}

void BallSplit(Ball *ball, Vector2 splitDir)
{
    float newRadius = ball->radius/2;
    if (newRadius > MIN_BALL_RADIUS)
    {
        float spawnDirRad = atan2f(splitDir.y, splitDir.x) + PI / 2;
        if (spawnDirRad > PI * 2) spawnDirRad -= PI * 2;

        Vector2 spawnPos = {cosf(spawnDirRad) * newRadius / 2, sinf(spawnDirRad) * newRadius / 2};

        Vector2 spawnPosRight = Vector2Add(ball->position, spawnPos);
        Ball *ballRight = BallInit(spawnPosRight, spawnPosRight, newRadius);
        ListNodePush(&ballHead, ballRight);

        Vector2 spawnPosLeft = Vector2Subtract(ball->position, spawnPos);
        Ball *ballLeft = BallInit(spawnPosLeft, spawnPosLeft, newRadius);
        ListNodePush(&ballHead, ballLeft);
    }

    AddToScore(1); // Temp
    ListNodeRemove(&ballHead, ball);
}

void BallDraw(Ball ball)
{
    Vector2 drawPos = {ball.position.x - ball.textureOffset.x, ball.position.y - ball.textureOffset.y};
    DrawTextureEx(*ball.texture, Vector2Round(drawPos), 0, ball.textureScale, WHITE);
}

void BallDrawShadow(Ball ball)
{
    Vector2 ballShadowPos = {ball.position.x + ball.shadowOffset.x, ball.position.y + ball.shadowOffset.y };
    DrawCircleV(Vector2Round(ballShadowPos), ball.shadowRadius, shadowColor);
}

// Ball Spawn Point ----------
BallSpawnPoint *BallSpawnPointInit(Ball *mockBall, double spawnTime)
{
    BallSpawnPoint *instance = (BallSpawnPoint*)malloc(sizeof(BallSpawnPoint));
    instance->mockBall = mockBall;
    instance->initTime = GetTime();
    instance->spawnTime = spawnTime;

    return instance;
}

static double TimeSinceInit(double initTime)
{
    return GetTime() - initTime;
}

void BallSpawnPointUpdate(BallSpawnPoint *ballSpawnPoint)
{
    if (TimeSinceInit(ballSpawnPoint->initTime) < ballSpawnPoint->spawnTime) return;

    ListNodePush(&ballHead, ballSpawnPoint->mockBall);
    ListNodeRemove(&ballSpawnPointHead, ballSpawnPoint);
}

void BallSpawnPointDraw(BallSpawnPoint ballSpawnPoint)
{
    double timeSinceInit = TimeSinceInit(ballSpawnPoint.initTime);

    // Outer Circle
    double outerAnimTime = ballSpawnPoint.spawnTime * 1.f; // Maybe change this value?
    float outerRadius = (float)(fmin(timeSinceInit, outerAnimTime) / outerAnimTime);
    Color outerColor = {255, 255, 255, outerRadius * 255};
    DrawCircleLinesV(ballSpawnPoint.mockBall->position, (1.f / outerRadius) * ballSpawnPoint.mockBall->radius, outerColor);

    // Inner Circle
    float innerRadius = (float)(timeSinceInit / ballSpawnPoint.spawnTime) * ballSpawnPoint.mockBall->radius;
    DrawCircleLinesV(ballSpawnPoint.mockBall->position, innerRadius, uiColorYellow);
}