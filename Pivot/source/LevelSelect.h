//
//  LevelSelect.h
//  Pivot
//
//  Created by Sarah Fleischman on 3/26/23.
//

#ifndef LevelSelect_h
#define LevelSelect_h
#include <cugl/cugl.h>
#include <unordered_map>

class LevelSelect : public cugl::Scene2 {
public:
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to start level 1 */
        level1,
        /** User wants to start level 2 */
        level2,
        /** User wants to start level 3 */
        level3,
        /** User wants to start level 4 */
        level4,
        /** User wants to start level 5 */
        level5,
        /** User wants to start level 6 */
        level6,
        /** User wants to start level 7 */
        level7,
        /** User wants to start level 8 */
        level8,
        /** User wants to start level 9 */
        level9,
        /** User wants to start level 10 */
        level10,
        /** User wants to start level 11 */
        level11,
        /** User wants to start level 12 */
        level12,
        /** User wants to start level 13 */
        level13,
        /** User wants to start level 14 */
        level14,
        /** User wants to start level 15 */
        level15,
    };
    
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The list of level buttons */
    std::unordered_map<std::string,std::shared_ptr<cugl::scene2::Button>> _buttons;
    
    /** The max level number unlocked (starts at 1)*/
    int _maxLevel;
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
    LevelSelect() : Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LevelSelect() { dispose(); }
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets) { return init(assets, 2); }
    // TODO: change this back to 1
    
    /**
     * Initializes the controller contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param level      The max unlocked level
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, int level);
    
    
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
     * The method that updates the menu to reflect unlocked levels
     */
    void updateLevel(int level);
    
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
  
private:
    int nameToNum(std::string name);
    
    void setChoice(std::string name);
    
};

#endif /* LevelSelect_h */
