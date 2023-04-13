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
 *  Loads a new level (loads new meshes)
 *
 *  @param level    The location of the level json to be loaded
 *  @param model    The game model to load the level data into
 *
 *  @return true if the controller is initialized properly, false otherwise.
 */
bool DataController::loadGameModel(std::string level, const std::shared_ptr<GameModel>& model) {
    
    // get the level json
    std::shared_ptr<cugl::JsonValue> constants = _assets->get<JsonValue>(level);
    
    // load the new meshes
    CULog("%s", Application::get()->getAssetDirectory().c_str());
    std::string assetDirectoryPath = Application::get()->getAssetDirectory();
    std::string meshPath = constants->getString("mesh");
    assetDirectoryPath.append(meshPath);
    model->_mesh = PivotMesh::MeshFromOBJ(assetDirectoryPath);
    
    // call reset game model
    return resetGameModel(level, model);
}

/**
 *  Resets current level (no new meshes)
 *
 *  @param level    The location of the level json to be loaded
 *  @param model    The game model to load the level data into
 *
 *  @return true if the model is initialized properly, false otherwise.
 */
bool DataController::resetGameModel(std::string level, const std::shared_ptr<GameModel>& model){
    
    // get the level json
    std::shared_ptr<cugl::JsonValue> constants = _assets->get<JsonValue>(level);
    
    // get and set player location
    Vec3 playerLoc;
    playerLoc.x = constants->get("player_loc")->get(0)->asFloat();
    playerLoc.y = constants->get("player_loc")->get(1)->asFloat();
    playerLoc.z = constants->get("player_loc")->get(2)->asFloat();
    std::cout<<"init x: " <<playerLoc.x <<std::endl;
    std::cout<<"init y: " <<playerLoc.y<<std::endl;
    std::cout<<"init z: " <<playerLoc.z<<std::endl;
    model->setInitPlayerLoc(playerLoc);
    
    model->setPlayer3DLoc(playerLoc);
    
    // get and set plane normal
    Vec3 norm;
    norm.x = constants->get("norm")->get(0)->asFloat();
    norm.y = constants->get("norm")->get(1)->asFloat();
    norm.z = constants->get("norm")->get(2)->asFloat();
    model->setInitPlaneNorm(norm);
    
    // get and set exit location and texture
    Vec3 exit;
    exit.x = constants->get("exit")->get(0)->asFloat();
    exit.y = constants->get("exit")->get(1)->asFloat();
    exit.z = constants->get("exit")->get(2)->asFloat();
    model->setExitLoc(exit);
    
    model->setExitTex(_assets->get<Texture>("exit"));
    
    // get and set collectibles
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
    
    // clear glowsticks
    model->clearGlowsticks();

    // get and set light sources
    model->clearLights();
    std::shared_ptr<cugl::JsonValue> lights = constants->get("lights");
    it = lights->size();
    for (int i = 0; i < it; i ++){
        Vec3 color;
        color.x = lights->get(std::to_string(i))->get("color")->get(0)->asFloat();
        color.y = lights->get(std::to_string(i))->get("color")->get(1)->asFloat();
        color.z = lights->get(std::to_string(i))->get("color")->get(2)->asFloat();
        
        Vec3 loc;
        loc.x = lights->get(std::to_string(i))->get("loc")->get(0)->asFloat();
        loc.y = lights->get(std::to_string(i))->get("loc")->get(1)->asFloat();
        loc.z = lights->get(std::to_string(i))->get("loc")->get(2)->asFloat();
        
        float intensity = lights->get(std::to_string(i))->get("intensity")->asFloat();
        
        GameModel::Light light = GameModel::Light(color, intensity, loc);
        model->_lights.push_back(light);
    }
    
    // get and set level id
    std::string level_id = constants->getString("level_id");
    model->setName(level_id);
    
    return true;
}



