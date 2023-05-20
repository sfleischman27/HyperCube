//
//  EndMenu.h
//  Pivot
//
//  Created by Sarah Fleischman on 3/27/23.
//

#ifndef EndMenu_h
#define EndMenu_h
#include <cugl/cugl.h>

class EndLevelMenu : public cugl::Scene2 {
public:
    // whether to play this background music
    bool _playMusic = false;
    
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to see the map */
        MAP,
        /** User wants to replay the level */
        REPLAY,
        /** User wants to go to the level select */
        LEVEL,
        /** User wants to go to the next level */
        NEXT
    };
    
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The "replay" button */
    std::shared_ptr<cugl::scene2::Button>    _replay;
    /** The "level select" button */
    std::shared_ptr<cugl::scene2::Button>    _level;
    /** The "next level" button */
    std::shared_ptr<cugl::scene2::Button>    _next;
    
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
    EndLevelMenu() : Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~EndLevelMenu() { dispose(); }
    
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
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    
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
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }
    
};

#endif /* EndMenu_h */
