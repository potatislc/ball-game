#include "player.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include "global.h"

#define sign(a) ((a > 0) ? 1 : -1)

void PlayerInit(Player *player, Vector2 initPos)
{
    player->state = PLAYER_MOVING;
    player->texture = LoadTexture("../assets/samurai/samurai.png");
    player->shadowTexture = LoadTexture("../assets/samurai/samurai_shadow.png");
    player->position = initPos;
    player->speed = 2;
    player->radius = 6;

    player->dash = (Dash *)malloc(sizeof(Dash));
    if (player->dash != NULL) {
        // Initialize the allocated memory
        *player->dash = ( Dash){0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 32.0f, 10, 30, 30, 0};
    } else {
        // Handle failure to allocate memory IDK :/
    }

    // Allocate memory for whatever ball that goes here
    player->collidingBall = (Ball *)malloc(sizeof(Ball));
    player->collidingBallCopy = (Ball *)malloc(sizeof(Ball));
}

void PlayerUpdate(Player *player, ListNode *ballHead)
{
    switch(player->state)
    {
        case PLAYER_IDLE:
            break;
        case PLAYER_MOVING:
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                PlayerMoveToPoint(player, to_virtual_coords_vector2(GetMousePosition()));
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->dash->reloadTime == 0)
            {
                PlayerBeginDash(player, to_virtual_coords_vector2(GetMousePosition()));
            }
            break;
        case PLAYER_DASHING:
            PlayerDash(player);
            break;
        case PLAYER_SLICING:
            PlayerSlice(player);
            break;
    }

    PlayerCollisionScreen(player);
    PlayerCollisionBall(player, ballHead);
    if (player->dash->reloadTime > 0) player->dash->reloadTime--;
}

void PlayerMoveToPoint(Player *player, Vector2 point)
{
    Vector2 dist = {point.x - player->position.x, point.y - player->position.y};
    float distLength = sqrtf(dist.x * dist.x + dist.y * dist.y);
    Vector2 distNormalized = {dist.x /distLength, dist.y / distLength};

    if (distLength <= player->speed)
    {
        // player->position = point;
        return;
    }

    player->velocity = (Vector2){ player->speed * distNormalized.x, player->speed * distNormalized.y };

    player->position.x += player->velocity.x;
    player->position.y += player->velocity.y;
}

void PlayerBeginDash(Player *player, Vector2 point)
{
    player->state = PLAYER_DASHING;
    player->dash->reloadTime = player->dash->initReloadTime;

    Vector2 direction = Vector2Normalize((Vector2){player->position.x - point.x, player->position.y - point.y});
    player->dash->targetPos = direction;
    player->dash->targetPos = (Vector2){
            player->position.x - player->dash->targetPos.x * player->dash->distance,
            player->position.y - player->dash->targetPos.y * player->dash->distance };

    player->velocity.x = -(float)sign(direction.x);
}

bool PlayerLerpUntilPoint(Player *player, Vector2 point)
{
    player->position = (Vector2){
            Lerp(player->position.x, point.x, player->dash->speed),
            Lerp(player->position.y, point.y, player->dash->speed) };

    if (Vector2Distance(player->position, point) <= 2 || !IsInsideScreen(*player))
    {
        return true;
    }

    return false;
}

void PlayerDash(Player *player)
{
    if (player->colliding)
    {
        PlayerBeginSlice(player);
    }

    if (PlayerLerpUntilPoint(player, player->dash->targetPos))
        player->state = PLAYER_MOVING;
}

void PlayerBeginSlice(Player *player)
{
    if (player->collidingBall == NULL) return; // Error Handling

    player->state = PLAYER_SLICING;
    freezeBalls = true;
    player->dash->startPos = player->position;

    Vector2 ballPos = player->collidingBall->position;
    float ballRadius = player->collidingBall->radius;

    Vector2 distance = {ballPos.x - player->position.x, ballPos.y - player->position.y};
    Vector2 sliceTargetPoint = Vector2Normalize(distance);
    sliceTargetPoint = (Vector2){sliceTargetPoint.x * ballRadius + sliceTargetPoint.x * player->radius, sliceTargetPoint.y * ballRadius + sliceTargetPoint.y * player->radius};
    player->dash->targetPos = (Vector2){ballPos.x + sliceTargetPoint.x, ballPos.y + sliceTargetPoint.y};

    player->velocity.x = (float)sign(sliceTargetPoint.x); // So player faces in the correct direction
}

void PlayerSlice(Player *player)
{
    if (PlayerLerpUntilPoint(player, player->dash->targetPos))
    {
        player->state = PLAYER_MOVING;
        freezeBalls = false;
        player->dash->reloadTime = 0;
    }
}

bool IsInsideScreen(Player player)
{
    Vector2 clampedPos = {
            Clamp(player.position.x, player.radius, VIRTUAL_SCREEN_WIDTH - player.radius),
            Clamp(player.position.y, player.radius, VIRTUAL_SCREEN_HEIGHT - player.radius)};
    return player.position.x == clampedPos.x && player.position.y == clampedPos.y;
}

void PlayerCollisionScreen(Player *player)
{
    player->position.x = fmaxf(player->radius, fminf(player->position.x, VIRTUAL_SCREEN_WIDTH - player->radius));
    player->position.y = fmaxf(player->radius, fminf(player->position.y, VIRTUAL_SCREEN_HEIGHT - player->radius));
}

/*
void PlayerCollisionBall(Player *player, Ball balls[], int nbrOfBalls)
{
    for (int i = 0; i < nbrOfBalls; i++)
    {
        Vector2 ballDistVec = { balls[i].position.x - player->position.x, balls[i].position.y - player->position.y };
        float ballDist = ballDistVec.x * ballDistVec.x + ballDistVec.y * ballDistVec.y;

        if (ballDist <= player->radius * player->radius + balls[i].radius * balls[i].radius)
        {
            player->colliding = true;
            player->collidingBall = &balls[i];
            *player->collidingBallCopy = balls[i];
            return;
        }
    }

    player->colliding = false;
    player->collidingBall = NULL;
}
*/

void PlayerCollisionBall(Player *player, ListNode *ballHead)
{
    ListNode *currentBallNode = ballHead;
    while (currentBallNode != NULL)
    {
        Ball *currentBall = (Ball*)currentBallNode->data;
        Vector2 ballDistVec = { currentBall->position.x - player->position.x, currentBall->position.y - player->position.y };
        float ballDist = ballDistVec.x * ballDistVec.x + ballDistVec.y * ballDistVec.y;

        if (ballDist <= player->radius * player->radius + currentBall->radius * currentBall->radius)
        {
            player->colliding = true;
            player->collidingBall = currentBall;
            *player->collidingBallCopy = *currentBall;
            return;
        }

        currentBallNode = currentBallNode->next;
    }

    player->colliding = false;
    player->collidingBall = NULL;
}

void PlayerDraw(Player player)
{
    Vector2 textureOffset = { (float)player.texture.width / 2.0f, (float)player.texture.height / 2.0f };

    Rectangle playerRect =
            {
                0,
                0,
                sign(player.velocity.x) * player.texture.width,
                (float)player.texture.height
            };

    DrawTextureRec
    (
        player.texture,
        playerRect,
        (Vector2){ player.position.x - textureOffset.x,player.position.y - textureOffset.y },
        WHITE
    );

    if (player.state == PLAYER_SLICING) PlayerDrawSlice(player);

    /*
    if (player.colliding)
    {
        if (player.state == PLAYER_DASHING)
            DrawCircleLinesV(roundVector2(player.position), (float)player.texture.width, BLUE);
        else
            DrawCircleLinesV(round_vector2(player.position), (float)player.texture.width / 2, RED);
    }
    */
}

void PlayerDrawSlice(Player player)
{
    // Draw a shape from start pos to target pos, width is dependent on distance until slice is finished
    float distanceTotal = Vector2Distance(player.dash->targetPos, player.dash->startPos);
    float distanceLeft = Vector2Distance(player.dash->targetPos, player.position);
    float sliceRatio = distanceLeft / distanceTotal;

    Vector2 dashDir = {player.dash->targetPos.x - player.dash->startPos.x, player.dash->targetPos.y - player.dash->startPos.y};
    dashDir = Vector2Normalize(dashDir);
    float dashDirDegrees = atan2f(dashDir.y, dashDir.x);
    float tipAngle = PI * .3f;
    Vector2 orthogonalDir = {cosf(dashDirDegrees + tipAngle), sinf(dashDirDegrees + tipAngle)};

    float triangleWidth = sliceRatio * 16;
    Vector2 point1 = player.dash->startPos;
    Vector2 point2 = Vector2Add(player.dash->targetPos, (Vector2){orthogonalDir.x * triangleWidth, orthogonalDir.y * triangleWidth});
    Vector2 point3 = Vector2Add(player.dash->targetPos, (Vector2){-orthogonalDir.x * triangleWidth, -orthogonalDir.y * triangleWidth});

    // Sword slice triangle
    DrawTriangle(point1, point2, point3, WHITE);

    // Melon exploding
    Color circleColor = {255, 255, 255, sliceRatio * 255};
    DrawCircleV(player.collidingBallCopy->position, player.collidingBallCopy->radius, circleColor);
}

void PlayerDrawShadow(Player player)
{
    DrawTexture(player.shadowTexture, (int)roundf(player.position.x) - player.texture.width / 2, (int)roundf(player.position.y) - player.texture.height / 2, shadowColor);
}