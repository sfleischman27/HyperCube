//
//  mainMenu.cpp
//  Pivot
//
//  Created by Sarah Fleischman on 3/26/23.
//

#include "mainMenu.h"

using namespace cugl;

#define SCENE_SIZE  1024;

bool mainMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    
    Size dimen = Size(0, 0);
    //Application::get()->getDisplaySize();
    
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
    
    
    return true;
}
