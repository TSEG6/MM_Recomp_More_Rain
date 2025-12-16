#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

static s16 lastSceneId = -1;
static u8 rainActive = 0;

static s16 rainScenes[] = {
    SCENE_00KEIKOKU,
    SCENE_30GYOSON,
    SCENE_31MISAKI,
    SCENE_KAIZOKU,
    SCENE_24KEMONOMITI,
    SCENE_11GORONNOSATO2,
    SCENE_10YUKIYAMANOMURA2,
    SCENE_TOWN,
    SCENE_ICHIBA,
    SCENE_BACKTOWN,
    SCENE_CLOCKTOWER,
    SCENE_ALLEY,
    SCENE_20SICHITAI2,
    SCENE_24KEMONOMITI,
    SCENE_20SICHITAI,
    SCENE_21MITURINMAE,
    SCENE_F01,
    SCENE_ROMANYMAE,
    SCENE_KOEPONARACE,
    SCENE_TORIDE
};

static s32 IsRainScene(s16 sceneId) {
    for (size_t i = 0; i < sizeof(rainScenes) / sizeof(rainScenes[0]); i++) {
        if (sceneId == rainScenes[i]) {
            return 1;
        }
    }
    return 0;
}


static s32 IsRainTime(void) {
    if (CURRENT_DAY != 2) {
        return 0;
    }
    return (CURRENT_TIME >= CLOCK_TIME(7, 0) && CURRENT_TIME < CLOCK_TIME(17, 30));
}

RECOMP_HOOK("Play_Update")
void more_rain(PlayState* play) {

    
    if (play->sceneId != lastSceneId) {
        lastSceneId = play->sceneId;
        rainActive = 0; 
    }

    u8 shouldRain = IsRainScene(play->sceneId) && IsRainTime();

    
    if (shouldRain && !rainActive) {
        
        rainActive = 1;
        gWeatherMode = WEATHER_MODE_RAIN;
        play->envCtx.stormState = STORM_STATE_ON;
        play->envCtx.lightningState = LIGHTNING_ON;

        if (play->envCtx.precipitation[PRECIP_RAIN_CUR] < 60) {
            play->envCtx.precipitation[PRECIP_RAIN_CUR] = 0;
        }
        if (play->envCtx.precipitation[PRECIP_RAIN_MAX] < 60) {
            play->envCtx.precipitation[PRECIP_RAIN_MAX] = 60;
        }

        Environment_PlayStormNatureAmbience(play);

    }
    else if (!shouldRain && rainActive) {
        
        rainActive = 0;
        gWeatherMode = WEATHER_MODE_CLEAR;
        play->envCtx.stormState = STORM_STATE_OFF;
        play->envCtx.lightningState = LIGHTNING_OFF;
        play->envCtx.precipitation[PRECIP_RAIN_CUR] = 0;
        play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;
    }
}
