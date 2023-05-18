//
//  Cutscene.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 5/18/23.
//

#include "Cutscene.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024


bool Cutscene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
    _choice = NONE;
    
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
    _assets->loadDirectory("json/pivot_cutsceneUI.json");
    auto layer = assets->get<scene2::SceneNode>("cutsceneLab");
    layer->setContentSize(dimen);
    layer->doLayout();
    
    _story1 = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("cutsceneLab_cutsceneScreen_cutscene1"));
    _story2 = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("cutsceneLab_cutsceneScreen_cutscene2"));
    _story3 = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("cutsceneLab_cutsceneScreen_cutscene3"));
    _story4 = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("cutsceneLab_cutsceneScreen_cutscene4"));
    
    _butt = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("cutsceneLab_cutsceneScreen_nextscene"));
    
    _butt->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::NEXT;
        }
    });
    
    addChild(layer);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void Cutscene::dispose() {
    Scene2::dispose();
    _story1 = nullptr;
    _story2 = nullptr;
    _story3 = nullptr;
    _story4 = nullptr;
    _butt = nullptr;
    _assets = nullptr;
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
void Cutscene::setActive(bool value){
    Scene2::setActive(value);
    if (value) {
        _choice = STORY1;
    } else {
        resetStory();
        _butt->deactivate();
        // reset if butt was pressed
        _butt->setDown(false);
    }
}

void Cutscene::resetStory(){
    // set alphas to 0.0 so elements can fade in
    auto color = _story1->getColor();
    auto newColor = Color4(color.r, color.g, color.b, 0.0);
    _story1->setColor(newColor);
    
    color = _story2->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _story2->setColor(newColor);
    
    color = _story3->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _story3->setColor(newColor);
    
    color = _story4->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _story4->setColor(newColor);
    
    color = _butt->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _butt->setColor(newColor);
    
    // set all to not visible
    _story1->setVisible(false);
    _story2->setVisible(false);
    _story3->setVisible(false);
    _story4->setVisible(false);
    _butt->setVisible(false);
}

void Cutscene::update(float timestep){
    switch (_choice) {
        case NONE: case NEXT:
            break;
        case STORY1: {
            _story1->setVisible(true);
            auto color = _story1->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
            _story1->setColor(newColor);
            if (newColor.a == 255){ _choice = STORY2; }
            break; }
        case STORY2:{
            _story2->setVisible(true);
            auto color = _story2->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
            _story2->setColor(newColor);
            if (newColor.a == 255){ _choice = STORY3; }
            break; }
        case STORY3:{
            _story3->setVisible(true);
            auto color = _story3->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
            _story3->setColor(newColor);
            if (newColor.a == 255){ _choice = STORY4; }
            break; }
        case STORY4:{
            _story4->setVisible(true);
            auto color = _story4->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
            _story4->setColor(newColor);
            if (newColor.a == 255){ _choice = NONE; }
            break; }
    }
}
