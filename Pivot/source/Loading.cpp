//
//  Loading.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 5/16/23.
//

#include "Loading.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

bool Loading::init(const std::shared_ptr<cugl::AssetManager> &assets){
    _status = NONE;
    
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
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<scene2::SceneNode>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    
    addChild(layer);
    return true;
}

void Loading::dispose() {
    _bar = nullptr;
    _assets = nullptr;
    _progress = 0.0f;
}

#pragma mark -
#pragma mark Progress Monitoring

/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void Loading::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            _progress = 1.0f;
            _bar->setVisible(false);
            _status = LOADED;
        } else {
            _bar->setProgress(_progress);
        }
    }
//    if (_status == LOADED) {
//        this->_active = false;
//
//    }
}
