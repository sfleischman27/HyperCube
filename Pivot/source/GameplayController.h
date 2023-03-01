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
#include "DataController.h"
#include "Level.h"
#include "PhysicsController.h"
#include "PlaneController.h"
#include "InputController.h"
#include "GameModel.h"
#include "GraphicsEngine.h"

class GameplayController : public cugl::Scene2 {

private:
    std::unique_ptr<GameModel> _model;
    InputController _input;
    
//protected:
//    /** The asset manager for this game mode. */
//    std::shared_ptr<cugl::AssetManager> _assets;
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
    *
    * @return true if the controller is initialized properly, false otherwise.
    */
    bool init(const std::shared_ptr<AssetManager>& assets);
    
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
     * Draws all this scene to the given SpriteBatch.
     *
     * The default implementation of this method simply draws the scene graph
     * to the sprite batch.  By overriding it, you can do custom drawing
     * in its place.
     *
     * @param batch     The SpriteBatch to draw with.
     */
    void render(const std::shared_ptr<cugl::SpriteBatch>& batch) override;
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
};

#endif /* GameplayController_h */
