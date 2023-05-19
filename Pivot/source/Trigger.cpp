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
        //CULog(arg.c_str());
    }
};

//** trigger the player to die when they enter this zone*/
void Trigger::killPlayer(TriggerArgs args) {
    args.player->setDead(true);
};

//** trigger the rotate tutorial to turn on */
void Trigger::showPopup(TriggerArgs args) {
    if (args.image == "rotate_tutorial"){
        args.popup->setState("rotate");
    } else if (args.image == "collect_tutorial"){
        args.popup->setState("collect");
    } else if (args.image == "glow_tutorial"){
        args.popup->setState("glow");
    } else if (args.image == "jump_tutorial"){
        args.popup->setState("jump");
    } else if (args.image == "run_tutorial"){
        args.popup->setState("run");
    } else if (args.image == "walk_tutorial"){
        args.popup->setState("walk");
    }
};

void Trigger::showMessage(TriggerArgs args) {
    args.messages->setState(args.text);
}

void Trigger::showExitMess(TriggerArgs args){
    args.messages->setExit(args.text);
}

//** trigger to turn off popups */
void Trigger::stopPopups(TriggerArgs args) {
    args.popup->setState("none");
}

void Trigger::stopMessages(TriggerArgs args){
    args.messages->clear();
}
