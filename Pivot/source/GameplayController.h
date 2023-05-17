//
//  GameplayController.h
//  Platformer
//
//  Controls the level play loop
//
//  Created by Sarah Fleischman on 2/23/23.
//

#ifndef GameplayController_h
#define GameplayController_h

#include <cugl/cugl.h>
#include "SoundController.h"
#include "DataController.h"
#include "PhysicsController.h"
#include "PlaneController.h"
#include "InputController.h"
#include "GameModel.h"
#include "RenderPipeline.h"
#include "PlayerModel.h"
#include "Collectible.h"
#include "GameItem.h"

class GameplayController : public cugl::Scene2 {
public:
    enum State {
        /** Level is currently running */
        NONE,
        /** User pressed the quit button */
        QUIT,
        /** User finished the level */
        END
    };
    
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** set debug mode */
    bool _debug;
    /** current state of the game */
    State _state;
    
    /** is the axis being rotated? (only remake colliders when false) */
    bool _rotating = false;
    
    bool _pickupGlowstick = false;
    
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    //SET OF CUT COLLISION OBSTACLES
    std::set<std::shared_ptr<cugl::physics2::Obstacle>> _cutobstacles;
    //SET OF CUT COLLISION NODES
    std::set<std::shared_ptr<cugl::scene2::SceneNode>> _cutnodes;
    //DEBUG COLLISION NODE
    std::shared_ptr<cugl::scene2::SceneNode> _debugnode;
    
    cugl::Vec2 prevPlay2DPos;
    
    cugl::Vec2 currPlay2DPos;

    cugl::Vec2 lastStablePlay2DPos;
    
    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture*> _sensorFixtures;
    
    std::unordered_map<std::string,std::shared_ptr<cugl::scene2::Button>> _buttons;

    
private:
    std::shared_ptr<SoundController> _sound;// = 
    std::shared_ptr<GameModel> _model;
    std::shared_ptr<PhysicsController> _physics;
    std::shared_ptr<InputController> _input;
    cugl::Size _dimen;
    std::shared_ptr<RenderPipeline> _pipeline;
    std::shared_ptr<PlaneController> _plane;
    std::shared_ptr<DataController> _data;
    
    float _portalDistance = 100.0;
    int  _walkCooldown = 0;


    
    /**
     * Removes all the nodes beloning to _polynodes from _worldnodes. In essence, this cleans up all the old collisions and SceneNodes pertaining to a previous cut to make room for the new cut's collisions.
     */
    void removePolyNodes();
    
protected:
    std::tuple<cugl::Vec2, float> tupleExit;
//    /** Reference to the left joystick image */
//    std::shared_ptr<cugl::scene2::PolygonNode> _leftnode;
//    /** Reference to the right joystick image */
//    std::shared_ptr<cugl::scene2::PolygonNode> _rightnode;
    
public:
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameplayController();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameplayController() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /** Initializes the controller contents, and starts the game
    *
    * The constructor does not allocate any objects or memory.  This allows
    * us to have a non-pointer reference to this controller, reducing our
    * memory allocation.  Instead, allocation happens in this method.
    *
    * The game world is scaled so that the screen coordinates do not agree
    * with the Box2d coordinates.  This initializer uses the default scale.
    *
    * @param assets    The (loaded) assets for this game mode
    * @param rect      The game bounds in Box2d coordinates
    *
    * @return true if the controller is initialized properly, false otherwise.
    */
    
    //bool init(const std::shared_ptr<AssetManager>& assets, const Size& displaySize);
    
    bool init(const std::shared_ptr<AssetManager>& assets, const Size& displaySize, std::shared_ptr<SoundController> sound);
    
    //bool init(const std::shared_ptr<AssetManager>& assets,  const Rect& rect);
    
    bool init(const std::shared_ptr<AssetManager>& assets, const Rect& rect, std::shared_ptr<SoundController> sound);
    
    /**
    * Generates obstacle instances from the given cut, specified by the list of Poly2s given by the GameModel _model.
    */
    void createCutObstacles();
    
    void createObstacleFromPolys(std::vector<cugl::Poly2> polys);

    
    
    /**
     * Adds obstacle to both physics world and world node/debug node via an intermediary SceneNode
     * @param obj the obstacle
     * @param node the SceneNode you're adding the obj to (which will then be added to the world+debug scenenodes
     * @param useObjPosition whether you should use the object's local position when adding to scenes. Default is true.
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                                //const std::shared_ptr<cugl::scene2::SceneNode>& node,
                     bool useObjPosition = true);
    
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
    
    /**
     * Loads a new level into the game model
     *
     * @param name    the name of the level to be loaded (key in assets file)
     */
    void load(std::string name);
    
    /**
     * Draws all this scene to the given SpriteBatch.
     *
     * The default implementation of this method simply draws the scene graph
     * to the sprite batch.  By overriding it, you can do custom drawing
     * in its place.
     *
     * @param batch     The SpriteBatch to draw with.
     */
    void render(const std::shared_ptr<cugl::SpriteBatch>& batch);
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    void setActive(bool value);
    
    /**
     * Sets collectible UI to only show the correct number of collectibles for the level
     *
     * @param col  The number of collectibles in the level (0 to 4)
     */
    void resetCollectibleUI(int col);
    
    /**
     * Changes the collecttible UI to reflect the number of collectibles collected
     *
     * @param col  The number of collectibles in the level (1 to 4)
     * @param got  The number of collectibles collected 
     */
    void collectUI(int col, int got);
    
    /**
     * Turns off and resets all popups
     */
    void resetPopups();
    
#pragma mark State Access

    bool isDebug( ) const { return _debug; }
    
    /** sets whether we are in debug mode or not */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }

    /**Get the 2d coordinates relative to the cut plane of a 3d location
    * it also returns the projected distance from that point to the cut plane, which can be used to threshold drawing of an object at that location
    * RETURN: screen coordinates and projection distance pairs are returned as a std::tuple<Vec2,float>*/
    std::tuple<cugl::Vec2, float> ScreenCoordinatesFrom3DPoint(cugl::Vec3);
    
    void updatePlayer3DLoc(Vec2 displacement);
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    State getState() const { return _state; }
    
    /**
     * Returns the data controller object
     */
    std::shared_ptr<DataController> getDataController() { return _data; }
    
#pragma mark Cut and player Collision Handling
    /**
    * Processes the start of a collision
    *
    * This method is called when we first get a collision between two objects.  We use
    * this method to test if it is the "right" kind of collision.  In particular, we
    * use it to test if we make it to the win door.  We also us it to eliminate bullets.
    *
    * @param  contact  The two bodies that collided
    */
    void beginContact(b2Contact* contact);
    /**
    * Processes the end of a collision
    *
    * This method is called when we no longer have a collision between two objects.
    * We use this method allow the character to jump again.
    *
    * @param  contact  The two bodies that collided
    */
    void endContact(b2Contact* contact);
    
#pragma mark Save File Functions

    /**
     * Saves the game
     */
    void save() { _data->save(); }
    
    /**
     * Updates the max level
     */
    void setMaxLevel(long maxLevel) { _data->setMaxLevel(maxLevel); }
    
    /**
     * Returns the max level unlocked that is stored in the save file
     */
    int getMaxLevel() { return _data->getMaxLevel(); }
    
    /**
     * Updates the buttons or joystick choice
     *
     * true = buttons, false = joystick
     */
    void setControls(bool buttons) { _data->setControls(buttons); }
    
    /**
     * Returns the button choice stored in the save file
     */
    bool getControls() { return _data->getControls(); }
    
    /**
     * Updates the volume in the save json
     */
    void setVolume(float volume) { _data->setVolume(volume); }
    
    /**
     * Returns the volume that is stored in the save file
     */
    float getVolume() { return _data->getVolume(); }
    
    /**
     * Updates the music choice in the save json
     *
     * true = on, false = off
     */
    void setMusic(bool music) { _data->setMusic(music); }
    
    /**
     * Returns the music choice that is stored in the save file
     */
    bool getMusic() { return _data->getMusic(); }
    
    /**
     * Updates the compass increment selection in the save json
     *
     * true = 180, false = 360
     */
    void setCompass(bool compass) { _data->setCompass(compass); }
    
    /**
     * Returns the compass selction that is stored in the save file
     */
    bool getCompass() { return _data->getCompass(); }
    
    /**
     * Updates the rotate selction in the save json
     */
    void setRotate(bool rotate) { _data->setRotate(rotate); }
    
    /**
     * Returns the rotate selction that is stored in the save file
     */
    bool getRotate() { return _data->getRotate(); }
    
    
#pragma mark Setting update functions
    /** Updates joystick setting in input controller */
    void updateJoystick(bool control) {
        _input->settings_isUsingJoystick = control;
        if(!control){
            _input->resetButtonPositions();
        }
    }
    
    /** Updates rotate setting in input controller */
    void updateRotate(bool rotate) {
        if (rotate){
            _input->settings_invertRotate = 1; //normal
        } else {
            _input->settings_invertRotate = -1; //inverted
        }
    }
    
    /** Updates the degrees settings in game model */
    void updateCompass(bool compass) { _model->_degrees = compass; }
    
};

#endif /* GameplayController_h */
