#include "ball.h"
#include <math.h>

// #define sign(a) ((a > 0) ? 1 : ((a < 0) ? -1 : 0))
#define sign(a) ((a > 0) ? 1 : -1)

void setPosBall(struct Ball *ball, float x, float y)
{
    ball->position = (Vector2){x, y};
    ball->collisionBox.x = x - ball->radius;
    ball->collisionBox.y = y - ball->radius;
}

void moveBall(struct Ball *ball)
{
    setPosBall(ball, ball->position.x + ball->speed.x, ball->position.y + ball->speed.y);
}

void screenCollisionBall(struct Ball *ball)
{
    if ((ball->position.x >= ((float)GetScreenWidth() - ball->radius)) || (ball->position.x <= ball->radius)) ball->speed.x *= -1.0f;
    if ((ball->position.y >= ((float)GetScreenHeight() - ball->radius)) || (ball->position.y <= ball->radius)) ball->speed.y *= -1.0f;
    ball->position.x = fmaxf(ball->radius, fminf(ball->position.x, (float)GetScreenWidth() - ball->radius));
    ball->position.y = fmaxf(ball->radius, fminf(ball->position.y, (float)GetScreenHeight() - ball->radius));
}

void ballCollisionBall(struct Ball *ball, struct Ball balls[], int nbrOfBalls)
{
    for (int i = 0; i < nbrOfBalls; i++)
    {
        if (&balls[i] == ball)
        {
            continue;
        }

        Vector2 dist =
                {
                    ball->position.x - balls[i].position.x,
                    ball->position.y - balls[i].position.y
                };

        Vector2 signDist =
                {
                        sign(dist.x),
                        sign(dist.y)
                };

        while (CheckCollisionRecs(ball->collisionBox, balls[i].collisionBox))
        {
            setPosBall(ball, ball->position.x + signDist.x, ball->position.y + signDist.y);
            setPosBall(&balls[i], balls[i].position.x - signDist.x, balls[i].position.y - signDist.y);

            if (fabsf(dist.x) > fabsf(dist.y))
            {
                ball->speed.x = signDist.x * fabsf(ball->speed.x);
                balls[i].speed.x = -signDist.x * fabsf(ball->speed.x);
            }
            else
            {
                ball->speed.y = signDist.y * fabsf(ball->speed.y);
                balls[i].speed.y = -signDist.y * fabsf(ball->speed.y);
            }

            ball->colliding = true;
            return;
        }
    }

    ball->colliding = false;
}

void drawBall(struct Ball ball, Vector2 shadowOffset, float shadowRadius)
{
    Vector2 ballShadowPos = {ball.position.x + shadowOffset.x, ball.position.y + shadowOffset.y };
    DrawCircleV(ballShadowPos, ball.radius - shadowRadius, BLACK);
    DrawCircleV(ball.position, ball.radius, (ball.colliding) ? MAROON : GREEN);
}