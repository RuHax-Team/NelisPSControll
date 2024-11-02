#include "NPGlobal.hpp"

ma_engine NPGlobal::snd_engine = { 0 };
bool NPGlobal::engine_ready = false;
bool NPGlobal::lock1 = false;
std::map<std::string, ma_sound*> NPGlobal::loaded_sounds;

#include <Geode/Geode.hpp>
using namespace geode::prelude;

void NPGlobal::init() {
    if (!engine_ready) {
        ma_result res = ma_engine_init(0, &snd_engine);
        if (res != MA_SUCCESS) {
            log::error("could not initialize miniaudio ({})", (int)res);
        }
        else {
            engine_ready = true;
            log::info("initialized miniaudio");
        }
    }
}

void NPGlobal::playSndFile(std::string path) {
    if (!engine_ready) return;

    std::string global_path = "geode/unzipped/ruhax.nelispscontroll/resources/ruhax.nelispscontroll/" + path;

    ma_sound* snd = (ma_sound*)malloc(sizeof(ma_sound));
    memset(snd, 0, sizeof(ma_sound));

    ma_result res = ma_sound_init_from_file(&snd_engine, global_path.c_str(), 0, nullptr, nullptr, snd);
    if (res != MA_SUCCESS) {
        log::error("could not play audio file {}: {}", path, (int)res);
        return;
    }
    else {
        log::info("initialized audio file {}", path);
    }

    ma_sound_set_looping(snd, 1);
    ma_sound_start(snd);

    loaded_sounds[path] = snd;
}

ma_sound* NPGlobal::ptrFromName(std::string name) {
    if (!loaded_sounds.contains(name)) return nullptr;

    return loaded_sounds[name];
}

void NPGlobal::test() {
    if (!engine_ready) return;
    if (lock1) return;

    playSndFile("MenuLoop_Drum.mp3");
    playSndFile("MenuLoop_MelodyBass.mp3");
    playSndFile("MenuLoop_Vocal.mp3");

    lock1 = true;
}