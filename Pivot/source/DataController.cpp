//
//  DataController.cpp
//  Platformer
//
//  Contains methhods to load data as needed for music and levels
//
//  Created by Jack otto on 2/23/23.
//

#include "DataController.h"

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
    
    std::shared_ptr<cugl::JsonValue> constants = _assets->get<JsonValue>(level);
    
    Vec3 playerLoc;
    playerLoc.x = constants->get("player_loc")->get(0)->asFloat();
    playerLoc.y = constants->get("player_loc")->get(1)->asFloat();
    playerLoc.z = constants->get("player_loc")->get(2)->asFloat();
    std::cout<<"init x: " <<playerLoc.x <<std::endl;
    std::cout<<"init y: " <<playerLoc.y<<std::endl;
    std::cout<<"init z: " <<playerLoc.z<<std::endl;
    model->setInitPlayerLoc(playerLoc);
    
    model->setPlayer3DLoc(playerLoc);
    
    Vec3 norm;
    norm.x = constants->get("norm")->get(0)->asFloat();
    norm.y = constants->get("norm")->get(1)->asFloat();
    norm.z = constants->get("norm")->get(2)->asFloat();
    model->setInitPlaneNorm(norm);
    
    //model->setPlaneNorm(norm);
    
    Vec3 exit;
    exit.x = constants->get("exit")->get(0)->asFloat();
    exit.y = constants->get("exit")->get(1)->asFloat();
    exit.z = constants->get("exit")->get(2)->asFloat();
    model->setExitLoc(exit);
    
    model->setExitTex(_assets->get<Texture>("exit"));
    
    //CULog(std::filesystem::current_path().c_str());
    CULog("%s", Application::get()->getAssetDirectory().c_str());
    std::string assetDirectoryPath = Application::get()->getAssetDirectory();
    std::string meshPath = constants->getString("mesh");
    assetDirectoryPath.append(meshPath);
    model->_mesh = PivotMesh::MeshFromOBJ(assetDirectoryPath);
    
    model->clearCollectibles();
    std::vector<Vec3> locs;
    std::vector<std::shared_ptr<cugl::Texture>> texs;
    std::shared_ptr<cugl::JsonValue> collectibles = constants->get("col");
    int it = collectibles->size();
    for (int i = 0; i < it; i ++){
        Vec3 loc;
        loc.x = collectibles->get(std::to_string(i))->get("loc")->get(0)->asFloat();
        loc.y = collectibles->get(std::to_string(i))->get("loc")->get(1)->asFloat();
        loc.z = collectibles->get(std::to_string(i))->get("loc")->get(2)->asFloat();
        std::string texKey = collectibles->get(std::to_string(i))->getString("tex");
        std::shared_ptr<Texture> tex = _assets->get<Texture>(texKey);
        locs.push_back(loc);
        texs.push_back(tex);
    }
    model->setCollectibles(locs, texs);
    
    model->clearGlowsticks();
    
    std::string level_id = constants->getString("level_id");
    model->setName(level_id);
    
    return true;
}

/**
 * Initializes the gameUI
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool DataController::initGameUI() {
    
    _assets->loadDirectory("json/pivot_gameUI.json");
    
    auto layer = _assets->get<cugl::scene2::SceneNode>("lab");
    
    return true;
}
