//
//  Credits.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 5/18/23.
//

#include "Credits.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool CreditsScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
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
    _assets->loadDirectory("json/pivot_creditsUI.json");
    auto layer = assets->get<scene2::SceneNode>("creditsLab");
    layer->setContentSize(dimen);
    layer->doLayout();
    
    _image = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("creditsLab_creditsScreen"));
    
    _butt = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("creditsLab_creditsScreen_creditsexit"));
    
    _butt->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::EXIT;
        }
    });
    
    addChild(layer);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void CreditsScene::dispose() {
    Scene2::dispose();
    _image = nullptr;
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
void CreditsScene::setActive(bool value){
    Scene2::setActive(value);
    if (value) {
        _choice = NONE;
        // set alpha to 0.0 and set visible
        auto color = _image->getColor();
        auto newColor = Color4(color.r, color.g, color.b, 0.0);
        _image->setColor(newColor);
        
        _image->setVisible(true);
        
        _playMusic = true;
    } else {
        _image->setVisible(false);
        _butt->deactivate();
        // reset if butt was pressed
        _butt->setDown(false);
    }
    
    
}

void CreditsScene::update(float timestep){
    auto color = _image->getColor();
    auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 2, 255));
    _image->setColor(newColor);
    if (newColor.a >= 255){
        _butt->activate();
    }
}
