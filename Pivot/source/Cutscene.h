//
//  Cutscene.h
//  Pivot
//
//  Created by Sarah Fleischman on 5/18/23.
//

#ifndef Cutscene_h
#define Cutscene_h
#include <cugl/cugl.h>

class Cutscene : public cugl::Scene2 {
public:
    enum Choice {
        /** The user has not yet pressed the button */
        NONE,
        /** Story 1 is being turned on */
        STORY1,
        /** Story 2 is being turned on */
        STORY2,
        /** Story 3 is being turned on */
        STORY3,
        /** Story 4 is being turned on */
        STORY4,
        /** Button is being turned on */
        BUTTON,
        /** The user has pressed the next button */
        NEXT
    };
    
protected:
    /** The asset manager for loading */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The first story element */
    std::shared_ptr<cugl::scene2::SceneNode> _story1;
    /** The second story element */
    std::shared_ptr<cugl::scene2::SceneNode> _story2;
    /** The third story element */
    std::shared_ptr<cugl::scene2::SceneNode> _story3;
    /** The fourth story element */
    std::shared_ptr<cugl::scene2::SceneNode> _story4;
    /** The next button */
    std::shared_ptr<cugl::scene2::Button> _butt;
    
    /** The current status of the scene */
    Choice _choice;
    /** A counter used to pause between storys */
    int _counter;
    
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
    Cutscene() : Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~Cutscene() { dispose(); }
    
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
    void update(float timestep);
    
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
     * Resets elements in the scene
     */
    void resetStory();
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }
    
};

#endif /* Cutscene_h */
