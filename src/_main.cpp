//#include "_main.hpp"

#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/MenuLayer.hpp>

// for user95401: initialize miniaudio only in _main.cpp
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

using namespace geode::prelude;

/*class $modify(FMODAudioEngineExt, FMODAudioEngine) {
    $override void playMusic(gd::string path, bool shouldLoop, float fadeInTime, int channel) {
        if (string::contains(path, "menuLoop")) {
            FMODAudioEngine::playMusic("MenuLoop_MelodyBass.mp3"_spr, shouldLoop, fadeInTime, channel);
            FMODAudioEngine::playEffectAdvanced("MenuLoop_Vocal.mp3"_spr, 1, 1, SETTING(double, "MenuLoop_Vocal"), 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
            FMODAudioEngine::playEffectAdvanced("MenuLoop_Drum.mp3"_spr, 1, 1, SETTING(double, "MenuLoop_Drum"), 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
            return;
        }
        return FMODAudioEngine::playMusic(path, shouldLoop, fadeInTime, channel);
    }
};*/

namespace NPGlobal {
    ma_engine snd_engine;
    bool engine_ready = false;
	bool lock1 = false;

    std::map<std::string, ma_sound*> loaded_sounds;

    void init() {
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

    void play_snd_file(std::string path) {
        if (!engine_ready) return;

        std::string global_path = "geode/unzipped/ruhax.nelispscontroll/resources/ruhax.nelispscontroll/" + path;

        ma_sound *snd = (ma_sound *)malloc(sizeof(ma_sound));
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

	ma_sound* ptrFromName(std::string name) {
		if (!loaded_sounds.contains(name)) return nullptr;

		return loaded_sounds[name];
	}

    void test() {
        if (!engine_ready) return;
		if (lock1) return;

        play_snd_file("MenuLoop_Drum.mp3");
        play_snd_file("MenuLoop_MelodyBass.mp3");
        play_snd_file("MenuLoop_Vocal.mp3");

		lock1 = true;
    }
}

enum SoundActionEnum {
	SA_VOLUME_DOWN = 0,
	SA_VOLUME_UP,
	SA_NONE
};
class SoundAction {
public:
	float volume = 1.f;
	SoundActionEnum action = SA_NONE;
	CCLabelBMFont* info_string;

	SoundAction() {
		info_string = CCLabelBMFont::create("-", "chatFont.fnt");
		info_string->setScale(0.5f);
	}
};

class SoundTestPopup : public FLAlertLayer {
private:
	std::map<std::string, SoundAction> _sndActions;
	float _delta;
public:
	void onExitButton(CCObject* sender) {
		removeMeAndCleanup();
	}

	void update(float delta) override {
		_delta = delta;

		for (auto& [k, act] : _sndActions) {
			switch (act.action) {
				case SA_VOLUME_UP: {
					act.volume += _delta * 2.f;
					if (act.volume >= 1.f) {
						act.volume = 1.f;
					}
					break;
				}
				case SA_VOLUME_DOWN: {
					act.volume -= _delta * 2.f;
					if (act.volume <= 0.f) {
						act.volume = 0.f;
					}
				}
			}

			std::string res = std::to_string(act.volume);
			act.info_string->setString(res.c_str());
			act.info_string->updateLabel();

			ma_sound* sound = NPGlobal::ptrFromName(k);

			if (sound) {
				ma_sound_set_volume(sound, act.volume);
			}
		}

		FLAlertLayer::update(delta);

		return;
	}

	static SoundTestPopup* create() {
		SoundTestPopup* pRet = new SoundTestPopup();
		if (pRet && pRet->init()) {
			pRet->autorelease();
			return pRet;
		}
		else {
			delete pRet;
			pRet = 0;
			return 0;
		}
	}

	void checkboxSnd(std::string _sound) {
		if (!_sndActions.contains(_sound)) {
			_sndActions[_sound] = SoundAction();
		}

		SoundAction& act = _sndActions.at(_sound);
		
		if (act.action == SA_NONE) {
			act.action = SA_VOLUME_UP;
			return;
		}
		if (act.action == SA_VOLUME_UP) {
			act.action = SA_VOLUME_DOWN;
			return;
		}
		if (act.action == SA_VOLUME_DOWN) {
			act.action = SA_VOLUME_UP;
			return;
		}
	}

	void onA(CCObject* sender) {
		checkboxSnd("MenuLoop_Drum.mp3");
	}
	void onB(CCObject* sender) {
		checkboxSnd("MenuLoop_MelodyBass.mp3");
	}
	void onC(CCObject* sender) {
		checkboxSnd("MenuLoop_Vocal.mp3");
	}

	bool init() {
		if (!FLAlertLayer::init(0)) return false;

		CCLayer* objectSelector = CCLayer::create();
		CCLayer* scale9layer = CCLayer::create();

		CCScale9Sprite* spr1 = CCScale9Sprite::create("GJ_square01.png");
		auto winsize = CCDirector::sharedDirector()->getWinSize();

		spr1->setContentSize({ 300, 125 });

		scale9layer->addChild(spr1);
		objectSelector->addChild(scale9layer, 0);

		scale9layer->setPosition({ winsize.width / 2, winsize.height / 2 });

		auto bmf = CCLabelBMFont::create("Sound Test", "bigFont.fnt");
		bmf->setScale(0.8f);
		bmf->setPositionX(winsize.width / 2);
		bmf->setPositionY(winsize.height / 2 + spr1->getContentSize().height / 2 - 15);

		objectSelector->addChild(bmf, 1);

		auto exitBtn = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
		auto btn3 = CCMenuItemSpriteExtra::create(
			exitBtn, this, menu_selector(SoundTestPopup::onExitButton)
		);

		CCMenu* men2 = CCMenu::create();

		men2->setPosition({
			winsize.width / 2 - spr1->getContentSize().width / 2,
			winsize.height / 2 + spr1->getContentSize().height / 2
			});
		men2->addChild(btn3);

		objectSelector->addChild(men2, 2);

		m_mainLayer->addChild(objectSelector);

		auto base = CCSprite::create("square.png");
		base->setPosition({ 0, 0 });
		base->setScale(500.f);
		base->setColor({ 0, 0, 0 });
		base->setOpacity(0);
		base->runAction(CCFadeTo::create(0.3f, 125));

		this->addChild(base, -1);

		auto menu1 = CCMenu::create();
		menu1->setLayout(RowLayout::create());

		{
			auto myButton = CCMenuItemSpriteExtra::create(
				CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
				this,
				menu_selector(SoundTestPopup::onA)
			);
			if (!_sndActions.contains("MenuLoop_Drum.mp3")) {
				_sndActions["MenuLoop_Drum.mp3"] = SoundAction();
				myButton->addChild(_sndActions["MenuLoop_Drum.mp3"].info_string);
			}

			menu1->addChild(myButton);
		}

		{
			auto myButton = CCMenuItemSpriteExtra::create(
				CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
				this,
				menu_selector(SoundTestPopup::onB)
			);
			if (!_sndActions.contains("MenuLoop_MelodyBass.mp3")) {
				_sndActions["MenuLoop_MelodyBass.mp3"] = SoundAction();
				myButton->addChild(_sndActions["MenuLoop_MelodyBass.mp3"].info_string);
			}

			menu1->addChild(myButton);
		}

		{
			auto myButton = CCMenuItemSpriteExtra::create(
				CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
				this,
				menu_selector(SoundTestPopup::onC)
			);
			if (!_sndActions.contains("MenuLoop_Vocal.mp3")) {
				_sndActions["MenuLoop_Vocal.mp3"] = SoundAction();
				myButton->addChild(_sndActions["MenuLoop_Vocal.mp3"].info_string);
			}

			menu1->addChild(myButton);
		}

		menu1->updateLayout();
		objectSelector->addChild(menu1);

		show();

		scheduleUpdate();

		return true;
	}

	void registerWithTouchDispatcher() override {
		CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();

		dispatcher->addTargetedDelegate(this, -502, true);
	}
};


class $modify(NMenuLayer, MenuLayer) {
    void onSoundTest(CCObject * sender) {
		SoundTestPopup::create();
    }

    bool init() {
        if (!MenuLayer::init()) return false;

        NPGlobal::init();
		NPGlobal::test();

        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
            this,
            menu_selector(NMenuLayer::onSoundTest)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(myButton);

        myButton->setID("my-button"_spr);

        menu->updateLayout();

        return true;
    }
};