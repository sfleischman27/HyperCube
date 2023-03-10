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

    // HARD CODED STUFF
    /** The starting location for the player */
    auto hcstartLoc = cugl::Vec3(0.0723488601292, -0.0809223593776, -4.47849889621);
    /** The end location for the player */
    auto hcendLoc = cugl::Vec3(-2.83521234341, 2.66340165704, -2.60769182756);
    /** The end location for the player */
    auto hckeyLoc = cugl::Vec3(0.142900888615, 3.64378919953, 3.90467730801);
    // set attributes
    float scale = 0.1; //idk why this scale is not that same as the scale for the polyline :(

    // for now this is just an empty level because the cut is hardcoded

    //Make a mesh object from an obj file
    //CULog(std::filesystem::current_path().string().c_str());
    bool isMac;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    isMac = false;
#else
    isMac = true;
#endif

    std::string objPath = isMac ? "../meshes/GameplayPrototype.obj" : "../../assets/meshes/GameplayPrototype.obj";
    auto mesh = PivotMesh::MeshFromOBJ(objPath, scale);

    return std::make_shared<Level>(hcstartLoc * scale, Vec3(-1, 0, 0) * scale, hcendLoc * scale, hckeyLoc * scale, mesh);
}
