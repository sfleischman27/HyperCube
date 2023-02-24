//
//  PhysicsController.cpp
//  Platformer
//
//  Created by Gordon Tenev on 2/22/23.
//

#include <stdio.h>
#include "PhysicsController.h"

/** The ratio between the physics world and the screen. */
#define PHYSICS_SCALE 50

/** The ratio between the physics world and the screen. */
#define GRAVITY 9.8

#define MAX_H_SPEED 100.0

#define MAX_V_SPEED 200.0

#define JUMP_SPEED 100.0


void PhysicsController::dispose() {
    
}

void PhysicsController::jump(GameModel g){
    if(g.touchingGround()){
        move2D(g, Vec2(0, JUMP_SPEED));
    }
}

void PhysicsController::move2D(GameModel g, Vec2 velocity){
    g.setPlayerVelocity(Vec3(velocity.x, velocity.y, 0));
}

/**
 *  Returns the sign (+1 -1) of float
 */
float getSignFloat(float f){
    return f/abs(f);
};

void PhysicsController::fall(GameModel g){
    //acceleration, with a max accel speed
    if(abs(g.getPlayerVelocity().y) <= MAX_V_SPEED){
        move2D(g, Vec3(0, g.getPlayerVelocity().y - GRAVITY));
    } else {
        move2D(g, Vec3(0, getSignFloat( g.getPlayerVelocity().y) * MAX_V_SPEED));
    }
}

