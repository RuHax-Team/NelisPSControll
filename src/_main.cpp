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

// opengd addition
class SimpleProgressBar : public cocos2d::CCLayer {
protected:
    cocos2d::CCSprite* _pGroove;
    cocos2d::CCSprite* _pBar;

    float _pBarWidth;

    cocos2d::ccTexParams _params;
public:
    CREATE_FUNC(SimpleProgressBar);

    virtual bool init() {
        _pGroove = CCSprite::create("slidergroove_02.png");
        _pBar = CCSprite::create("sliderBar.png");

        // config
        _pBarWidth = _pGroove->getContentSize().width - 4;
        setContentSize(_pGroove->getContentSize());

        _params.magFilter = GL_NEAREST;
        _params.minFilter = GL_NEAREST;
        _params.wrapS = GL_REPEAT;
        _params.wrapT = GL_REPEAT;

        _pBar->getTexture()->setTexParameters(&_params);
        _pBar->setAnchorPoint({ 0, 0 });
        _pBar->setPosition({ 2.f, 1.25f });

        // add everything
        _pGroove->addChild(_pBar, -1);
        addChild(_pGroove);

        scheduleUpdate();
        return true;
    }

    void update(float delta) {
        if (_pPercentage > 100.f) {
            _pPercentage = 100.f;
        } else if (_pPercentage < 0.f) {
            _pPercentage = 0.f;
        }

        _pBar->setTextureRect({ 0, 0, _pBarWidth * (this->_pPercentage / 100), _pBar->getContentSize().height });

        return;
    }

    void setBarColor(ccColor3B color) {
        _pBar->setColor(color);
    }

    CC_SYNTHESIZE(float, _pPercentage, Percentage);
};

#include "iAndy.hpp"

class NPUiLevelCell : public cocos2d::CCNode {
private:
    CCScale9Sprite* _bg;
    cocos2d::CCNode* _infoContainer;
    cocos2d::CCNode* _titleContainer;
    cocos2d::CCLabelBMFont* _titleLabel;
    cocos2d::CCMenu* _buttonMenu;

    std::vector<CCSprite*> _cardIcons;

    GJGameLevel* _level;
public:
    static NPUiLevelCell* create(GJGameLevel *level) {
        NPUiLevelCell* pRet = new NPUiLevelCell(); if (pRet && pRet->init(level)) {
            pRet->autorelease(); return pRet;
        }
        else {
            delete pRet; pRet = 0; return 0;
        }
    };

    std::string getLengthString() {
        switch (_level->m_levelLength) {
        case 0: {
            return std::string((const char *)u8"Короткий");
        }
        case 1: {
            return std::string((const char*)u8"Средний");
        }
        case 2: {
            return std::string((const char*)u8"Длинный");
        }
        case 3: {
            return std::string((const char*)u8"Платформер");
        }
        }
        
        return "???";
    }

    std::string getLikeAmount() {
        float mult = 1.f;

        return iAndy::intToFormatString(_level->m_likes + _level->m_dislikes, mult);
    }
    std::string getDownloadAmount() {
        float mult = 1.f;

        return iAndy::intToFormatString(_level->m_downloads, mult);
    }

    bool init(GJGameLevel* level) {
        if (!CCNode::init()) return false;

        _level = level;

        // log::info("mainNode: {}", m_mainLayer);

        _bg = CCScale9Sprite::create("square02_small.png");
        _bg->setID("bg");
        _bg->setOpacity(80);
        _bg->ignoreAnchorPointForPosition(false);
        _bg->setAnchorPoint({ .5f, .5f });
        _bg->setScale(.7f);
        this->addChild(_bg);

        _infoContainer = CCNode::create();
        _infoContainer->setID("info-container");

        _titleContainer = CCNode::create();

        std::string id_str = level->m_levelName;

        _titleLabel = CCLabelBMFont::create(id_str.c_str(), "bigFont.fnt", CCTextAlignment::kCCTextAlignmentLeft);
        _titleLabel->setID("title-label");
        _titleLabel->setScale(0.5f);
        _titleLabel->setPositionX(-16.f);
        _titleLabel->setPositionY(10.f);
        _titleLabel->setAnchorPoint({ 0.f, 0.5f });

        std::string length = getLengthString();
        std::string likes = getLikeAmount();
        std::string downloads = getDownloadAmount();

        {
            CCSprite* spr = CCSprite::createWithSpriteFrameName("time_lvl.png"_spr);

            CCSize sz = { 140, 70 };
            CCLabelTTF* label = CCLabelTTF::create(length.c_str(), "ARIAL.ttf"_spr, 10.f, sz, CCTextAlignment::kCCTextAlignmentLeft);

            spr->setScale(0.5f);
            spr->setPosition({ -10.f, -7.f });
            label->setPosition({ 71.f, -38.f });

            _infoContainer->addChild(spr);
            _infoContainer->addChild(label);
        }

        {
            CCSprite* spr = CCSprite::createWithSpriteFrameName("download.png"_spr);

            CCSize sz = { 140, 70 };
            CCLabelTTF* label = CCLabelTTF::create(downloads.c_str(), "ARIAL.ttf"_spr, 10.f, sz, CCTextAlignment::kCCTextAlignmentLeft);

            spr->setScale(0.5f);
            spr->setPosition({ 53.f, -7.f });
            label->setPosition({ 132.f, -38.f });

            _infoContainer->addChild(spr);
            _infoContainer->addChild(label);
        }

        {
            CCSprite* spr = CCSprite::createWithSpriteFrameName("like-lvl.png"_spr);

            CCSize sz = { 140, 70 };
            CCLabelTTF* label = CCLabelTTF::create(likes.c_str(), "ARIAL.ttf"_spr, 10.f, sz, CCTextAlignment::kCCTextAlignmentLeft);

            spr->setScale(0.5f);
            spr->setPosition({ 110.f, -7.f });
            label->setPosition({ 191.f, -38.f });

            _infoContainer->addChild(spr);
            _infoContainer->addChild(label);
        }

        if (_titleLabel->getContentWidth() > 196.f) {
            float sc = _titleLabel->getContentWidth() / 196.f;
            _titleLabel->setScale(_titleLabel->getScale() / sc);
        }

        _infoContainer->addChild(_titleLabel);

        CCSprite* joinSprite = nullptr;

        joinSprite = CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");
        joinSprite->setRotation(-90.f);
        joinSprite->setFlipX(true);
        joinSprite->setScale(1.5f);
        joinSprite->setPositionX(-6.f);
       
        _buttonMenu = CCMenu::create();

        CCMenuItemSpriteExtra* joinBtn = CCMenuItemSpriteExtra::create(joinSprite, this, menu_selector(NPUiLevelCell::onJoin));
        joinBtn->setPositionX(-13.f);

        _buttonMenu->addChild(joinBtn);
        _buttonMenu->setPositionX(36.f);

        CCNode* progressNode = CCNode::create();

        SimpleProgressBar* bar = SimpleProgressBar::create();
        bar->setPercentage(level->m_normalPercent.value());
        bar->update(0.f);
        bar->setPosition({ 5.f, 1.f });
        progressNode->addChild(bar);

        bar = SimpleProgressBar::create();
        bar->setPercentage(level->m_practicePercent);
        bar->update(0.f);
        bar->setPosition({ 5.f, 19.f });
        bar->setBarColor({ 255, 255, 0 });
        progressNode->addChild(bar);

        progressNode->setScale(0.575f);
        progressNode->setPosition({ 145.f, 4.f });

        _infoContainer->addChild(progressNode);

        std::string a = std::string((const char *)(u8"От ")) + level->m_creatorName;

        //CCLabelBMFont* authorLabel = CCLabelBMFont::create(level->m_creatorName.c_str(), "bigFont.fnt", CCTextAlignment::kCCTextAlignmentRight);
        CCSize sz = { 140, 70 };
        CCLabelTTF* authorLabel = CCLabelTTF::create(a.c_str(), "ARIAL.ttf"_spr, 25.f, sz, CCTextAlignment::kCCTextAlignmentRight);
        authorLabel->setScale(0.45f);
        authorLabel->setAnchorPoint({ authorLabel->getAnchorPoint().x, 0.45f });
        authorLabel->setPositionX(260.f);
        authorLabel->setColor({ 51, 255, 204 });

        _infoContainer->addChild(authorLabel);

        this->addChild(_infoContainer);
        this->addChild(_buttonMenu);
        this->setID("level-item");

        return true;
    }
    void updateSize(float width) {
        this->setContentSize({ width, 50.f });

        _bg->setContentSize((m_obContentSize - ccp(6, 0)) / _bg->getScale());
        _bg->setPosition(m_obContentSize / 2);

        CCSize titleSpace{
            m_obContentSize.width / 2 - m_obContentSize.height,
            0
        };

        _infoContainer->setPosition(m_obContentSize.height + 10, m_obContentSize.height / 2);
        _infoContainer->setContentSize(ccp(titleSpace.width, titleSpace.height) / _infoContainer->getScale());
        // _infoContainer->updateLayout();

        _titleContainer->setContentWidth(titleSpace.width / _infoContainer->getScale());
        // _titleContainer->updateLayout();

        _infoContainer->setPosition(m_obContentSize.height + 10, m_obContentSize.height / 2);
        _infoContainer->setContentSize(ccp(titleSpace.width, titleSpace.height) / _infoContainer->getScale());
        // s_infoContainer->updateLayout();

        _buttonMenu->setPosition(m_obContentSize.height + 10, m_obContentSize.height / 2);
        _buttonMenu->setContentSize(ccp(titleSpace.width, titleSpace.height) / _buttonMenu->getScale());
        _buttonMenu->setPositionX(36.f);

        this->updateLayout();
    }

    void onJoin(cocos2d::CCObject* sender) {
        log::info("join");
    }
    void onInfo(cocos2d::CCObject* sender) {
        log::info("info");
    }

    void setBgOpacity(unsigned char op) {
        _bg->setOpacity(op);
    }
};

class NPUiLevelList : public CCNode {
private:
    ScrollLayer* _list;
    cocos2d::CCLayerColor* _bg;
public:
    CREATE_FUNC(NPUiLevelList);

    bool init() {
        if (!CCNode::init()) return false;

        this->setContentSize({ 360, 200 });
        this->setAnchorPoint({ .5f, .5f });
        this->setID("level-list");

        _bg = CCLayerColor::create({ 0, 0, 0 }, this->getContentWidth(), this->getContentHeight());
        _bg->setOpacity(128);

        addChild(_bg);

        _list = ScrollLayer::create(getContentSize());
        _list->m_contentLayer->setLayout(
            ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(getContentSize().height)
            ->setGap(2.5f)
        );
        this->addChildAtPosition(_list, Anchor::Bottom, ccp(-_list->getScaledContentWidth() / 2, 0));

        return true;
    }
    void updateSize() {
        for (auto& node : CCArrayExt<CCNode*>(_list->m_contentLayer->getChildren())) {
            if (auto item = typeinfo_cast<NPUiLevelCell*>(node)) {
                item->updateSize(_list->getContentWidth());
            }
        }

        // Store old relative scroll position (ensuring no divide by zero happens)
        auto oldPositionArea = _list->m_contentLayer->getContentHeight() - _list->getContentHeight();
        auto oldPosition = oldPositionArea > 0.f ?
            _list->m_contentLayer->getPositionY() / oldPositionArea :
            -1.f;

        // Auto-grow the size of the list content
        _list->m_contentLayer->updateLayout();

        // Preserve relative scroll position
        _list->m_contentLayer->setPositionY((
            _list->m_contentLayer->getContentHeight() - _list->getContentHeight()
        ) * oldPosition);
    }

    void addCell(NPUiLevelCell* cell) {
        if (_list == nullptr) {
            log::info("NPUiLevelList: error: _list = nullptr");

            return;
        }

        _list->m_contentLayer->addChild(cell);

        if ((_list->m_contentLayer->getChildrenCount() % 2) == 1) {
            cell->setBgOpacity(115);
        }

        updateSize();
    }
    void removeCells() {
        _list->m_contentLayer->removeAllChildrenWithCleanup(true);
    }
};

class NPUiSearch : public CCLayer {
public:
    static NPUiSearch* create() {
        NPUiSearch* pRet = new NPUiSearch();
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

    bool init() override {
        static const auto _t = u8"Поиск уровней";

        CCLabelTTF* title = CCLabelTTF::create((const char*)_t, "ARIAL.ttf"_spr, 20.f);
        CCLayerColor* col = CCLayerColor::create({ 51, 51, 51, 255 });

        const auto wsz = CCDirector::get()->getWinSize();

        title->setPosition({ wsz.width / 2, wsz.height - 30 });

        addChild(col);
        addChild(title);

        CCScale9Sprite* level_frame = CCScale9Sprite::create("GJ_square07.png");
        level_frame->setContentSize({ 360, 200 });

        level_frame->setPosition({ wsz.width / 2, wsz.height / 2 });
        
        NPUiLevelList* list = NPUiLevelList::create();
        list->setPosition({ wsz.width / 2, wsz.height / 2 });

        static const auto author = u8"Гущин";

        GJGameLevel* leveltest = GJGameLevel::create();
        leveltest->m_levelName = "tesaaaaaaaaaaaaat";
        leveltest->m_creatorName = (const char*)author;
        leveltest->m_normalPercent = 50;
        leveltest->m_practicePercent = 90;
        leveltest->m_downloads = 50000;
        leveltest->m_likes = 10000000;
        leveltest->retain();

        NPUiLevelCell* cell = NPUiLevelCell::create(leveltest);

        list->addCell(cell);

        addChild(list);
        addChild(level_frame);

        return true;
    }
};

class $modify(NLevelBrowserLayer, LevelBrowserLayer) {
    void loadLevelsFinished(CCArray * p0, const char* p1, int p2) {
        LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);

        m_list->m_listView->m_tableView->sortAllChildren();
        CCContentLayer* list_entries_node = typeinfo_cast<CCContentLayer*>(m_list->m_listView->m_tableView->getChildren()->objectAtIndex(0));

        if (!list_entries_node) {
            log::error("CCContentLayer cannot be found");

            return;
        }
    }
};

class $modify(NMenuLayer, MenuLayer) {
    void onSoundTest(CCObject * sender) {
		/*SoundTestPopup::create();*/
        auto layer = NPUiSearch::create();
        addChild(layer, 1000);
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
