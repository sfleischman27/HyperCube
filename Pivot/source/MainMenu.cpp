//
//  MainMenu.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 3/26/23.
//

#include "MainMenu.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool MainMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // check if there is a save file
    std::string savePath = Application::get()->getSaveDirectory();
    savePath.append("save.json");
    savePath = filetool::normalize_path(savePath);
    
    // deletes save file for testing purposes
    //filetool::file_delete(savePath);
    // true if there is a save file
    _canResume = filetool::file_exists(savePath);
    //_canResume = false;
    
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
    _assets->loadDirectory("json/pivot_startUI.json");
    auto layer = assets->get<scene2::SceneNode>("startLab");
    layer->setContentSize(dimen);
    layer->doLayout();
    
    _start = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("startLab_startScreen_startB"));
    _start->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::START;
        }
    });
    
    
    _resume = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("startLab_startScreen_resumeB"));
    _resume->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::RESUME;
        }
    });
    
    addChild(layer);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MainMenu::dispose() {
    Scene2::dispose();
    _start = nullptr;
    _resume = nullptr;
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
void MainMenu::setActive(bool value) {
    Scene2::setActive(value);
    if (value) {
        _choice = NONE;
        if (_canResume){
            _resume->activate();
            _resume->setVisible(true);
            
            _start->setVisible(false);
            _start->deactivate();
            _start->setDown(false);
        } else {
            _start->activate();
            _start->setVisible(true);
            
            _resume->setVisible(false);
            _resume->deactivate();
            _resume->setDown(false);
        }
    } else {
        _start->deactivate();
        _resume->deactivate();
        // If any were pressed, reset them
        _start->setDown(false);
        _resume->setDown(false);
    }
}
