#ifndef MY_RAYLIB_GAME_BALL_H
#define MY_RAYLIB_GAME_BALL_H

#include "raylib.h"

struct Ball
{
    Vector2 position;
    Vector2 speed;
    float radius;
    bool colliding;
    Rectangle collisionBox;
};

void setPosBall(struct Ball *ball, float x, float y);
void moveBall(struct Ball *ball);
void screenCollisionBall(struct Ball *ball);
void ballCollisionBall(struct Ball *ball, struct Ball balls[], int nbrOfBalls);
void drawBall(struct Ball ball, Vector2 shadowOffset, float shadowRadius);

#endif //MY_RAYLIB_GAME_BALL_H
