//
//  DataController.h
//  Platformer
//
//  Contains methhods to load data as needed for levels
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef DataController_h
#define DataController_h
#include <cugl/base/CUApplication.h>
#include <cugl/cugl.h>
#include "GameModel.h"
#include <vector>

/**
 *  Takes in level file and outputs Level object
 */
class DataController {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The save file JsonValue */
    std::shared_ptr<cugl::JsonValue> _save;
    /** The save file directory path */
    std::string _saveDir;
    /** The default save file JsonValue */
    std::shared_ptr<cugl::JsonValue> _default;
  
#pragma mark Constructors
public:
    /**
     * Creates a new data controller with no values.
     */
    DataController() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources.
     */
    ~DataController() { }//dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    //void dispose();
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets) {
        _assets = assets;
        return true;
    };
    
    /**
     * Loads a new level
     *
     *  @param level    The location of the level json to be loaded
     *  @param model    The game model to load the level data into
     *
     *  @return true if the controller is initialized properly, false otherwise.
     */
    bool loadGameModel(std::string level, const std::shared_ptr<GameModel>& model);
    
    /**
     * Resets current level
     *
     *  @param level    The location of the level json to be loaded
     *  @param model    The game model to load the level data into
     *
     *  @return true if the model is initialized properly, false otherwise.
     */
    bool resetGameModel(std::string level, const std::shared_ptr<GameModel>& model);
    
    /**
     * Sets up the save file writer, json value, and path
     *
     * @param dir         The path to the save file (save.json)
     * @param exists  True if there is already a save file
     */
    void setupSave(std::string dir, bool exists);
    
    /**
     * Resets the save json to the default
     */
    void resetSave();
    
    /**
     * Writes the content of the save json to the save file
     */
    void save();
    
    /**
     * Updates the max level in the save json
     */
    void setMaxLevel(long maxLevel) { _save->get("max_level")->set(maxLevel); }
    
    /**
     * Returns the max level unlocked that is stored in the save file
     */
    int getMaxLevel() { return _save->getLong("max_level"); }
    
    /**
     * Updates the buttons or joystick choice
     *
     * true = buttons, false = joystick
     */
    void setControls(bool buttons) { _save->get("buttons")->set(buttons); }
    
    /**
     * Returns the button choice stored in the save file
     */
    bool getControls() { return _save->getBool("buttons"); }
    
    /**
     * Updates the volume in the save json
     */
    void setVolume(float volume) { _save->get("volume")->set(volume); }
    
    /**
     * Returns the volume that is stored in the save file
     */
    float getVolume() { return _save->getFloat("volume"); }
    
    /**
     * Updates the music choice in the save json
     *
     * true = on, false = off
     */
    void setMusic(bool music) { _save->get("music")->set(music); }
    
    /**
     * Returns the music choice that is stored in the save file
     */
    bool getMusic() { return _save->getBool("music"); }
    
    /**
     * Updates the compass increment selection in the save json
     *
     * true = 180, false = 360
     */
    void setCompass(bool compass) { _save->get("compass_180")->set(compass); }
    
    /**
     * Returns the compass selction that is stored in the save file
     */
    bool getCompass() { return _save->getBool("compass_180"); }
    
    /**
     * Updates the rotate selction in the save json
     */
    void setRotate(bool rotate) { _save->get("rotate")->set(rotate); }
    
    /**
     * Returns the rotate selction that is stored in the save file
     */
    bool getRotate() { return _save->getBool("rotate"); }
    
    /**
     * Updates the outline selction in the save json
     */
    void setOutline(bool outline) { _save->get("outline")->set(outline); }
    
    /**
     * Returns the outline selction that is stored in the save file
     */
    bool getOutline() { return _save->getBool("outline"); }
    
};

#endif /* DataController_h */
