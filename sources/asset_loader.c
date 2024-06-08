#include "asset_loader.h"

GameTextures gameTextures;
GameAudio gameAudio;

void LoadGameTextures()
{
    gameTextures.floorStandard = LoadTexture("../assets/textures/floor.png");
    gameTextures.melonBig = LoadTexture("../assets/textures/melon/melon_big.png");
    gameTextures.melonSmall = LoadTexture("../assets/textures/melon/melon_small.png");
    gameTextures.melonShadow = LoadTexture("../assets/textures/melon/melon_shadow_big.png");
    gameTextures.samurai = LoadTexture("../assets/textures/samurai/samurai.png");
    gameTextures.samuraiShadow = LoadTexture("../assets/textures/samurai/samurai_shadow.png");
    gameTextures.particleRound = LoadTexture("../assets/textures/particle/particle_round.png");
}

void MixGameAudio()
{
    SetSoundVolume(gameAudio.swordSlash, .5f);
    SetSoundVolume(gameAudio.melonSplats[0], .7f);
    SetSoundVolume(gameAudio.melonSplats[1], .7f);
    SetSoundVolume(gameAudio.melonSplats[2], .7f);

}

void LoadGameAudio()
{
    gameAudio.swordSlash = LoadSound("../assets/sfx/sword_slash.wav");
    gameAudio.dash = LoadSound("../assets/sfx/dash.wav");
    gameAudio.melonSplats[0] = LoadSound("../assets/sfx/melon_splat_1.wav");
    gameAudio.melonSplats[1] = LoadSound("../assets/sfx/melon_splat_2.wav");
    gameAudio.melonSplats[2] = LoadSound("../assets/sfx/melon_splat_3.wav");

    MixGameAudio();
}