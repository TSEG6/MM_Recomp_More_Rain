#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "dependency.h"
#include "z_en_test4.h"

int RAIN_INTENSITY_TARGET = 60;
#define RAIN_FADE_SPEED 1
int RainUpgradeModActive;
static u8 rainActive = 0;
static s16 lastScene = -1;


RECOMP_HOOK("Play_Init")
void depcheck() {
    RainUpgradeModActive = recomp_is_dependency_met("mm_recomp_upgraded_rain") == DEPENDENCY_STATUS_FOUND;
}


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
    SCENE_20SICHITAI,
    SCENE_21MITURINMAE,
    SCENE_F01,
    SCENE_ROMANYMAE,
    SCENE_KOEPONARACE,
    SCENE_TORIDE,
    SCENE_35TAKI,
    SCENE_F01C,
    SCENE_F01_B
};


static s32 IsRainScene(s16 sceneId) {
    for (size_t i = 0; i < ARRAY_COUNT(rainScenes); i++) {
        if (sceneId == rainScenes[i]) {
            return 1;
        }
    }
    return 0;
}


static s32 IsRainTime(void) {
    if (CURRENT_DAY != 2) return 0;

    return (CURRENT_TIME >= CLOCK_TIME((int)recomp_get_config_double("start_hour"), 0) &&
        CURRENT_TIME < CLOCK_TIME((int)recomp_get_config_double("end_hour"), 30));
}


RECOMP_HOOK("Play_Update")
void more_rain(PlayState* play) {

    if (CURRENT_DAY != 2) return;

    if (play->sceneId != lastScene) {
        lastScene = play->sceneId;

        if (!IsRainScene(play->sceneId)) {
            rainActive = 0;

            play->envCtx.precipitation[PRECIP_RAIN_CUR] = 0;
            play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;

            play->envCtx.stormState = STORM_STATE_OFF;
            play->envCtx.lightningState = LIGHTNING_OFF;

            gWeatherMode = WEATHER_MODE_CLEAR;

            Environment_StopStormNatureAmbience(play);
        }
    }

    if (RainUpgradeModActive) {
        RAIN_INTENSITY_TARGET = RAIN_INTENSITY();
    }
    else {

    }

    u8 shouldRain = IsRainScene(play->sceneId) && IsRainTime();


    if (shouldRain) {
        if (!rainActive) {
            rainActive = 1;
            Environment_PlayStormNatureAmbience(play);
        }

        gWeatherMode = WEATHER_MODE_RAIN;
        play->envCtx.lightningState = LIGHTNING_ON;
        play->envCtx.stormState = STORM_STATE_ON;

        play->envCtx.precipitation[PRECIP_RAIN_MAX] = RAIN_INTENSITY_TARGET;
    }


    if (rainActive && play->envCtx.precipitation[PRECIP_RAIN_CUR] < play->envCtx.precipitation[PRECIP_RAIN_MAX]) {
        u8 diff = play->envCtx.precipitation[PRECIP_RAIN_MAX] - play->envCtx.precipitation[PRECIP_RAIN_CUR];
        play->envCtx.precipitation[PRECIP_RAIN_CUR] += (diff < RAIN_FADE_SPEED) ? diff : RAIN_FADE_SPEED;
    }

    if (play->envCtx.precipitation[PRECIP_RAIN_CUR] >= 25) {
        play->envCtx.stormState = STORM_STATE_ON;
    }


    if (!shouldRain && rainActive) {

        if ((play->state.frames % 4) == 0) {

            if (play->envCtx.precipitation[PRECIP_RAIN_MAX] > 0) {
                play->envCtx.precipitation[PRECIP_RAIN_MAX]--;
            }

            if (play->envCtx.precipitation[PRECIP_RAIN_CUR] > 0) {
                play->envCtx.precipitation[PRECIP_RAIN_CUR]--;
            }

            if (play->envCtx.precipitation[PRECIP_RAIN_MAX] <= 8) {
                Environment_StopStormNatureAmbience(play);
            }
        }

        if (play->envCtx.precipitation[PRECIP_RAIN_CUR] == 0) {
            rainActive = 0;
            play->envCtx.stormState = STORM_STATE_OFF;
            play->envCtx.lightningState = LIGHTNING_OFF;
            gWeatherMode = WEATHER_MODE_CLEAR;
        }
    }
//    recomp_printf(
//        "[RainMod] "
//        "Scene=%d "
//        "LastScene=%d "
//        "Day=%d "
//        "Time=%d "
//        "rainScenes=%d "
//        "RainTime=%d "
//        "ShouldRain=%d "
//        "RainActive=%d "
//        "RainCur=%d "
//        "RainMax=%d "
//        "StormState=%d "
//        "LightningState=%d "
//        "WeatherMode=%d\n",
//        play->sceneId,
//        lastScene,
//        CURRENT_DAY,
//        CURRENT_TIME,
//        rainScenes,
//        IsRainTime,
//        shouldRain,
//        rainActive,
//        play->envCtx.precipitation[PRECIP_RAIN_CUR],
//        play->envCtx.precipitation[PRECIP_RAIN_MAX],
//        play->envCtx.stormState,
//        play->envCtx.lightningState,
//        gWeatherMode
//    );
}