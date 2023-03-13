//
//  DataController.cpp
//  Platformer
//
//  Contains methhods to load data as needed for music and levels
//
//  Created by Jack otto on 2/23/23.
//

#include "DataController.h"
#include <cugl/cugl.h>
#include "GameplayController.h"
#include <vector>

using namespace cugl;

/**
 * Initializes the game model contents
 *
 * @param assets    The (loaded) assets for this game mode
 * @param level      The locaiton of the level json to be loaded
 * @param model      The game model to load the level data into
 *
 * @return true if the game model is initialized properly, false otherwise.
 */
bool DataController::initGameModel(std::string level, const std::shared_ptr<GameModel>& model) {
    
    bool isMac;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    isMac = false;
#else
    isMac = true;
#endif
    
    if (isMac) {
        _assets->loadDirectory("../json/" + level);
    } else {
        _assets->loadDirectory("../../assets/jsonWindows/" + level);
    }
    
    std::shared_ptr<cugl::JsonValue> constants = _assets->get<JsonValue>("constants");
    
    Vec3 playerLoc;
    playerLoc.x = constants->get("player_loc")->get(0)->asFloat();
    playerLoc.y = constants->get("player_loc")->get(1)->asFloat();
    playerLoc.z = constants->get("player_loc")->get(2)->asFloat();
    model->setInitPlayerLoc(playerLoc);
    
    //model->_player->set3DLoc(playerLoc);
    
    Vec3 norm;
    norm.x = constants->get("norm")->get(0)->asFloat();
    norm.y = constants->get("norm")->get(1)->asFloat();
    norm.z = constants->get("norm")->get(2)->asFloat();
    model->setInitPlaneNorm(norm);
    
    model->setPlaneNorm(norm);
    
    Vec3 exit;
    exit.x = constants->get("exit")->get(0)->asFloat();
    exit.y = constants->get("exit")->get(1)->asFloat();
    exit.z = constants->get("exit")->get(2)->asFloat();
    model->setExitLoc(exit);
    
    model->setExitTex(_assets->get<Texture>("exit"));
    
    std::string meshPath = constants->getString("mesh");
    model->_mesh = PivotMesh::MeshFromOBJ(meshPath);
    
    std::vector<Vec3> locs;
    std::vector<std::shared_ptr<cugl::Texture>> texs;
    std::shared_ptr<cugl::JsonValue> collectibles = constants->get("collectibles");
    int it = constants->getInt("numCol");
    for (int i = 0; i < it; i ++){
        Vec3 loc;
        loc.x = collectibles->get(std::to_string(i+1))->get("loc")->get(0)->asFloat();
        loc.x = collectibles->get(std::to_string(i+1))->get("loc")->get(1)->asFloat();
        loc.x = collectibles->get(std::to_string(i+1))->get("loc")->get(2)->asFloat();
        locs.push_back(loc);
        texs.push_back(_assets->get<Texture>("col" + std::to_string(i)));
    }
    model->setCollectibles(locs, texs);
    
}

