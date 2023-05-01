//
//  Trigger.cpp
//  Platformer
//
//  Trigger regions for checking players location and doing callbacks
//
//  Created by Jack Otto on 4/26/23.
//

#include <cugl/cugl.h>
# include "Trigger.h"
# include "Mesh.h"

using namespace cugl;

bool Trigger::update(cugl::Vec3 location){
        
    auto inside = _mesh->containsPoint(location);

    if (state == 0) {
        if (inside) {
            //CULog("Entered Trigger");
            state = 1;
            doEnterCallbacks();
        }
    }
    else {
        if (inside) {
            //CULog("Still Inside");
            doInBoundsCallbacks();
        }
        else {
            //CULog("Exited Trigger");
            state = 0;
            doExitCallbacks();
        }
    }
    return inside;
};

//** print a string to terminal, useful for checking if callbacks are working*/
void Trigger::speak(TriggerArgs args) {
    for(auto arg : args.strings) {
        CULog(arg.c_str());
    }
};

//** trigger the player to die when they enter this zone*/
void Trigger::killPlayer(TriggerArgs args) {
    args.player->setDead(true);
};