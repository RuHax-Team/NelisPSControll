#include "_main.hpp"

#include <Geode/modify/FMODAudioEngine.hpp>
class $modify(FMODAudioEngineExt, FMODAudioEngine) {
    $override void playMusic(gd::string path, bool shouldLoop, float fadeInTime, int channel) {
        if (string::contains(path, "menuLoop")) {
            FMODAudioEngine::playMusic("MenuLoop_MelodyBass.mp3"_spr, shouldLoop, fadeInTime, channel);
            FMODAudioEngine::playEffectAdvanced("MenuLoop_Vocal.mp3"_spr, 1, 1, SETTING(double, "MenuLoop_Vocal"), 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
            FMODAudioEngine::playEffectAdvanced("MenuLoop_Drum.mp3"_spr, 1, 1, SETTING(double, "MenuLoop_Drum"), 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
            return;
        }
        return FMODAudioEngine::playMusic(path, shouldLoop, fadeInTime, channel);
    }
};