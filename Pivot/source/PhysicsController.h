//
//  PhysicsController.h
//  Platformer
//
//  Controls the physics of the player and updates the game model as needed
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef PhysicsController_h
#define PhysicsController_h
#include <cugl/cugl.h>
#include "GameModel.h"

/**
 *  Have a few functions that take in the GameModel and do the following:
 *   - alter the player movement vector to move right
 *   - alter the player movement vector to move left
 *   - alter the player movement vector vector to jump
 *   - update the player location according to current movement vector
 */

class PhysicsController {
    
#pragma mark Physics Controller
public:
        
    PhysicsController();
    
    ~PhysicsController(){ dispose(); };
    
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
    void fall(GameModel g);
    
    void dispose();
    
private:
    /**
     * Moves player along the 2D plane?
     *  TODO: Map the velocity along the plane specified by the normal vec?
     */
    void move2D(GameModel g, Vec2 velocity);
    
    /**
     *  Returns the sign (+1 -1) of float
     */
    float getSignFloat(float f);
    
};


#endif /* PhysicsController_h */
