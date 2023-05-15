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
    _pack = ZERO;
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
    if(level <= _maxLevel){
        // no update needed
        return;
    }
    _maxLevel = level;
    
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        int num = buttNameToNum(it->first);
        if (it->first == "levelprevious" | it->first == "levelnext" | it->first == "settings"){ // non level butts
            it->second->activate();
        } else if (num <= _maxLevel && num != -1){
            // unlocked and unlocked button
            it->second->activate();
            _buttons[it->first + "Locked"]->setVisible(false);
            _buttons[it->first]->setVisible(true);
        } else if (num == -1){ // locked button
            it->second->deactivate();
        } else{ // locked and unlocked button
            it->second->deactivate();
            _buttons[it->first + "Locked"]->setVisible(true);
            _buttons[it->first]->setVisible(false);
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
    if (value) {
        _playMusic = true;
        _choice = NONE;
    }
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        int num = buttNameToNum(it->first);
        if (it->first == "levelprevious" | it->first == "levelnext" | it->first == "settings"){ // non level butts
            if (value) {
                it->second->activate();
            } else {
                it->second->deactivate();
                // reset if pressed
                it->second->setDown(false);
            }
        } else if (num <= _maxLevel && value && num != -1){
            // unlocked and unlocked button
            it->second->activate();
            _buttons[it->first + "Locked"]->setVisible(false);
            _buttons[it->first]->setVisible(true);
        } else if (num == -1){ // locked button
            it->second->deactivate();
            it->second->setDown(false);
        } else { // locked and unlocked button
            it->second->deactivate();
            it->second->setDown(false);
            _buttons[it->first + "Locked"]->setVisible(true);
            _buttons[it->first]->setVisible(false);
        }
    }
}
//TODO: change this and update to take pack into account

std::string LevelSelect::packToString(Pack pack){
    switch (pack) {
        case Pack::ZERO:
            return "tutorial";
        case Pack::ONE:
            return "debug";
    }
}

int LevelSelect::buttNameToNum(std::string name){
    if (name == "level1"){
        return 0;
    } else if (name == "level2"){
        return 1;
    } else if (name == "level3"){
        return 2;
    } else if (name == "level4"){
        return 3;
    } else if (name == "level5"){
        return 4;
    } else if (name == "level6"){
        return 5;
    } else if (name == "level7"){
        return 6;
    } else if (name == "level8"){
        return 7;
    } else if (name == "level9"){
        return 8;
    } else if (name == "level10"){
        return 9;
    } else if (name == "level11"){
        return 10;
    } else if (name == "level12"){
        return 11;
    } else if (name == "level13"){
        return 12;
    } else if (name == "level14"){
        return 13;
    } else if (name == "level15"){
        return 14;
    } else {
        return -1;
    }
}

std::string LevelSelect::numToName(int level){
    int pack = 0;
    while (level >= 15){
        level -= 15;
        pack += 1;
    }
    std::string packStr = packToString(static_cast<Pack>(pack));
    return toLevelString(level, packStr);
}

void LevelSelect::setChoice(std::string name){
    //TODO: integer.parsestring use char after 5
    if (name == "level1"){
        _choice = Choice::LEVEL1;
    } else if (name == "level2"){
        _choice = Choice::LEVEL2;
    } else if (name == "level3"){
        _choice = Choice::LEVEL3;
    } else if (name == "level4"){
        _choice = Choice::LEVEL4;
    } else if (name == "level5"){
        _choice = Choice::LEVEL5;
    } else if (name == "level6"){
        _choice = Choice::LEVEL6;
    } else if (name == "level7"){
        _choice = Choice::LEVEL7;
    } else if (name == "level8"){
        _choice = Choice::LEVEL8;
    } else if (name == "level9"){
        _choice = Choice::LEVEL9;
    } else if (name == "level10"){
        _choice = Choice::LEVEL10;
    } else if (name == "level11"){
        _choice = Choice::LEVEL11;
    } else if (name == "level12"){
        _choice = Choice::LEVEL12;
    } else if (name == "level13"){
        _choice = Choice::LEVEL13;
    } else if (name == "level14"){
        _choice = Choice::LEVEL14;
    } else if (name == "level15"){
        _choice = Choice::LEVEL15;
    } else if (name == "levelnext"){
        //TODO: REMOVE THIS TEMP CODEEEEE
        _choice = Choice::LEVEL10;
        //_choice = Choice::NEXT;
    } else if (name == "levelprevious"){
        //TODO: REMOVE THIS TEMP CODEEEEE
        _choice = Choice::LEVEL6;
        //_choice = Choice::PREV;
    } else if (name == "settings"){
        _choice = Choice::SETTINGS;
    } else{
        _choice = Choice::NONE;
    }
}

std::string LevelSelect::toLevelString(int level, std::string pack){
    if (level < 10){
        return pack + "_000" + std::to_string(level);
    } else{
        return pack + "_00" + std::to_string(level);
    }
}

std::string LevelSelect::getLevelString(){
    // TODO: fix once things are renamed
    if(_choice < 3){ // intro levels
        _pack = static_cast<LevelSelect::Pack>(0);
        return toLevelString(_choice, packToString(_pack));
    } else { // debug levels
        _choice = static_cast<LevelSelect::Choice>(_choice - 3);
        _pack = static_cast<LevelSelect::Pack>(1);
        return toLevelString(_choice, packToString(_pack));
    }
    
    //return toLevelString(_choice, packToString(_pack));
}

std::string LevelSelect::getNextLevelString(){
    nextLevel();
    return toLevelString(_choice, packToString(_pack));
}

void LevelSelect::nextLevel(){
    // TODO: fix once things are renamed
    if(_pack == 0 && _choice == 2){ // end of tutorial
        _choice = Choice::LEVEL1;
        _pack = static_cast<LevelSelect::Pack>(_pack + 1);
    } else { // in debug pack
        _choice = static_cast<LevelSelect::Choice>(_choice + 1);
    }
    updateMax(levelNum());
    
    /*
    int level = _choice;
   
    if (level == 14){
        level = 0;
        _choice = Choice::LEVEL1;
        _pack = static_cast<LevelSelect::Pack>(_pack + 1);
    } else{
        level +=1;
        _choice = static_cast<LevelSelect::Choice>(_choice + 1);
    }
    // update maxLevel if needed
    updateMax(levelNum());
     */
}

int LevelSelect::levelNum(){
    // TODO: fix once things are renamed
    if( _pack == 0){
        return _choice;
    } else {
        return _choice + 3;
    }
    //return (_pack * 15) + _choice;
}

void LevelSelect::updateMax(int level){
    if(level > _maxLevel && level <= (LEVELS_IMPLEMENTED - 1)){
        _maxLevel = level;
    }
}

void LevelSelect::unlockNextLevel(){
    int level = levelNum() + 1;
    updateMax(level);
}

bool LevelSelect::isLast(){
    return levelNum() == LEVELS_IMPLEMENTED - 1;
}
