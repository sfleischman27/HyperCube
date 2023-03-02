//
//  PhysicsController.cpp
//  Platformer
//
//  Created by Gordon Tenev on 2/22/23.
//

#include <stdio.h>
#include "PhysicsController.h"

/** The ratio between the physics world and the screen. */
//#define PHYSICS_SCALE 50

/** The ratio between the physics world and the screen. */
#define GRAVITY 9.8

#define MAX_H_SPEED 100.0

#define MAX_V_SPEED 200.0

#define JUMP_SPEED 100.0

PhysicsController::PhysicsController(){
    
}

bool PhysicsController::init(Size dimen, const Rect& rect, float scene_width){
    //bool success = true;
    
    _world = physics2::ObstacleWorld::alloc(rect, Vec2(0,-GRAVITY));
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
      beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact* contact) {
      endContact(contact);
    };
    
    
    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = dimen.width == scene_width ? dimen.width/rect.size.width : dimen.height/rect.size.height;

    
    return true;
}
 
void PhysicsController::clear(){
    _world->clear();
}

void PhysicsController::dispose() {
    removeObstacles();
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

#pragma mark Cut to obstacles

void PhysicsController::removeObstacles(){
    for (std::shared_ptr<cugl::physics2::Obstacle> obj : _world->getObstacles()){
        obj->markRemoved(true);
    }
    _world->garbageCollect();
}

void PhysicsController::createPhysics(GameModel g, Size b){
    //init physics world and bounding box
    _world = std::make_shared<physics2::ObstacleWorld>();
    
    removeObstacles();
    
    std::shared_ptr<Rect> bounds = std::make_shared<Rect>(Vec2::ZERO, b / _scale);
    
    _world->init(*bounds, Vec2(0, -GRAVITY));
    
    //get the cut from the gamemodel
    std::vector<cugl::Poly2> polys = g.getCut();
    
    for(Poly2 p : polys){
        
        std::shared_ptr<cugl::physics2::PolygonObstacle> obstacle = std::make_shared<cugl::physics2::PolygonObstacle>();
        
        bool initialized = obstacle->init(p);
        CUAssertLog(initialized, "levelbounds polygonobstacle not initialized properly");

        obstacle->setBodyType(b2_staticBody);
        obstacle->setPosition(obstacle->getPosition()*_scale);
        
        _world->addObstacle(obstacle);
    }
}

void PhysicsController::update(float dt){
    _world->update(dt);
}

#pragma mark Collision Handling
/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */
void PhysicsController::beginContact(b2Contact* contact) {
    /** TODO:  ADD BEGIN CONTACT COLLISIONS */
}

/**
 * Callback method for the start of a collision
 *
 * This method is called when two objects cease to touch.  The main use of this method
 * is to determine when the characer is NOT on the ground.  This is how we prevent
 * double jumping.
 */
void PhysicsController::endContact(b2Contact* contact) {
    /** TODO:  ADD END CONTACT COLLISIONS */
}
