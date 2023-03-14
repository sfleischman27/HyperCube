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
#include <cugl/cugl.h>
#include "GameModel.h"

/**
 *  Takes in level file and outputs Level object
 */
class DataController {
protected:
    std::shared_ptr<cugl::AssetManager> _assets;
  
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
     * Initializes the game model contents
     *
     * @param level      The locaiton of the level json to be loaded
     * @param model      The game model to load the level data into
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool initGameModel(std::string level, const std::shared_ptr<GameModel>& model);
    
};

#endif /* DataController_h */
