//#include "_main.hpp"

#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include <Geode/Geode.hpp>

// for user95401: initialize miniaudio only in _main.cpp
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

using namespace geode::prelude;

#include "NPGlobal.hpp"
#include "SoundTestPopup.hpp"

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