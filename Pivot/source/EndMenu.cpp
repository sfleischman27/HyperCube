//
//  EndMenu.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 3/27/23.
//

#include "EndMenu.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool EndMenu::init(const std::shared_ptr<cugl::AssetManager> &assets) {
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
    _assets->loadDirectory("json/pivot_endUI.json");
    auto layer = assets->get<scene2::SceneNode>("endLab");
    layer->setContentSize(dimen);
    layer->doLayout();

    _replay = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("endLab_endScreen_board_replay"));
    _level = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("endLab_endScreen_board_level"));
    _next = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("endLab_endScreen_board_next"));
    
    
    _replay->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::REPLAY;
        }
    });
    _level->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::LEVEL;
        }
    });
    _next->addListener([this](const std::string& name, bool down) {
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
void EndMenu::dispose() {
    Scene2::dispose();
    _replay = nullptr;
    _level = nullptr;
    _next = nullptr;
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
void EndMenu::setActive(bool value){
    Scene2::setActive(value);
    if (value) {
        _choice = NONE;
        _level->activate();
        _next->activate();
        _replay->activate();
    } else {
        _level->deactivate();
        _next->deactivate();
        _replay->deactivate();
        // If any were pressed, reset them
        _level->setDown(false);
        _next->setDown(false);
        _replay->setDown(false);
    }
}

