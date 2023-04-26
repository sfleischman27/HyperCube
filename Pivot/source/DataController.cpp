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
    std::string rendmeshPath = constants->getString("render_mesh");

    assetDirectoryPath.append(rendmeshPath);
    assetDirectoryPath = filetool::normalize_path(assetDirectoryPath);
    model->_renderMesh = PivotMesh::MeshFromOBJ(assetDirectoryPath);
    
    assetDirectoryPath = Application::get()->getAssetDirectory();
    std::string colmeshPath = constants->getString("collision_mesh");
    
    assetDirectoryPath.append(colmeshPath);
    assetDirectoryPath = filetool::normalize_path(assetDirectoryPath);
    model->_colMesh = PivotMesh::MeshFromOBJ(assetDirectoryPath);
    
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
    Vec3 exitPos;
    exitPos.x = constants->get("exit")->get(0)->asFloat();
    exitPos.y = constants->get("exit")->get(1)->asFloat();
    exitPos.z = constants->get("exit")->get(2)->asFloat();
    std::shared_ptr<GameItem> exitPtr = std::make_shared<GameItem>(exitPos, "exit", _assets->get<Texture>("exit"));
    model->setExit(exitPtr);
    
    // jack was here :)
    // get the sprites
    std::shared_ptr<cugl::JsonValue> sprites = constants->get("sprites");

    
    // clear glowsticks
    model->clearGlowsticks();
    
    // clear backpack
    model->clearBackpack();
    
    // clear collectibles and init data vectors
    model->clearCollectibles();
    model->clearLights();
    model->clearDecorations();
    
    std::vector<Vec3> col_locs;
    std::vector<std::shared_ptr<cugl::Texture>> col_texs;

    for (int i = 0; i < sprites->size(); i++) {
        auto iscol = sprites->get(std::to_string(i))->get("collectible")->asBool();
        auto tex = sprites->get(std::to_string(i))->getString("tex");
        if (iscol && (tex != "")) {
            // its a collectible
            // get sprite location
            Vec3 loc;
            loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            col_locs.push_back(loc);
            
            // get sprite texture
            std::string texKey = tex;
            std::shared_ptr<Texture> tex = _assets->get<Texture>(texKey);
            col_texs.push_back(tex);

            // does the sprite emit light?
            // TODO make lights for those sprites here
        }
        else if (tex == "") {
            // its ONLY a light with no texture
            Vec3 color;
            color.x = sprites->get(std::to_string(i))->get("color")->get(0)->asFloat();
            color.y = sprites->get(std::to_string(i))->get("color")->get(1)->asFloat();
            color.z = sprites->get(std::to_string(i))->get("color")->get(2)->asFloat();

            Vec3 loc;
            loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();

            float intensity = sprites->get(std::to_string(i))->get("intense")->asFloat();

            GameModel::Light light = GameModel::Light(color, intensity, loc);
            model->_lights.push_back(light);

        }
        
        else {
            // its a decoration
            Vec3 loc;
            loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            //get texture
            auto texkey = sprites->get(std::to_string(i))->getString("tex");

            std::shared_ptr<GameItem> decPtr = std::make_shared<GameItem>(loc, "deco"+std::to_string(i), _assets->get<Texture>(texkey));
            model->_decorations.push_back(decPtr);
        }
    }

    model->setCollectibles(col_locs, col_texs);
    
    // get and set level id
    std::string level_id = constants->getString("level_id");
    model->setName(level_id);
    
    return true;
}


void DataController::setupSave(std::string dir, bool exists){
    if(exists){ // save file already exists
        // make a reader
        std::shared_ptr<JsonReader> read = JsonReader::alloc(dir);
        // get the save json
        _save = std::make_shared<JsonValue>(JsonValue());
        _save->initWithJson(read->readJsonString());
    } else{ // no save file
        // make a save file
        filetool::file_create(dir);
        // make an empty json
        _save = std::make_shared<JsonValue>(JsonValue());
    }
    // make a writer
    _write = JsonWriter::alloc(dir);
}

// Note: dir includes "save.json"
void DataController::createSaveFile(std::string dir){
    // make an empty json
    _save = std::make_shared<JsonValue>(JsonValue());
    // make a save file
    filetool::file_create(dir);
    // make a json writer for the save file
    _write = JsonWriter::alloc(dir);
    // make curr_level = "debug_0000"
    _save->appendValue("curr_level", "debug_0000");
    // write the json value to the file
    _write->writeJson(_save);
}

//TODO: make init of save file a part of init of the gameplay controller
void DataController::updateSaveFile(const std::shared_ptr<GameModel>& model){
    // update curr_level
    _save->get("curr_level")->set(model->getName());
    
    // TODO: Implement if we want to enable resuming your current level
    // update player_loc
    // update norm
    // update collectibles
    // update glowsticks
    // update unlocked_levels
}

/**
 {
     "player_loc": [10,0,0],
     "norm": [0,0,0],
     "curr_level": "level2",
     "collectibles": [2, 3],
     "glowsticks": {
       "0": {
         "loc": [1,0,0]
       },
   "1": {
         "loc": [4,0,0]
       }
     },
     "past_levels": {
         "level1": {
            "collectibles": [0, 2],
         }
     },
     "volume": 0.4
 }
 */
