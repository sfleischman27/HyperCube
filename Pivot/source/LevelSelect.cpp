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
    
    _label = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("levelLab_levelselectScreen_panelNode_leveltitle_label"));
    
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

/**
 * The method that updates the menu to reflect unlocked levels
 */
void LevelSelect::updateLevel(int level) {
    if(level <= _maxLevel){
        // no update needed
        return;
    }
    _maxLevel = level;
    
    setActive(true);
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
    // maximum level number in the pack
    int maxPackNum = prePackNum(_pack) + inPackNum(_pack) - 1;
    // min level number in pack
    int minPackNum = prePackNum(_pack);
    
    for(auto it = _buttons.begin(); it != _buttons.end(); ++it){
        // get the button number
        int num = buttNameToNum(it->first);
        
        // if it is not a level button
        if (it->first == "levelprevious" | it->first == "levelnext" | it->first == "settings"){
            if (value) {
                it->second->activate();
            } else {
                it->second->deactivate();
                // reset if pressed
                it->second->setDown(false);
            }
        // if it is the unlocked button on this page that is actually unlocked
        } else if (num != -1 && num >= minPackNum && num <= maxPackNum && num <= _maxLevel){
            // if scene is active
            if (value) {
                it->second->activate();
                it->second->setVisible(true);
                _buttons[it->first + "Locked"]->setVisible(false);
            } else {
                it->second->deactivate();
                // reset if pressed
                it->second->setDown(false);
            }
        // if it is the locked button
        } else if (num == -1){
            it->second->deactivate();
            // reset if pressed
            it->second->setDown(false);
        // if it is an unlocked button on this page
        } else if (num >= minPackNum && num <= maxPackNum){
            // disable and hide unlocked
            it->second->deactivate();
            it->second->setDown(false);
            it->second->setVisible(false);
            // show locked
            _buttons[it->first + "Locked"]->setVisible(true);
        // if it is an unlocked button not on the page
        } else {
            // disable and hide unlocked
            it->second->deactivate();
            it->second->setDown(false);
            it->second->setVisible(false);
            // hide locked
            _buttons[it->first + "Locked"]->setVisible(false);
        }
    }
}

std::string LevelSelect::packToString(Pack pack){
    switch (pack) {
        case Pack::ZERO:
            return "tutorial";
        case Pack::ONE:
            return "lab";
        case Pack::TWO:
            return "tunnel";
        case Pack::THREE:
            return "woods";
        case Pack::FOUR:
            return "final";
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
    } else if (name == "level16"){
        return 15;
    } else if (name == "level17"){
        return 16;
    } else if (name == "level18"){
        return 17;
    } else if (name == "level19"){
        return 18;
    } else if (name == "level20"){
        return 19;
    } else {
        return -1;
    }
}

void LevelSelect::setChoice(std::string name){
    //TODO: integer.parsestring use char after 5
    if (name == "level1" | name == "level5" | name == "level9" | name == "level13" | name == "level17"){
        _choice = Choice::LEVEL1;
    } else if (name == "level2" | name == "level6" | name == "level10" | name == "level14" | name == "level18"){
        _choice = Choice::LEVEL2;
    } else if (name == "level3" | name == "level7" | name == "level11" | name == "level15" | name == "level19"){
        _choice = Choice::LEVEL3;
    } else if (name == "level4" | name == "level8" | name == "level12" | name == "level16" | name == "level20"){
        _choice = Choice::LEVEL4;
    } else if (name == "levelnext"){
        _choice = Choice::NEXT;
    } else if (name == "levelprevious"){
        _choice = Choice::PREV;
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
    return toLevelString(_choice, packToString(_pack));
}

std::string LevelSelect::getFirstInPackString(){
    _choice = LEVEL1;
    return toLevelString(_choice, packToString(_pack));
}

std::string LevelSelect::getNextLevelString(){
    nextLevel();
    return toLevelString(_choice, packToString(_pack));
}

void LevelSelect::nextLevel(){
    if( _choice == inPackNum(_pack) - 1){ // reached the end of the pack
        _choice = Choice::LEVEL1;
        _pack = static_cast<LevelSelect::Pack>(_pack + 1);
    } else {
        _choice = static_cast<LevelSelect::Choice>(_choice + 1);
    }
    updateMax(levelNum());
}

/**
 * Switches page to the next pack if possible
 */
void LevelSelect::nextPack() {
    if (_pack < PACKS_IMPLEMENTED - 1){
        _pack = static_cast<LevelSelect::Pack>(_pack + 1);
        setPackLabel();
        setActive(true);
    }
    _choice = NONE;
}

/**
 * Switches page to the previous pack if possible
 */
void LevelSelect::prevPack() {
    if (_pack != 0){
        _pack = static_cast<LevelSelect::Pack>(_pack - 1);
        setPackLabel();
        setActive(true);
    }
    _choice = NONE;
}

void LevelSelect::setPackLabel(){
    switch (_pack) {
        case ZERO:
            _label->setText("Tutorial Levels");
            break;
        case ONE:
            _label->setText("Laboratory Levels");
            break;
        case TWO:
            _label->setText("Underground Levels");
            break;
        case THREE:
            _label->setText("Wooded Swamp Levels");
            break;
        case FOUR:
            _label->setText("The Final Bunker");
            break;
    }
}

int LevelSelect::levelNum(){
    return prePackNum(_pack) + _choice;
}

/**
 * Returns the number of levels before the current pack
 */
int LevelSelect::prePackNum(Pack pack){
    return 4 * pack;
}

int LevelSelect::inPackNum(Pack pack){
    return 4;
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

bool LevelSelect::isLastInPack(){
    return _choice == inPackNum(_pack) - 1;
}
