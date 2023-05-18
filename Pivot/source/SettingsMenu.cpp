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
    
    _volumeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_volume_value"));
    _volume = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_volume_slider"));
    _volume->addListener([this](const std::string& name, float value) {
        if (value != _volumeValue){
            _volumeValue = value;
            _data->setVolume(value);
            _volumeLabel->setText(cugl::strtool::to_string(value,1));
        }
    });
    
    _musicLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_music_value"));
    _music = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_music_toggle"));
    _music->setToggle(true);
    _music->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setMusic(false);
            _musicLabel->setText("Off");
        } else {
            _data->setMusic(true);
            _musicLabel->setText("On");
        }
    });
    
    _movementLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_movement_value"));
    _movement = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_movement_toggle"));
    _movement->setToggle(true);
    _movement->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setControls(false);
            _movementLabel->setText("Joystick");
        } else {
            _data->setControls(true);
            _movementLabel->setText("Buttons");
        }
    });
    
    _rotationLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_rotation_value"));
    _rotation = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_rotation_toggle"));
    _rotation->setToggle(true);
    _rotation->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setRotate(true);
            _rotationLabel->setText("Counter Clockwise");
        } else {
            _data->setRotate(false);
            _rotationLabel->setText("Clockwise");
        }
    });
    
    _compassLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_compass_value"));
    _compass = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_compass_toggle"));
    _compass->setToggle(true);
    _compass->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setCompass(false);
            _compassLabel->setText("360");
        } else {
            _data->setCompass(true);
            _compassLabel->setText("180");
        }
    });
    
    _outlineLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_outline_value"));
    _outline = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_outline_toggle"));
    _outline->setToggle(true);
    _outline->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setOutline(false);
            _outlineLabel->setText("Off");
        } else {
            _data->setOutline(true);
            _outlineLabel->setText("On");
        }
    });
    
    _rightHandLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_control_value"));
    _rightHand = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_control_toggle"));
    _rightHand->setToggle(true);
    _rightHand->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->setRightHand(false);
            _rightHandLabel->setText("Left Hand");
        } else {
            _data->setRightHand(true);
            _rightHandLabel->setText("Right Hand");
        }
    });
    
    _credits = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_credits"));
    _credits->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = CREDITS;
        }
    });
    
    _overlay = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_overlay"));
    _overlay->setVisible(false);
    
    _yes = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_overlay_yes"));
    _yes->addListener([this](const std::string& name, bool down) {
        if (down) {
            _data->resetSave();
            setFromSave();
            // switch out of sub scene
            _choice = OVEROFF;
        }
    });
    
    _no = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settingsLab_settingsScreen_baseNode_base_overlay_no"));
    _no->addListener([this](const std::string& name, bool down) {
        if (down) {
            // switch out of sub scene
            _choice = OVEROFF;
        }
    });
    
    setFromSave();
    
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
    _outline = nullptr;
    _rightHand = nullptr;
    _credits = nullptr;
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
        _outline->activate();
        _rightHand->activate();
        _credits->activate();
    } else {
        _back->deactivate();
        _clear->deactivate();
        _volume->deactivate();
        _music->deactivate();
        _movement->deactivate();
        _rotation->deactivate();
        _compass->deactivate();
        _outline->deactivate();
        _rightHand->deactivate();
        _credits->deactivate();
        // if back or clear were pressed, reset them
        _back->setDown(false);
        _clear->setDown(false);
        _credits->setDown(false);
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

/**
 * Uses the save data to setup the scene
 */
void SettingsMenu::setFromSave() {
    if (_data->getMusic()) {
        _music->setDown(false);
        _musicLabel->setText("On");
    } else {
        _music->setDown(true);
        _musicLabel->setText("Off");
    }
    
    if (_data->getControls()) {
        _movement->setDown(false);
        _movementLabel->setText("Buttons");
    } else {
        _movement->setDown(true);
        _movementLabel->setText("Joystick");
    }
    
    if (_data->getRotate()) {
        _rotation->setDown(true);
        _rotationLabel->setText("Counter Clockwise");
    } else {
        _rotation->setDown(false);
        _rotationLabel->setText("Clockwise");
    }
    
    if (_data->getCompass()) {
        _compass->setDown(false);
        _compassLabel->setText("180");
    } else {
        _compass->setDown(true);
        _compassLabel->setText("360");
    }
    
    if (_data->getOutline()) {
        _outline->setDown(false);
        _outlineLabel->setText("On");
    } else {
        _outline->setDown(true);
        _outlineLabel->setText("Off");
    }
    
    if (_data->getRightHand()) {
        _rightHand->setDown(false);
        _rightHandLabel->setText("Right Hand");
    } else {
        _rightHand->setDown(true);
        _rightHandLabel->setText("Left Hand");
    }
    
    _volume->setValue(_data->getVolume());
    _volumeLabel->setText(cugl::strtool::to_string(_data->getVolume(), 1));
}


//Note: default -> up setDown(false) = true (setting) = on (label)
