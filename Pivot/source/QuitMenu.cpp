//
//  QuitMenu.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 3/28/23.
//

#include "QuitMenu.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool QuitMenu::init(const std::shared_ptr<cugl::AssetManager> &assets) {
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
    _assets->loadDirectory("json/pivot_quitUI.json");
    auto layer = assets->get<scene2::SceneNode>("quitLab");
    layer->setContentSize(dimen);
    layer->doLayout();
    
    _yes = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("quitLab_quitScreen_boardBase_board_yes"));
    _no = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("quitLab_quitScreen_boardBase_board_no"));
    
    _yes->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::Y;
        }
    });
    _no->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::N;
        }
    });
    
    addChild(layer);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void QuitMenu::dispose() {
    Scene2::dispose();
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
void QuitMenu::setActive(bool value){
    Scene2::setActive(value);
    if (value) {
        _choice = NONE;
        _yes->activate();
        _no->activate();
    } else {
        _yes->deactivate();
        _no->deactivate();
        // If any were pressed, reset them
        _yes->setDown(false);
        _no->setDown(true);
    }
}
