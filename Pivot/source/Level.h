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
    /** The location of the exit */
    Vec3 _exitLoc;
    /** The location of the key*/
    Vec3 _keyLoc;

    
public:
    /** A public accessible, read-only version of the starting location */
    const Vec3& startLoc;
    /** A public accessible, read-only version of the starting plane */
    const Vec3& startNorm;
    /** A public accessbile, read-only version of the exit location */
    const Vec3& exitLoc;
    /** A public accessbile, read-only version of the key location */
    const Vec3& keyLoc;
    
#pragma mark Main Functions
public:

    Level() : startLoc(_startLoc), startNorm(_startNorm), exitLoc(_exitLoc), keyLoc(_keyLoc) {
        _startLoc = Vec3::ZERO;
        _startNorm = Vec3(-1, 0, 0);
        _exitLoc = Vec3(0, 0.03, 0.03);
        _keyLoc = Vec3(0, -0.02, -0.02);
    }

    /**
     * Creates the model state.
     *
     * @param startLoc          The starting location of the player
     * @param startNorm        The starting norm of the plane
     */
    Level(Vec3 startLoc, Vec3 startNorm, Vec3 exitLoc, Vec3 keyLoc)
        : startLoc(_startLoc), startNorm(_startNorm), exitLoc(_exitLoc), keyLoc(_keyLoc) {

        setPlayerLoc(startLoc);
        setPlaneNorm(startNorm);
        setExitLoc(exitLoc);
        setkeyLoc(keyLoc);
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

    /**
     *  Sets the exit location
     *
     *  @param exitLoc The location of the exit
     */
    void setExitLoc(Vec3 exitLoc) {
        _exitLoc = exitLoc;
    }

    /**
     *  Sets the key location
     *
     *  @param keyLoc The location of the key
     */
    void setkeyLoc(Vec3 keyLoc) {
        _keyLoc = keyLoc;
    }

    /**Gets a vector of Poly2 objects which represent a CUT throught the map at the given angle
    * @param origin the origin of the cut plane
    * @param normal the normal vector of the cut plane
    */
    std::vector<cugl::Poly2> GetMapCut(Vec3 origin, Vec3 normal);

    /**Get the 2d coordinates relative to the cut plane of a 3d location
    * it also returns the projected distance from that point to the cut plane, which can be used to threshold drawing of an object at that location
    * RETURN: screen coordinates and projection distance pairs are returned as a std::tuple<Vec2,float>*/
    std::tuple<cugl::Vec2, float> ScreenCoordinatesFrom3DPoint(cugl::Vec3);
    

    /** A static method for loading and returning a Level object from the given path*/
    static std::shared_ptr<Level> loadLevel(std::string path) {

        // HARD CODED STUFF
        /** The starting location for the player */
        auto hcstartLoc = cugl::Vec3(0.0, 0.0, -4.47849889621);
        /** The end location for the player */
        auto hcendLoc = cugl::Vec3(3.99815288094, 0.0, -2.60769182756);
        /** The end location for the player */
        auto hckeyLoc = cugl::Vec3(2.50532717051, 2.75713430113, 3.90467730801);
        // set attributes

        // for now this is just an empty level because the cut is hardcoded
        return std::make_shared<Level>(hcstartLoc, Vec3(-1, 0, 0), hcendLoc, hckeyLoc);
    }

};

#endif /* Level_h */
