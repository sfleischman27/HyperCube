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
    std::shared_ptr<GameItem> exitPtr = std::make_shared<GameItem>(exitPos, "exit", _assets->get<Texture>("goal"));
    model->setExit(exitPtr);
    exitPtr->rotateSpriteSheet = SpriteSheet::alloc(_assets->get<Texture>("goal"), 6, 6);
    exitPtr->rotateNormalSpriteSheet = SpriteSheet::alloc(_assets->get<Texture>("goal-normal"), 6, 6);

    model->backgroundPic = _assets->get<Texture>("space135");
    
    // get the sprites
    std::shared_ptr<cugl::JsonValue> sprites = constants->get("sprites");

    // remove any active popups
    model->clearPopups();
    
    // clear glowsticks
    model->clearGlowsticks();
    
    // clear backpack
    model->clearBackpack();
    
    // clear collectibles and init data vectors
    model->clearCollectibles();
    model->clearLights();
    model->clearDecorations();

    // clear the triggers
    model->clearTriggers();
    
    std::vector<Vec3> col_locs;
    std::vector<std::shared_ptr<cugl::Texture>> col_texs;
    std::vector<std::shared_ptr<cugl::Texture>> col_normal_texs;

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
            std::shared_ptr<Texture> normaltex = _assets->get<Texture>(texKey + "-normal");
            col_texs.push_back(tex);
            col_normal_texs.push_back(normaltex);

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
    if (col_locs.size() > 0) {
        model->_nav_target = col_locs[0];
    }
    else {
        model->_nav_target = model->_exit->getPosition();
    }
    model->setCollectibles(col_locs, col_texs, col_normal_texs);

    // get and set triggers
    std::shared_ptr<cugl::JsonValue> triggers = constants->get("triggers");

    if (triggers != nullptr) {
        for (int i = 0; i < triggers->size(); i++) {
            std::string region_path = triggers->get(std::to_string(i))->getString("mesh");
            std::string assetDirectoryPath = Application::get()->getAssetDirectory();
            assetDirectoryPath.append(region_path);
            assetDirectoryPath = filetool::normalize_path(assetDirectoryPath);
            auto region = PivotMesh::MeshFromOBJ(assetDirectoryPath);
            auto trig = std::make_shared<Trigger>(region);

            std::string trig_type = triggers->get(std::to_string(i))->getString("type");
            if (trig_type == "DEATH") {
                auto args = TriggerArgs();
                args.player = model->_player;
                trig->registerEnterCallback(Trigger::killPlayer, args);
            }
            else if (trig_type == "POPUP"){
                auto args = TriggerArgs();
                args.popup = model->_popup;
                trig->registerEnterCallback(Trigger::showRotate, args);
                trig->registerExitCallback(Trigger::stopPopups, args);
            }
            
            

            model->_triggers.push_back(trig);
        }
    }

    
    
    // get and set level id
    std::string level_id = constants->getString("level_id");
    model->setName(level_id);
    
    return true;
}

// Note: dir includes "save.json"
void DataController::setupSave(std::string dir, bool exists){
    _saveDir = dir;
    // get default save file
    _default = _assets->get<JsonValue>("default_save");
    if(exists){ // save file already exists
        // make a reader
        std::shared_ptr<JsonReader> read = JsonReader::alloc(_saveDir);
        // get the save json
        _save = JsonValue::allocObject();
        _save->initWithJson(read->readJsonString());
    } else{ // no save file
        // make a save file
        filetool::file_create(_saveDir);
        // get default save json
        _save = JsonValue::allocObject();
        _save->initWithJson(_default->toString());
    }
}

void DataController::resetSave(){
    // get default save json
    _save = JsonValue::allocObject();
    _save->initWithJson(_default->toString());
}

void DataController::save(){
    // make a writer
    auto write = JsonWriter::alloc(_saveDir);
    // write the json value to the file
    write->writeJson(_save);
    // close the writer
    write->close();
}
