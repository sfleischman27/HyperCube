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
    
    
    /*_world = std::make_shared<physics2::ObstacleWorld>();
    _world->init(*bounds, Vec2(0, -GRAVITY));*/
    
    _world = physics2::ObstacleWorld::alloc(rect, Vec2(0,-GRAVITY));
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        //CULog("contact");
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

/** TODO: return the world's polygonObstacles*/
std::vector<cugl::physics2::PolygonObstacle> PhysicsController::getPolygonObstacles(){
    
    auto obstacles = _world->getObstacles();
    std::vector<cugl::physics2::PolygonObstacle> polygonObstacles;
    
    /*for (cugl::physics2::PolygonObstacle it = obstacles.begin(); it != obstacles.end(); it++) {
        if(it.get)
    }*/
    
    return std::vector<cugl::physics2::PolygonObstacle>();
}

void PhysicsController::removeObstacles(){
    for (std::shared_ptr<cugl::physics2::Obstacle> obj : _world->getObstacles()){
        obj->markRemoved(true);
    }
}

void PhysicsController::update(float dt){
    _world->update(dt);
    _world->garbageCollect();
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
        
    //*contact.get();
    
    /*b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);
    

    // See if we have landed on the ground.
    if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2)) {
        _avatar->setGrounded(true);
        // Could have more than one ground
        _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    }*/
/*
    // If we hit the "win" door, we are done
    if((bd1 == _avatar.get()   && bd2 == _goalDoor.get()) ||
        (bd1 == _goalDoor.get() && bd2 == _avatar.get())) {
        setComplete(true);
    }
 */
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
