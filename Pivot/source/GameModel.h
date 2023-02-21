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
    Vec3 _velocity
    /** Player animation frame */
    // TODO: figure out what type this should be (_frame)
    
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
     * @param loc               The location of the player
     * @param velocity    The velocity of the player
     * @param norm             The norm of the plane
     * @param cut               The cut
     */
    GameModel(Vec3 loc, Vec3 velocity, Vec3 norm, std::vector<Poly2> cut){
        setPlayerLoc(loc);
        setPlayerVelocity(velocity);
        setPlaneNorm(norm);
        setCut(cut);
    }
    
#pragema mark Setters
public:
    /**
     *  Sets the position of the player
     *
     *  @param loc          The location of the player
     */
    void setPlayerLoc(Vec3 loc) {
        _loc = loc;
    }
    
    /**
     *  Sets the velocity of the player
     *
     *  @param velocity          The location of the player
     */
    void setPlayerLoc(Vec3 velocity) {
        _velocity = velocity;
    }
    
    /**
     *  Sets the normal of the plane
     *
     *  @param norm        The norm of the plane
     */
    void setPosition(Vec3 norm) {
        _norm = norm;
    }
    
    /**
     *  Sets the cut
     *
     *  @param cut          The cut
     */
    void setCut(cut) {
        _cut = cut;
    }
    
}

#endif /* GameModel_h */
