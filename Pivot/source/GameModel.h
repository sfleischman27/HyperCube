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
#include "Collectible.h"
#include "PlayerModel.h"

using namespace cugl;

/**
 * A class representing an active level and its starting data
 */
class GameModel{
    
#pragma mark Player State
public:
    /** Player */
    std::shared_ptr<PlayerModel> _player;
#pragma mark Level
private:
    /**The Level being played*/
    std::shared_ptr<Level> _level;
    
#pragma mark Plane State
private:
    /** Plane normal vector */
    Vec3 _norm;
    
#pragma mark Cut State
private:
    /** Vector of cut polygons */
    std::vector<Poly2> _cut;
    
#pragma mark Collectibles State
public:
    /** Vector of collectibles */
    std::vector<Collectible> _collectibles;
    
#pragma mark Main Functions
public:
    /**
     * Creates the model state.
     *
     * @param level The level object containing all the info necessary for initializing game
     */
    GameModel(std::shared_ptr<Level> level) {
        setPlaneNorm(level->startNorm);
        setCut(level->GetMapCut(level->startLoc, level->startNorm));
        setCollectibles(level->GetCollectibleLocs());
        _level = level;
    }
    
#pragma mark Setters
public:
    /**
     * Sets the player model
     *
     * @param player  The player object
     */
    void setPlayer(std::shared_ptr<PlayerModel> player){
        _player = player;
    }
    
    /**
     *  Sets the normal of the plane and recomputes the CUT
     *
     *  @param norm        The norm of the plane
     */
    void setPlaneNorm(Vec3 norm) {
        _norm = norm.normalize();
        setCut(_level->GetMapCut(Vec3::ZERO, _norm));
    }

    /**Rotate the normal around the player by some angle in radians*/
    void rotateNorm(float radians) {
        Vec3 newNorm = Vec3(
            _norm.x * cos(radians) - _norm.y * sin(radians),
            _norm.x * sin(radians) + _norm.y * cos(radians), 
            0
        );
        setPlaneNorm(newNorm);
    }

    Vec3 getPlaneNorm() {
        return _norm;
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
    
    void setCollectibles(std::vector<Vec3> locs) {
        for(int i = 0; i < locs.size(); i++) {
            _collectibles.push_back(Collectible(locs[i], std::to_string(i)));
        }
    }
    
    std::vector<Collectible> getCollectibles() {
        return _collectibles;
    }

    std::shared_ptr<Level> getLevel() {
        return _level;
    }
    
};

#endif /* GameModel_h */
