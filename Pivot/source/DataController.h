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
    /** The save file JsonWriter */
    std::shared_ptr<cugl::JsonWriter> _write;
    /** The save file JsonValue */
    std::shared_ptr<cugl::JsonValue> _save;
    /** The save file directory path */
    std::string _saveDir;
  
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
    
    void createSaveFile();
    
    void save(int maxLevel);
    
    void updateSaveJson(long maxLevel);
    
    int getMaxLevel();
};

#endif /* DataController_h */
