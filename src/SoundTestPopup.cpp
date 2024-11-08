#include "SoundTestPopup.hpp"
#include <miniaudio.h>
#include "NPGlobal.hpp"

void SoundTestPopup::onExitButton(CCObject* sender) {
	removeMeAndCleanup();
}

void SoundTestPopup::update(float delta) {
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

SoundTestPopup* SoundTestPopup::create() {
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

void SoundTestPopup::checkboxSnd(std::string _sound) {
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

void SoundTestPopup::onA(CCObject* sender) {
	checkboxSnd("MenuLoop_Drum.mp3");
}
void SoundTestPopup::onB(CCObject* sender) {
	checkboxSnd("MenuLoop_MelodyBass.mp3");
}
void SoundTestPopup::onC(CCObject* sender) {
	checkboxSnd("MenuLoop_Vocal.mp3");
}

bool SoundTestPopup::init() {
	if (!FLAlertLayer::init(0)) return false;

	CCLayer* objectSelector = CCLayer::create();
	CCLayer* scale9layer = CCLayer::create();

	CCScale9Sprite* spr1 = CCScale9Sprite::create("GJ_square01.png");
	auto winsize = CCDirector::sharedDirector()->getWinSize();

	spr1->setContentSize({ 300, 125 });

	scale9layer->addChild(spr1);
	objectSelector->addChild(scale9layer, 0);

	scale9layer->setPosition({ winsize.width / 2, winsize.height / 2 });

	auto title = u8"Тестирование звука";

	CCLabelTTF* bmf = CCLabelTTF::create((const char*)title, "ARIAL.ttf"_spr, 20.f);
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

void SoundTestPopup::registerWithTouchDispatcher() {
	CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();

	dispatcher->addTargetedDelegate(this, -502, true);
}
