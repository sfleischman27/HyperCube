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
#include "Mesh.h"

using namespace cugl;

/**
 * A class representing a level and its starting data
 */
class Level{
    
#pragma mark State
private:
    /** Player starting location */
    Vec3 _startLoc;
    /** Starting plane */
    Vec3 _startNorm;
    /** Level mesh object */
    // TODO: Mesh _mesh;
    
public:
    /** A public accessible, read-only version of the starting location */
    const Vec3& startLoc;
    /** A public accessible, read-only version of the starting plane */
    const Vec3& startNorm;
    /** A public accessible, read-only version of the leve mesh */
    // TODO: const Mesh mesh;
    
#pragma mark Main Functions
public:

    Level() : startLoc(_startLoc), startNorm(_startNorm) {
        _startLoc = Vec3::ZERO;
        _startNorm = Vec3(-1, 0, 0);
    }

    /**
     * Creates the model state.
     *
     * @param startLoc          The starting location of the player
     * @param startNorm        The starting norm of the plane
     */
    Level(Vec3 startLoc, Vec3 startNorm) : startLoc(_startLoc), startNorm(_startNorm) {
        // set attributes
        setPlayerLoc(startLoc);
        setPlaneNorm(startNorm);
        setMesh();
    }

    
    
#pragma mark Setters
public:
    /**
     *  Sets the starting position of the player
     *
     *  @param startLoc The starting location of the player
     */
    void setPlayerLoc(Vec3 startLoc) {
        _startLoc = startLoc;
    }
    
    /**
     *  Sets the starting normal of the plane
     *
     *  @param startLoc The starting norm of the plane
     */
    void setPlaneNorm(Vec3 startNorm) {
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

    /**Gets a vector of Poly2 objects which represent a CUT throught the map at the given angle
    * @param origin the origin of the cut plane
    * @param normal the normal vector of the cut plane
    */
    std::vector<cugl::Poly2> GetMapCut(Vec3 origin, Vec3 normal);
    

    /** A static method for loading and returning a Level object from the given path*/
    static Level loadLevel(std::string path) {
        // for now this is just an empty level because the cut is hardcoded
        return Level(Vec3::ZERO, Vec3(-1, 0, 0));
    }
};

#endif /* Level_h */
