#include "_main.hpp"

#include <Geode/modify/MenuGameLayer.hpp>
class $modify(MenuGameLayerColors, MenuGameLayer) {
    $override cocos2d::ccColor3B getBGColor(int p0) {
        return GameToolbox::transformColor(MenuGameLayer::getBGColor(p0), cchsv(1, 0.3, 0.6, 0, 0));
    }
};