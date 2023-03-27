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
    
    _background = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("levelLab_levelselectScreen"));
    _panel = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("levelLab_levelselectScreen_panelNode"));
    
    auto levels = _panel->getChildren()[0]->getChildren();
    for(auto it = levels.begin(); it != levels.end(); ++it){
        std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(*it);
        _buttons[butt->getName()] = butt;
        butt->addListener([this](const std::string& name, bool down) {
            if (down) {
                setChoice(name);
            }
        });
    }
    
    addChild(layer);
    return true;
}

void LevelSelect::dispose() {
    _background = nullptr;
    _panel = nullptr;
    _buttons.clear();
    _assets = nullptr;
    
}

void LevelSelect::updateLevel(int level) {
    _maxLevel = level;
    int num = 1;
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        // TODO: figure out how to change the level state fo the widget from locked to unlocked
        if (num <= level){
            it->second->activate();
        } else{
            it->second->deactivate();
        }
        num += 1;
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
    int num = 1;
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        // TODO: figure out how to change the level state fo the widget from locked to unlocked
        if (num <= _maxLevel and value){
            it->second->activate();
        } else{
            it->second->deactivate();
        }
        num += 1;
    }
}

void LevelSelect::setChoice(std::string name){
    if (name == "level1"){
        _choice = Choice::level1;
    } else if (name == "level2"){
        _choice = Choice::level2;
    } else if (name == "level3"){
        _choice = Choice::level3;
    } else if (name == "level4"){
        _choice = Choice::level4;
    } else if (name == "level5"){
        _choice = Choice::level5;
    } else if (name == "level6"){
        _choice = Choice::level6;
    } else if (name == "level7"){
        _choice = Choice::level7;
    } else if (name == "level8"){
        _choice = Choice::level8;
    } else if (name == "level9"){
        _choice = Choice::level9;
    } else if (name == "level10"){
        _choice = Choice::level10;
    } else if (name == "level11"){
        _choice = Choice::level11;
    } else if (name == "level12"){
        _choice = Choice::level12;
    } else if (name == "level13"){
        _choice = Choice::level13;
    } else if (name == "level14"){
        _choice = Choice::level14;
    } else if (name == "level15"){
        _choice = Choice::level15;
    }
}
