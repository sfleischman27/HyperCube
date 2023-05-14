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
#define GRAVITY 300.0

#define MAX_H_SPEED 300.0

#define MAX_V_SPEED 300.0

#define JUMP_SPEED 400.0

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

    _world->setLockStep(true);
    _world->setStepsize(0.015);

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
    g._player->setVelocity(Vec3(velocity.x, velocity.y, 0));
}

/**
 *  Returns the sign (+1 -1) of float
 */
float getSignFloat(float f){
    return f/abs(f);
};

void PhysicsController::fall(GameModel g){
    //acceleration, with a max accel speed
    /*if(abs(g._player->getVelocity().y) <= MAX_V_SPEED){
        move2D(g, Vec3(0, g._player->getVelocity().y - GRAVITY));
    } else {
        move2D(g, Vec3(0, getSignFloat( g._player->getVelocity().y) * MAX_V_SPEED));
    }*/
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
    //_world->garbageCollect();
}

#pragma mark Collision Handling
// Deprecated: will delete later
void PhysicsController::beginContact(b2Contact* contact) {
    /** Deprecated: the method is moved to GamePlayController as it involved player and cut collision */
}
void PhysicsController::endContact(b2Contact* contact) {
    /** Deprecated: the method is moved to GamePlayController as it involved player and cut collision */
}
