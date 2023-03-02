//
//  PhysicsController.h
//  Platformer
//  Controls the physics of the player and updates the game model as needed
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef PhysicsController_h
#define PhysicsController_h
#include <cugl/cugl.h>
#include "GameModel.h"

//bool _active(false);
/**
 *  Have a few functions that take in the GameModel and do the following:
 *   - alter the player movement vector to move right
 *   - alter the player movement vector to move left
 *   - alter the player movement vector vector to jump
 *   - update the player location according to current movement vector
 */

class PhysicsController {
    
protected:
    /** the physics world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    
    //std::vector<cugl::physics2::PolygonObstacle> _levelbounds;
    
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

public:
    
#pragma mark Physics Controller

    PhysicsController();
    
    ~PhysicsController(){ dispose(); };
    
    bool init(Size dimen, const Rect& rect, float scene_width);
    
    /**
     * Takes a GameModel g and moves it horizontally by a float speed
     * @param g
     * @param speed
     */
    void move(GameModel g, float speed){
        move2D(g, Vec2(speed, 0));
    }
    
    /**
     * Takes a GameModel g and moves it horizontally by a float speed
     * @param g
     * @param speed
     */
    void jump(GameModel g);
    
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
    
    /**
     * Disposes this physics controller, deactivating all listeners.
     *
     * As the listeners are deactived, the user will not be able to
     * monitor input until the controller is reinitialized with the
     * {@link #init} method.
     */
    
    /**
     * Takes a GameModel g and make it fall by the gravity
     * @param g
     */
    
    float getScale() { return _scale; };
    
    std::shared_ptr<cugl::physics2::ObstacleWorld> getWorld(){ return _world; };
    
    void fall(GameModel g);
    
    void dispose();
    
    void clear();
    
    
private:
    /**
     * Moves player along the 2D plane?
     *  TODO: Map the velocity along the plane specified by the normal vec?
     */
    void move2D(GameModel g, Vec2 velocity);
    
#pragma mark Convert Poly2s to physics objects
    
public:
    /**
    * Creates all physics objects from a list of Poly2s given by the GameModel.
    */
    void createPhysics( GameModel g, Size b);
    
    void update(float dt);
    
    /**
     *
     */
    std::vector<cugl::physics2::PolygonObstacle> getPolygonObstacles();
    /**
     * mark old obstacles for garbage collection and remove them.
     */
    void removeObstacles();

};


#endif /* PhysicsController_h */
