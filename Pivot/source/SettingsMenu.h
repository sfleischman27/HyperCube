//
//  SettingsMenu.h
//  Pivot
//
//  Created by Sarah Fleischman on 5/15/23.
//

#ifndef SettingsMenu_h
#define SettingsMenu_h
#include <cugl/cugl.h>
#include "DataController.h"

class SettingsMenu : public cugl::Scene2 {
public:
    enum Choice {
        /** User has not made a choice */
        NONE,
        /** User wants to exit the menu */
        EXIT,
        /** User has triggered the overlay */
        OVERLAY,
        /** User is exiting the overlay */
        OVEROFF
    };
    
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The data controller */
    std::shared_ptr<DataController> _data;
    
    /** The back button */
    std::shared_ptr<cugl::scene2::Button> _back;
    /** The clear data button */
    std::shared_ptr<cugl::scene2::Button> _clear;
    /** The volume slider */
    std::shared_ptr<cugl::scene2::Slider> _volume;
    /** The volume label */
    std::shared_ptr<cugl::scene2::Label> _volumeLabel;
    /** The value of the volume */
    float _volumeValue;
    /** The music toggle */
    std::shared_ptr<cugl::scene2::Button> _music;
    /** The music label */
    std::shared_ptr<cugl::scene2::Label> _musicLabel;
    /** The movement toggle */
    std::shared_ptr<cugl::scene2::Button> _movement;
    /** The movement label */
    std::shared_ptr<cugl::scene2::Label> _movementLabel;
    /** The rotation toggle */
    std::shared_ptr<cugl::scene2::Button> _rotation;
    /** The rotation label */
    std::shared_ptr<cugl::scene2::Label> _rotationLabel;
    /** The compass toggle */
    std::shared_ptr<cugl::scene2::Button> _compass;
    /** The compass label */
    std::shared_ptr<cugl::scene2::Label> _compassLabel;
    /** The clear data overlay */
    std::shared_ptr<cugl::scene2::SceneNode> _overlay;
    /** The yes clear the data button */
    std::shared_ptr<cugl::scene2::Button> _yes;
    /** The no don't clear the data button */
    std::shared_ptr<cugl::scene2::Button> _no;
    
    /** The player menu choice */
    Choice _choice;
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    SettingsMenu() : Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~SettingsMenu() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param data        The data controller for the game
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<DataController>& data);
    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) {}
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    /**
     * Sets whether the overlay scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    void setOverlayActive(bool value);
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }
    
   /**
     * Updates the buttons or joystick choice
     *
     * true = buttons, false = joystick
     */
    void setControls(bool buttons) { _movement->setDown(!buttons); }
    
    /**
     * Updates the volume in the save json
     */
    void setVolume(float volume) { _volume->setValue(volume); }
    
    /**
     * Updates the music choice in the save json
     *
     * true = on, false = off
     */
    void setMusic(bool music) { _music->setDown(!music); }
    
    /**
     * Updates the compass increment selection in the save json
     *
     * true = 180, false = 360
     */
    void setCompass(bool compass) { _compass->setDown(!compass); }
    
    /**
     * Updates the rotate selction in the save json
     */
    void setRotate(bool rotate) { _rotation->setDown(!rotate); }
    
};


#endif /* SettingsMenu_h */
