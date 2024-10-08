#include <math.h>
#include <string.h>
#include <stdio.h>
#include "tutorial.h"
#include "game.h"
#include "renderer.h"
#include "spawner.h"
#include "global.h"
#include "ball.h"
#include "score_handler.h"
#include "asset_loader.h"
#include "storage.h"

int tutorialStateIndex = 0;
void (*tutorialState)();
double stateEndTime = 0;
char *tutorialText = "Text";
int tutorialTextWidth;

const double statesTimeDuration[TUTORIAL_LENGTH] =
        {
            1,
            2.5,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            2
        };
bool statesComplete[TUTORIAL_LENGTH] = {false};

bool stateBegun = false;

typedef struct
{
    bool started;
    double startTime;
    double duration;
} SliceTimer;

SliceTimer combos2Timer = {false, 0, 4};

int textCounter = 0;

void SetTutorialText(char* text)
{
    tutorialText = text;
    tutorialTextWidth = MeasureText(tutorialText, 8);
}

void BeginState(TutorialStates index)
{
    stateBegun = true;

    switch (tutorialStateIndex)
    {
        case TUTORIAL_SLICING:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_LARGE, true, TYPE_MELON);
            break;
        }

        case TUTORIAL_SLICE_CHAIN:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_LARGE, true, TYPE_MELON);
            break;
        }

        case TUTORIAL_SLICE_CHAIN_2:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_LARGE, true, TYPE_MELON);
            break;
        }

        case TUTORIAL_SLICE_CHAIN_3:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_LARGE, true, TYPE_MELON);
            break;
        }

        case TUTORIAL_COMBOS:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_LARGE, true, TYPE_MELON);
            break;
        }

        case TUTORIAL_COMBOS_2:
        {
            combos2Timer.started = false;
            SpawnerPlaceBallSpawnPoint(RADIUS_LARGE, true, TYPE_MELON);
            SetUiProgressBarMidToEnds(&spawningProgressBar, 0, 1);
            break;
        }

        case TUTORIAL_ORANGES:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_MEDIUM, true, TYPE_ORANGE);
            break;
        }

        case TUTORIAL_ORANGES_2:
        {
            SpawnerPlaceBallSpawnPoint(RADIUS_MEDIUM, true, TYPE_ORANGE);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            SpawnerPlaceBallSpawnPoint(RADIUS_SMALL, true, TYPE_MELON);
            break;
        }

        default:
        {
            return;
        }
    }
}

void StateMoving()
{
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) statesComplete[TUTORIAL_MOVING] = true;
}

void StateDashing()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) statesComplete[TUTORIAL_DASHING] = true;
}

void StateSlicing()
{
    if (ListLength(&ballHead) > 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        freezeBalls = true;
    }

    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        statesComplete[TUTORIAL_SLICING] = true;
    }
}

void StateSliceChain()
{
    if (ListLength(&ballHead) > 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        freezeBalls = true;
    }

    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        statesComplete[TUTORIAL_SLICE_CHAIN] = true;
    }
}

void StateSliceChain2()
{
    if (ListLength(&ballHead) > 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        freezeBalls = true;
    }

    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        statesComplete[TUTORIAL_SLICE_CHAIN_2] = true;
        freezeBalls = false;
    }
}

void StateSliceChain3()
{
    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        statesComplete[TUTORIAL_SLICE_CHAIN_3] = true;
    }
}

void StateCombos()
{
    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        if (ScoreHandlerGetComboScore() > 2)
        {
            statesComplete[TUTORIAL_COMBOS] = true;
        }
        else
        {
            BeginState(TUTORIAL_COMBOS);
            SetTutorialText("Don't lose your combo.");
            textCounter = 0;
        }
    }
}

void StateCombos2()
{
    if (statesComplete[TUTORIAL_COMBOS_2]) return;

    if (ListLength(&ballHead) > 1 && !combos2Timer.started)
    {
        combos2Timer.started = true;
        combos2Timer.startTime = GetTime();
    }

    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        if (GetTime() <= combos2Timer.startTime + combos2Timer.duration)
        {
            statesComplete[TUTORIAL_COMBOS_2] = true;
        }
        else
        {
            TutorialSetState(TUTORIAL_COMBOS_2);
        }
    }

    if (combos2Timer.started)
    {
        SetUiProgressBarMidToEnds(&spawningProgressBar, GetTime() - combos2Timer.startTime, combos2Timer.duration);
    }
}

void StateOranges()
{
    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        if (!statesComplete[TUTORIAL_ORANGES]) targetFps = 2;
        statesComplete[TUTORIAL_ORANGES] = true;
    }
}

void StateOranges2()
{
    if (ListLength(&ballHead) == 1 && ListLength(&ballSpawnPointHead) == 0)
    {
        Ball *ball = (Ball*)ballHead->data;
        if (ball->type == TYPE_ORANGE)
        {
            BallDestroy(ball);
            TutorialSetState(TUTORIAL_ORANGES_2);
            return;
        }
    }

    if (ListLength(&ballHead) == 0 && ListLength(&ballSpawnPointHead) == 0)
    {
        statesComplete[TUTORIAL_ORANGES_2] = true;
    }
}

void StateEnd()
{
    statesComplete[TUTORIAL_END] = true;
}

void TutorialSetState(TutorialStates index)
{
    tutorialStateIndex = index;
    SaveStorageValue(STORAGE_POSITION_TUTORIAL_STATE_INDEX, tutorialStateIndex);
    textCounter = 0;
    stateBegun = false;

    switch (tutorialStateIndex)
    {
        case TUTORIAL_MOVING:
        {
            SetTutorialText("Right Mouse Button to move.");
            tutorialState = StateMoving;
            break;
        }

        case TUTORIAL_DASHING:
        {
            SetTutorialText("Left Mouse Button to dash.");
            tutorialState = StateDashing;
            break;
        }

        case TUTORIAL_SLICING:
        {
            SetTutorialText("Dash into a melon to slice it.");
            tutorialState = StateSlicing;
            break;
        }

        case TUTORIAL_SLICE_CHAIN:
        {
            SetTutorialText("Slice recharges dash instantly.");
            tutorialState = StateSliceChain;
            break;
        }

        case TUTORIAL_SLICE_CHAIN_2:
        {
            SetTutorialText("So you can slice super fast.");
            tutorialState = StateSliceChain2;
            break;
        }

        case TUTORIAL_SLICE_CHAIN_3:
        {
            SetTutorialText("Chain quick, consecutive slices.");
            tutorialState = StateSliceChain3;
            break;
        }

        case TUTORIAL_COMBOS:
        {
            SetTutorialText("Missed slice = Lose Combo.");
            tutorialState = StateCombos;
            break;
        }

        case TUTORIAL_COMBOS_2:
        {
            SetTutorialText("Destroy the entire melon in 4s.");
            tutorialState = StateCombos2;
            SetUiProgressBarMidToEnds(&spawningProgressBar, 0, 1);
            break;
        }

        case TUTORIAL_ORANGES:
        {
            SetTutorialText("Oranges multiply your combo.");
            tutorialState = StateOranges;
            break;
        }

        case TUTORIAL_ORANGES_2:
        {
            SetTutorialText("Oranges also clear melons.");
            tutorialState = StateOranges2;
            break;
        }

        case TUTORIAL_END:
        {
            SetTutorialText("Good luck.");
            tutorialState = StateEnd;
            break;
        }

        default:
        {
            ScoreHandlerResetScore();
            SetTutorialText("");
            RendererPlayRingTransition(GameStart);
            return;
        }
    }
}

void TutorialBegin()
{
    if (gameState == GAME_TUTORIAL) return;

    gameState = GAME_TUTORIAL;
    stateEndTime = FRAME_COUNTER_TO_TIME;
    TutorialSetState(tutorialStateIndex);
}

void TutorialUpdate()
{
    if (!statesComplete[tutorialStateIndex]) stateEndTime = FRAME_COUNTER_TO_TIME + statesTimeDuration[tutorialStateIndex];

    int tutorialTextLength = (int)strlen(tutorialText);

    if (textCounter == tutorialTextLength)
    {
        if (!stateBegun) BeginState(tutorialStateIndex);
    }
    else if (textCounter < tutorialTextLength)
    {
        if (frameCounter % 3 == 0)
        {
            textCounter++;
            PlaySound(gameAudio.tutorialText);
        }
        return;
    }

    bool playedClearSound = statesComplete[tutorialStateIndex];

    if (!TutorialIsFinished()) tutorialState();

    if (playedClearSound != statesComplete[tutorialStateIndex])
    {
        PlaySound(gameAudio.tutorialClear);
    }

    if (FRAME_COUNTER_TO_TIME > stateEndTime + statesTimeDuration[tutorialStateIndex])
    {
        TutorialSetState(tutorialStateIndex+1);
    }
}

void TutorialDraw()
{
    Color textColor = (statesComplete[tutorialStateIndex]) ? GREEN : WHITE;
    int yPos = (int)virtualScreenCenter.y - 8 + (int)(sin(GetTime() * 4) * 4);
    DrawText(TextSubtext(tutorialText, 0, textCounter), (int)virtualScreenCenter.x - tutorialTextWidth / 2, yPos, 8, textColor);
}

void TutorialReset()
{
    SaveStorageValue(STORAGE_POSITION_TUTORIAL_STATE_INDEX, 0);

    tutorialStateIndex = 0;
    for (int i = 0; i < TUTORIAL_LENGTH; i++)
    {
        statesComplete[i] = false;
    }
}

bool TutorialIsFinished()
{
    return tutorialStateIndex >= TUTORIAL_LENGTH;
}
