#pragma once

#include <miniaudio.h>
#include <map>
#include <string>

namespace NPGlobal {
	extern ma_engine snd_engine;
	extern bool engine_ready;
	extern bool lock1;

	extern std::map<std::string, ma_sound*> loaded_sounds;

	void init();

	void playSndFile(std::string path);
	ma_sound* ptrFromName(std::string name);

	void test();
}