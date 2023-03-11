//
//  Level.cpp
//	Platformer
// 
//  Contains methods for retreving information about a level loaded from memory
//
//  Created by Jack on 2/23/23.
//

#include "Level.h"
#include <cugl/cugl.h>
#include <vector>


std::vector<Vec3> Level::GetCollectibleLocs() {
    std::vector<Vec3> collectibles;
    collectibles.push_back(keyLoc);
    return collectibles;
}

/** A static method for loading and returning a Level object from the given path*/
std::shared_ptr<Level> Level::loadLevel(std::string path) {

    // HARD CODED STUFF: last updated 3/10/23 by jack to match gameplayPrototype.obj mesh object
    /** The starting location for the player */
    auto hcstartLoc = cugl::Vec3(-4.55713, 5.70794, -169.109);
    /** The end location for the player */
    auto hcendLoc = cugl::Vec3(89.7733, -81.1569, -104.641);
    /** The end location for the player */
    auto hckeyLoc = cugl::Vec3(-101.054, 180.439, 31.7418);
    /**the starting normal cut*/
    auto hcnormal = Vec3(-1, 0, 0);

    bool isMac;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    isMac = false;
#else
    isMac = true;
#endif

    std::string objPath = isMac ? "../meshes/GameplayPrototype.obj" : "../../assets/meshes/GameplayPrototype.obj";
    auto mesh = PivotMesh::MeshFromOBJ(objPath);

    return std::make_shared<Level>(hcstartLoc, hcnormal, hcendLoc, hckeyLoc, mesh);
}
