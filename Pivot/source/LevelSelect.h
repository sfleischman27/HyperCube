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
    // whether to play this levels background music
    bool _playMusic = false;

    enum Choice {
        /** User has not yet made a choice */
        NONE = -1,
        /** User wants to start level 1 */
        LEVEL1 = 0,
        /** User wants to start level 2 */
        LEVEL2 = 1,
        /** User wants to start level 3 */
        LEVEL3 = 2,
        /** User wants to start level 4 */
        LEVEL4 = 3,
        /** User wants to go to the next page of levels */
        NEXT = 20,
        /** User wants to go the the previous page of levels */
        PREV = 21,
        /** User wants to go to the settings menu */
        SETTINGS = 22
    };

    enum Pack {
        /** The pack of levels that the page is displaying */
        ZERO = 0, // tutorial
        ONE = 1, // lab
        TWO = 2, // woods
        THREE = 3, // final
    };

protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** The list of level buttons */
    std::unordered_map<std::string,std::shared_ptr<cugl::scene2::Button>> _buttons;
    /** The pack label */
    std::shared_ptr<cugl::scene2::Label> _label;

    /** The max level number unlocked (starts at 1)*/
    int _maxLevel;
    /** The player menu choice */
    Choice _choice;
    /** The current pack */
    Pack _pack;

    /** The total number of levels we have in the game */
    const int LEVELS_IMPLEMENTED = 13;
    /** The total number of packs in the game */
    const int PACKS_IMPLEMENTED = 4;

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
     * Initializes the controller contents, making it ready for loading with only the first level unlocked
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets) { return init(assets, 0); }

    /**
     * Initializes the controller contents, making it ready for loading with all levels unlocked
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool initMax(const std::shared_ptr<cugl::AssetManager>& assets) { return init(assets, LEVELS_IMPLEMENTED - 1); }

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

#pragma mark State Transition Helpers
public:
    /**
     * Returns a level name string using the current level menu states
     *
     * Relies on the choice and pack enums in levelSelect (which get reset when levelSelect is activated)
     * Only use when a level has been selected! (otherwise this will break things)
     */
    std::string getLevelString();

    /**
     * Returns a level name string using the current level menu states
     *
     * Relies on the choice and pack enums in levelSelect (which get reset when levelSelect is activated)
     * Only use when a level has been selected! (otherwise this will break things)
     */
    std::string getNextLevelString();

    /**
     * Returns a level name string that is the first level in the current pack and sets to that level
     */
    std::string getFirstInPackString();
    
    /**
     * Updates the choice and pack to the next level
     */
    void nextLevel();

    /**
     * Returns the level number  of the max level unlocked
     */
    int getMaxLevel() { return _maxLevel; }

    /**
     * Returns the integer version of the current level and pack
     */
    int levelNum();
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }

    /**
     * Returns true if the current level is the last level
     */
    bool isLast();

    /**
     * Returns true if the current level is the last level in the current pack
     */
    bool isLastInPack();

    /**
     * Unlocks the next level if it is not yet unlocked
     */
    void unlockNextLevel();

    /**
     * Switches page to the next pack if possible
     */
    void nextPack();

    /**
     * Switches page to the previous pack if possible
     */
    void prevPack();

    /**
     * Resets unlocked levels
     */
    void resetMax() { _maxLevel = 0; }

private:
    /**
     * Returns a level name string using the given level states
     *
     * @param level  Level number (0-14)
     * @param pack    Level pack string
     */
    std::string toLevelString(int level, std::string pack);

    /**
     * Returns the current pack as a string.
     *
     * @return the string of the current pack
     */
    std::string packToString(Pack pack);

    /** Translates a button level string to a number
     *  Only use for levels 1-15  all others return -1
     *
     *  Example: "level12" --> 11
     */
    int buttNameToNum(std::string name);

    /**
     * Updated max level if level is greater than the current max
     */
    void updateMax(int level);

    void setChoice(std::string name);

    /**
     * Returns the number of levels before the given pack
     */
    int prePackNum(Pack pack);

    /**
     * Returns the number of levels in the given pack
     */
    int inPackNum(Pack pack);
    
    /**
     * Updates the label for the current pack
     */
    void setPackLabel();

};

#endif /* LevelSelect_h */
