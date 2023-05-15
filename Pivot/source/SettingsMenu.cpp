//
//  SettingsMenu.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 5/15/23.
//

#include "SettingsMenu.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool SettingsMenu::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<DataController>& data) {
    
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
    
    _data = data;
    _assets = assets;
    _assets->loadDirectory("json/pivot_settingsUI.json");
    auto layer = assets->get<scene2::SceneNode>("settingsLab");
    layer->setContentSize(dimen);
    layer->doLayout();
    
    
    _back = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_back"));
    _back->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::EXIT;
        }
    });
    
    _clear = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_clear"));
    _clear->addListener([this](const std::string& name, bool down) {
        if (down) {
            // switch into sub scene
            _choice = OVERLAY;
        }
    });
    
    _volume = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_volume_slider"));
    _volume->addListener([this](const std::string& name, float value) {
        if (value != _volumeValue){
            _volumeValue = value;
            _data->setVolume(value);
        }
    });
    
    _music = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_music_toggle"));
    _music->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setMusic(false);
        } else {
            _data->setMusic(true);
        }
    });
    
    _movement = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_movement_toggle"));
    _movement->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setControls(false);
        } else {
            _data->setControls(true);
        }
    });
    
    _rotation = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_rotation_toggle"));
    _rotation->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setRotate(false);
        } else {
            _data->setRotate(true);
        }
    });
    
    _compass = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_compass_toggle"));
    _compass->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setCompass(false);
        } else {
            _data->setCompass(true);
        }
    });
    
    _overlay = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_overlay"));
    _overlay->setVisible(false);
    
    _yes = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_overlay_yes"));
    _yes->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->resetSave();
            // switch out of sub scene
            _choice = OVEROFF;
            //TODO: show that data has been cleared
        }
    });
    
    _no = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_overlay_no"));
    _no->addListener([this](const std::string& name, bool down) {
        if (down) {
            // switch out of sub scene
            _choice = OVEROFF;
        }
    });
    
    // TODO: use the _save in data to setup the buttons to proper settings
    
    
    addChild(layer);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void SettingsMenu::dispose() {
    Scene2::dispose();
    _assets = nullptr;
    _data = nullptr;
    _back = nullptr;
    _clear = nullptr;
    _volume = nullptr;
    _music = nullptr;
    _movement = nullptr;
    _rotation = nullptr;
    _compass = nullptr;
    _overlay = nullptr;
    _yes = nullptr;
    _no = nullptr;
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
void SettingsMenu::setActive(bool value) {
    Scene2::setActive(value);
    if (value) {
        _choice = NONE;
        _back->activate();
        _clear->activate();
        _volume->activate();
        _music->activate();
        _movement->activate();
        _rotation->activate();
        _compass->activate();
    } else {
        _back->deactivate();
        _clear->deactivate();
        _volume->deactivate();
        _music->deactivate();
        _movement->deactivate();
        _rotation->deactivate();
        _compass->deactivate();
        // if back or clear were pressed, reset them
        _back->setDown(false);
        _clear->setDown(false);
    }
}

/**
 * Sets whether the overlay scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void SettingsMenu::setOverlayActive(bool value) {
    if (value) {
        setActive(false);
        
        _overlay->setVisible(true);
        _yes->activate();
        _no->activate();
    } else {
        setActive(true);
        
        _overlay->setVisible(false);
        _yes->deactivate();
        _no->deactivate();
        // if buttons were pressed, reset them
        _yes->setDown(false);
        _no->setDown(false);
    }
}

