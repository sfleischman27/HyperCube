//
//  Trigger.h
//  Platformer
//
//  Trigger regions for checking players location and doing callbacks
//
//  Created by Jack Otto on 4/26/23.
//

#ifndef Trigger_h
#define Trigger_h
#include <cugl/cugl.h>
# include "Mesh.h"
# include "PlayerModel.h"


struct Popups {
    enum State {
        /** No popup is currently active */
        NONE,
        /** Rotate graphic is currently active */
        ROTATE,
        /** Collect graphic is currently active */
        COLLECT,
        /** Glow graphic is currently active */
        GLOW,
        /** Jump graphic is currently active */
        JUMP,
        /** Running graphic is currently active */
        RUN,
        /** Walking graphic is currenly active */
        WALK
    };
    
    State _state;
    
    Popups(){ _state = NONE; }
    
    void setState(std::string state) {
        if(state == "none"){
            _state = NONE;
        } else if(state == "rotate"){
            _state = ROTATE;
        } else if(state == "collect"){
            _state = COLLECT;
        } else if(state == "glow"){
            _state = GLOW;
        } else if(state == "jump"){
            _state = JUMP;
        } else if(state == "run"){
            _state = RUN;
        } else if(state == "walk"){
            _state = WALK;
        }
    }
    
    State getState() { return _state; }
    
    void clear() { _state = NONE; }
};

struct TriggerArgs {
    std::vector<std::string> strings;
    std::shared_ptr<PlayerModel> player;
    std::shared_ptr<Popups> popup;
    std::string image;
    std::string message;
};

class Trigger {

public:

    int state;

private:
    std::vector<std::pair<std::function<void(TriggerArgs)>, TriggerArgs>> _onEnterCallbacks;
    std::vector<std::pair<std::function<void(TriggerArgs)>, TriggerArgs>> _onExitCallbacks;
    std::vector<std::pair<std::function<void(TriggerArgs)>, TriggerArgs>> _inBoundsCallbacks;

    std::shared_ptr<PivotMesh> _mesh;

public:
    /**construct a trigger object from a mesh*/
    Trigger(std::shared_ptr<PivotMesh> mesh) {
        _mesh = mesh;
        state = 0;
    }


    /**add a callback to run when the player enters the trigger region*/
    void registerEnterCallback(std::function<void(TriggerArgs)> cb, TriggerArgs args) {
        _onEnterCallbacks.push_back(std::pair<std::function<void(TriggerArgs)>, TriggerArgs>(cb,args));
    }

    /**add a callback to run when the player exits the trigger region*/
    void registerExitCallback(std::function<void(TriggerArgs)> cb, TriggerArgs args) {
        _onExitCallbacks.push_back(std::pair<std::function<void(TriggerArgs)>, TriggerArgs>(cb, args));
    }

    /**add a callback to run when the player remains inside the trigger region
    triggered every frame after the first frame the player enters the region
    */
    void registerInBoundsCallback(std::function<void(TriggerArgs)> cb, TriggerArgs args) {
        _inBoundsCallbacks.push_back(std::pair<std::function<void(TriggerArgs)>, TriggerArgs>(cb, args));
    }

    /**do the callbacks associated with this trigger region*/
    void doEnterCallbacks() {
        for (const auto& cbp : _onEnterCallbacks) {
            cbp.first(cbp.second);
        }
    }

    /**do the callbacks associated with this trigger region*/
    void doExitCallbacks() {
        for (const auto& cbp : _onExitCallbacks) {
            cbp.first(cbp.second);
        }
    }

    /**do the callbacks associated with this trigger region*/
    void doInBoundsCallbacks() {
        for (const auto& cbp : _inBoundsCallbacks) {
            cbp.first(cbp.second);
        }
    }

    /**updates the trigger state according to the new location
    if there is a state change the corresponding callbacks will be made
    */
    bool update(cugl::Vec3 location);

    //-----Static methods used as callbacks-----//

    //** print a string to terminal, useful for checking if callbacks are working*/
    static void speak(TriggerArgs args);

    //** trigger the player to die when they enter this zone*/
    static void killPlayer(TriggerArgs args);

    //** trigger the popup to turn on */
    static void showPopup(TriggerArgs args);

    //** trigger a message to appear */
    static void showMessage(TriggerArgs args);
    
    //** trigger to turn off popups */
    static void stopPopups(TriggerArgs args);
};


#endif /* Trigger_h */
