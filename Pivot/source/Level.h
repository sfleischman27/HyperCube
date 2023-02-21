//
//  Level.h
//  Platformer
//
//  Light model that has getters and setters for the level mesh, starting position
//  and plane, as well as any other level data we add.
//
//  storage for the level mesh once loaded
//  includes level default data will be placed into gameModel then updated there
//  this default data will be used to restart levels
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef Level_h
#define Level_h
#include <cugl/cugl.h>

using namespace cugl;

/**
 * A class representing a level and its starting data
 */
class Level{
  
#pragma mark State
private:
    /** Player starting location */
    Vec3 _startLoc;
    /** Plane starting plane */
    Vec3 _startNorm;
    /** Level mesh object */
    // TODO: figure out what type this should be (_mesh)
    
public:
    /** A public accessible, read-only version of the starting location */
    const Vec3& startLoc;
    /** A public accessible, read-only version of the starting plane */
    const Vec3& startNorm;
}

#pragma mark Main Functions
public:
    /**
     * Creates the model state.
     *
     * @param startLoc          The starting location of the player
     * @param startNorm        The starting norm of the plane
     * @param mesh                   The mesh object
     */
    TileModel(Vec3 startLoc, Vec3 startNorm): startLoc(_startLoc), startNorm(_startNorm) {
        // set attributes
        setPlayerLoc(startLoc);
        setPlaneNorm(startNorm);
        setMesh();
    }

#pragema mark Setters
public:
    /**
     *  Sets the starting position of the player
     *
     *  @param startLoc The starting location of the player
     */
    void setPosition(Vec3 startLoc) {
        _startLoc = startLoc;
    }

    /**
     *  Sets the starting normal of the plane
     *
     *  @param startLoc The starting norm of the plane
     */
    void setPosition(Vec3 startNorm) {
        _startNorm = startNorm;
    }

    /**
     *  Sets the level mesh
     *
     *  @param mesh  The level mesh
     */
    void setMesh() {
        return;
    }


#endif /* Level_h */
