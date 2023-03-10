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
    std::unordered_map<std::string, Collectible> _collectibles;
    
#pragma mark Backpack State
public:
    /** Vector of collectibles in player backpack */
    std::unordered_set<std::string> _backpack;
    
#pragma mark Main Functions
public:
    /**
     * Creates the model state.
     *
     * @param level The level object containing all the info necessary for initializing game
     */
    GameModel(std::shared_ptr<Level> level) {
        setPlaneNorm(level->startNorm);
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
     *  Sets the Norm
     * 
     * NOTE: YOU should not use this method to set the plane normal,
     * instead use the methods available through  the PLANE CONTROLLER
     * 
     * IF you do use this method it will NOT recompute the cut
     */

    void setPlaneNorm( Vec3 norm) {
        _norm =norm;
    }

    /**
     *  Gets the Current Norm
     */

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

    /**
     *  Gets the cut
     */
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
            _collectibles.insert({std::to_string(i), Collectible(locs[i], std::to_string(i))});
        }
    }
    

    std::shared_ptr<Level> getLevel() {
        return _level;
    }
    
};

#endif /* GameModel_h */
