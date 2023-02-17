//
//  PFGameScene.h
//  PlatformDemo
//
//  This is the most important class in this demo.  This class manages the gameplay
//  for this demo.  It also handles collision detection. There is not much to do for
//  collisions; our WorldController class takes care of all of that for us.  This
//  controller mainly transforms input into gameplay.
//
//  You will notice that we do not use a Scene asset this time.  While we could
//  have done this, we wanted to highlight the issues of connecting physics
//  objects to scene graph objects.  Hence we include all of the API calls.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#ifndef __PF_GAME_SCENE_H__
#define __PF_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "PFInput.h"
#include "PFBullet.h"
#include "PFDudeModel.h"
#include "PFRopeBridge.h"
#include "PFSpinner.h"

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    PlatformInput _input;
    
    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _debugnode;
    /** Reference to the win message label */
    std::shared_ptr<cugl::scene2::Label> _winnode;
    /** Reference to the lose message label */
    std::shared_ptr<cugl::scene2::Label> _losenode;
    /** Reference to the left joystick image */
    std::shared_ptr<cugl::scene2::PolygonNode> _leftnode;
    /** Reference to the right joystick image */
    std::shared_ptr<cugl::scene2::PolygonNode> _rightnode;

    /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    std::shared_ptr<cugl::physics2::BoxObstacle>    _goalDoor;
    /** Reference to the player avatar */
    std::shared_ptr<DudeModel>			  _avatar;
    /** Reference to the spinning barrier */
    std::shared_ptr<Spinner>			  _spinner;
    /** Reference to the rope bridge */
    std::shared_ptr<RopeBridge>			  _ropebridge;

    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    /** Whether we have failed at this world (and need a reset) */
    bool _failed;
    /** Countdown active for winning or losing */
    int _countdown;
      
    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture*> _sensorFixtures;

#pragma mark Internal Object Management
    /**
     * Lays out the game geography.
     *
     * Pay close attention to how we attach physics objects to a scene graph.
     * The simplest way is to make a subclass, like we do for the dude.  However,
     * for simple objects you can just use a callback function to lightly couple
     * them.  This is what we do with the crates.
     *
     * This method is really, really long.  In practice, you would replace this
     * with your serialization loader, which would process a level file.
     */
    void populate();
    
    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with dude.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * @param obj    The physics object to add
     * @param node   The scene graph node to attach it to
     * @param useObjPosition  Whether to update the node's position to be at the object's position
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                     const std::shared_ptr<cugl::scene2::SceneNode>& node,
                     bool useObjPosition=true);

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
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameScene();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  This initializer uses the default scale.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect& rect);
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     * @param gravity   The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect& rect, const cugl::Vec2& gravity);
    
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
	void setComplete(bool value);

	/**
	* Returns true if the level is failed.
	*
	* If true, the level will reset after a countdown
	*
	* @return true if the level is failed.
	*/
	bool isFailure() const { return _failed; }

	/**
	* Sets whether the level is failed.
	*
	* If true, the level will reset after a countdown
	*
	* @param value whether the level is failed.
	*/
	void setFailure(bool value);
    
#pragma mark -
#pragma mark Collision Handling
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

#pragma mark -
#pragma mark Gameplay Handling
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
    * Adds a new bullet to the world and sends it in the right direction.
    */
    void createBullet();

    /**
    * Removes the input Bullet from the world.
    *
    * @param  bullet   the bullet to remove
    */
    void removeBullet(Bullet* bullet);

  };

#endif /* __PF_GAME_SCENE_H__ */
