//
//  LevelSelect.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 3/26/23.
//

#include "LevelSelect.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool LevelSelect::init(const std::shared_ptr<cugl::AssetManager> &assets, int level) {
    
    _choice = NONE;
    _maxLevel = level;
    
    Size dimen = Application::get()->getDisplaySize();
    
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    _assets = assets;
    _assets->loadDirectory("json/pivot_levelUI.json");
    auto layer = assets->get<scene2::SceneNode>("levelLab");
    layer->setContentSize(dimen);
    layer->doLayout();
    
    auto panel = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("levelLab_levelselectScreen_panelNode"));
    
    auto levels = panel->getChildren()[0]->getChildren();
    for(auto it = levels.begin(); it != levels.end(); ++it){
        std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(*it);
        _buttons[butt->getName()] = butt;
        butt->addListener([this](const std::string& name, bool down) {
            if (down) {
                setChoice(name);
            }
        });
    }
    
//    auto test = _assets->get<JsonValue>("pivot_levelUI");
//    test->get("scene2s")->get("levelLab")->get("type")->set("Widget");
   
    addChild(layer);
    setActive(false);
    return true;
}

void LevelSelect::dispose() {
    Scene2::dispose();
    _buttons.clear();
    _assets = nullptr;
}

void LevelSelect::updateLevel(int level) {
    _maxLevel = level;
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        int num = nameToNum(it->first);
        if (num <= level && num != -1){ // unlocked and unlocked button
            it->second->activate();
            _buttons[it->first + "Locked"]->setVisible(false);
        } else if (num == -1){ // locked button
            it->second->deactivate();
        } else{ // locked and unlocked button
            it->second->deactivate();
            _buttons[it->first + "Locked"]->setVisible(true);
        }
    }
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void LevelSelect::setActive(bool value){
    Scene2::setActive(value);
    if (value) { _choice = NONE; }
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        int num = nameToNum(it->first);
        if (num <= _maxLevel && value && num != -1){ // unlocked and unlocked button
            it->second->activate();
            _buttons[it->first + "Locked"]->setVisible(false);
        } else if (num == -1){ // locked button
            it->second->deactivate();
            it->second->setDown(false);
        } else { // locked and unlocked button
            it->second->deactivate();
            it->second->setDown(false);
            _buttons[it->first + "Locked"]->setVisible(true);
        }
    }
}

std::string LevelSelect::getPackString(){
    switch (_pack) {
        case Pack::debug:
            return "debug";
        case Pack::test:
            return "test";
    }
}

int LevelSelect::nameToNum(std::string name){
    if (name == "level1"){
        return 1;
    } else if (name == "level2"){
        return 2;
    } else if (name == "level3"){
        return 3;
    } else if (name == "level4"){
        return 4;
    } else if (name == "level5"){
        return 5;
    } else if (name == "level6"){
        return 6;
    } else if (name == "level7"){
        return 7;
    } else if (name == "level8"){
        return 8;
    } else if (name == "level9"){
        return 9;
    } else if (name == "level10"){
        return 10;
    } else if (name == "level11"){
        return 11;
    } else if (name == "level12"){
        return 12;
    } else if (name == "level13"){
        return 13;
    } else if (name == "level14"){
        return 14;
    } else if (name == "level15"){
        return 15;
    } else {
        return -1;
    }
}

void LevelSelect::setChoice(std::string name){
    _choice = static_cast<Choice>(nameToNum(name));
}

std::string LevelSelect::numToName(int num){
    return "level" + toString(num);
}

