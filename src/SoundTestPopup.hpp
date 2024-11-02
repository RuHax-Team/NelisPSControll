#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

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
	void onExitButton(CCObject* sender);

	void update(float delta) override;

	bool init() override;
	void registerWithTouchDispatcher() override;
	static SoundTestPopup* create();

	void checkboxSnd(std::string _sound);

	void onA(CCObject* sender);
	void onB(CCObject* sender);
	void onC(CCObject* sender);
};