//
//  GameModel.h
//  Platformer
//
//  Light model that holds the Player data, Plane data, and other game state data
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef GameModel_h
#define GameModel_h
#include <cugl/cugl.h>
#include "Level.h"

using namespace cugl;

/**
 * A class representing an active level and its starting data
 */
class GameModel{
    
#pragma mark Player State
private:
    /** Player location */
    Vec3 _loc;
    /** Player velocity */
    Vec3 _velocity;
    /** Player animation frame */
    // TODO: figure out what type this should be (_frame)

    /**The Level being played*/
    Level _level;
    
#pragma mark Plane State
private:
    /** Plane normal vector */
    Vec3 _norm;
    
#pragma mark Cut State
private:
    /** Vector of cut polygons */
    std::vector<Poly2> _cut;
    
#pragma mark Main Functions
public:
    /**
     * Creates the model state.
     *
     * @param level The level object containing all the info necessary for initializing game
     */
    GameModel(Level level) {
        setPlayerLoc(level.startLoc);
        setPlayerVelocity(Vec3::ZERO);
        setPlaneNorm(level.startNorm);
        setCut(level.GetMapCut(_loc, _norm));
    }
    
#pragma mark Setters
public:
    /**
     *  Sets the position of the player in 3D space
     *
     *  @param loc          The location of the player
     */
    void setPlayerLoc(Vec3 loc) {
        _loc = loc;
    }
    
    /**
     *  Gets the position of the player in 3D space
     *
     *  @return loc          The location of the player
     */
    Vec3 getPlayerLoc() {
        return _loc;
    }
    
    /**
     *  Sets the velocity of the player in 3D space
     *
     *  @param velocity          The location of the player
     */
    void setPlayerVelocity(Vec3 velocity) {
        _velocity = velocity;
    }
    
    Vec3 getPlayerVelocity() {
        return _velocity;
    }
    
    /**
     *  Sets the normal of the plane
     *
     *  @param norm        The norm of the plane
     */
    void setPlaneNorm(Vec3 norm) {
        _norm = norm;
    }
    
    /**
     *  Sets the cut
     *
     *  @param cut          The cut
     */
    void setCut(std::vector<Poly2> cut) {
        _cut = cut;
    }

    std::vector<Poly2> getCut() {
        return _cut;
    }
    
    /**
     * Returns if the player is touching the ground
     */
    bool touchingGround(){
        return true;
    }
    
};

#endif /* GameModel_h */
