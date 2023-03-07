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
#include <filesystem>


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

    /** The Mesh For the Level*/
    std::shared_ptr<PivotMesh> _mesh;
    

    
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
    Level(Vec3 startLoc, Vec3 startNorm, Vec3 exitLoc, Vec3 keyLoc, std::shared_ptr<PivotMesh> mesh)
        : startLoc(_startLoc), startNorm(_startNorm), exitLoc(_exitLoc), keyLoc(_keyLoc) {

        setPlayerLoc(startLoc);
        setPlaneNorm(startNorm);
        setExitLoc(exitLoc);
        setkeyLoc(keyLoc);
        setMesh(mesh);
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
    void setMesh(std::shared_ptr<PivotMesh> mesh) {
        _mesh = mesh;
    }

    /**
    *   Gets the level mesh
    */
    std::shared_ptr<PivotMesh> getMesh() {
        return _mesh;
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
    
    std::vector<Vec3> GetCollectibleLocs();
    

    /** A static method for loading and returning a Level object from the given path*/
    static std::shared_ptr<Level> loadLevel(std::string path) {

        // HARD CODED STUFF
        /** The starting location for the player */
        auto hcstartLoc = cugl::Vec3(0.0723488601292 , -0.0809223593776, -4.47849889621);
        /** The end location for the player */
        auto hcendLoc = cugl::Vec3(-2.83521234341, 2.66340165704, -2.60769182756);
        /** The end location for the player */
        auto hckeyLoc = cugl::Vec3(0.142900888615, 3.64378919953, 3.90467730801);
        // set attributes
        float scale = 0.1; //idk why this scale is not that same as the scale for the polyline :(

        // for now this is just an empty level because the cut is hardcoded

        //Make a mesh object from an obj file
        CULog(std::filesystem::current_path().string().c_str());
        path = "..\\..\\assets\\meshes\\MapTest.obj";
        auto mesh = PivotMesh::MeshFromOBJ(path, scale);

        return std::make_shared<Level>(hcstartLoc*scale, Vec3(-1, 0, 0)*scale, hcendLoc*scale, hckeyLoc*scale, mesh);
    }

};

#endif /* Level_h */
