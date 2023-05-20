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
    //CULog("%s", Application::get()->getAssetDirectory().c_str());
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

    model->shadeDepth = constants->get("shade_depth")->asFloat();

    auto shade_color = cugl::Color4f();
    shade_color.r = constants->get("shade_color")->get(0)->asFloat();
    shade_color.g = constants->get("shade_color")->get(1)->asFloat();
    shade_color.b = constants->get("shade_color")->get(2)->asFloat();
    model->shadeColor = shade_color;

    if (constants->has("ambient_color")) {
        auto ambient_color = cugl::Color4f();
        ambient_color.r = constants->get("ambient_color")->get(0)->asFloat();
        ambient_color.g = constants->get("ambient_color")->get(1)->asFloat();
        ambient_color.b = constants->get("ambient_color")->get(2)->asFloat();
        ambient_color.a = constants->get("ambient_color")->get(3)->asFloat();
        model->ambientLight = ambient_color;
    }

    auto bg_color = cugl::Color4f();
    bg_color.r = constants->get("bg_color")->get(0)->asFloat();
    bg_color.g = constants->get("bg_color")->get(1)->asFloat();
    bg_color.b = constants->get("bg_color")->get(2)->asFloat();
    model->bgColor = bg_color;

    if (constants->has("cutline_color")) {
        auto cutline_color = cugl::Color4f();
        cutline_color.r = constants->get("cutline_color")->get(0)->asFloat();
        cutline_color.g = constants->get("cutline_color")->get(1)->asFloat();
        cutline_color.b = constants->get("cutline_color")->get(2)->asFloat();
        model->cutLineColor = cutline_color;
    }
    
    if (constants->has("cutfill_color")) {
        auto cutfill_color = cugl::Color4f();
        cutfill_color.r = constants->get("cutfill_color")->get(0)->asFloat();
        cutfill_color.g = constants->get("cutfill_color")->get(1)->asFloat();
        cutfill_color.b = constants->get("cutfill_color")->get(2)->asFloat();
        model->cutFillColor = cutfill_color;
    }

    if (constants->has("bg_image") ) {
        std::string texkey = constants->get("bg_image")->asString();
        model->backgroundPic = _assets->get<Texture>(texkey);
    }
    else {
        model->backgroundPic = _assets->get<Texture>("background_default");
    }

    // get the number of glowsticks
    model->_numGlowsticks = constants->get("glowsticks")->asInt();
    
    // get the sprites
    std::shared_ptr<cugl::JsonValue> sprites = constants->get("sprites");

    // remove any active popups
    model->clearPopups();
    // remove any active messages
    model->clearMessages();
    
    // clear glowsticks
    model->clearGlowsticks();

    // clear backpack
    model->clearExpectedCol();
    
    // clear expected collectibles
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
        auto isbill = sprites->get(std::to_string(i))->get("billboard")->asBool();
        auto isemit = sprites->get(std::to_string(i))->get("emissive")->asBool();

        auto tex = sprites->get(std::to_string(i))->getString("tex");

        if (iscol && (tex != "")) {
            // its a collectible
            // get sprite location
            Vec3 loc;
            loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            col_locs.push_back(loc);

            Vec3 norm;
            norm.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            norm.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            norm.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            // TODO: convert normal to an angle,
            // use angle to offset the rotating sprite index so they dont all look the same

            //get sprite scale
            float scale = sprites->get(std::to_string(i))->get("scale")->asFloat();
            // TODO use scale to scale sprites differently @matt

            // get sprite texture
            std::string texKey = tex;
            std::shared_ptr<Texture> tex = _assets->get<Texture>(texKey);
            std::shared_ptr<Texture> normaltex = _assets->get<Texture>(texKey + "-normal");
            col_texs.push_back(tex);
            col_normal_texs.push_back(normaltex);

            // does the sprite emit light?
            auto haslight = !sprites->get(std::to_string(i))->get("color")->isNull();
            if (haslight) {
                Vec3 color;
                color.x = sprites->get(std::to_string(i))->get("color")->get(0)->asFloat();
                color.y = sprites->get(std::to_string(i))->get("color")->get(1)->asFloat();
                color.z = sprites->get(std::to_string(i))->get("color")->get(2)->asFloat();

                float intensity = sprites->get(std::to_string(i))->get("intense")->asFloat();
                float radius = sprites->get(std::to_string(i))->get("radius")->asFloat(); //falloff
                // TODO make lights for those sprites here
                // those lights need to disappear when the collectible is collected
                // same as glowsticks @jolene
            }

            //TODO if(isemit) put it in the emissive collectibles
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
            float radius = sprites->get(std::to_string(i))->get("radius")->asFloat(); //falloff
            //TODO @matt how to make lights with falloff?

            GameModel::Light light = GameModel::Light(color, intensity, loc, radius);
            model->_lights.push_back(light);

        }

        else if (isbill) {
            // its a decoration
            Vec3 loc;
            loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            
            Vec3 norm;
            norm.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            norm.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            norm.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            // TODO: convert normal to an angle,
            float offsetAngle = getOffsetAngleDeg(norm);
            // use angle to offset the rotating sprite index so they dont all look the same
            
            //get sprite scale
            float scale = sprites->get(std::to_string(i))->get("scale")->asFloat();
            // TODO use scale to scale sprites differently @matt
            // does the sprite emit light?
            auto haslight = !sprites->get(std::to_string(i))->get("color")->isNull();
            if (haslight) {
                Vec3 color;
                color.x = sprites->get(std::to_string(i))->get("color")->get(0)->asFloat();
                color.y = sprites->get(std::to_string(i))->get("color")->get(1)->asFloat();
                color.z = sprites->get(std::to_string(i))->get("color")->get(2)->asFloat();
                
                float intensity = sprites->get(std::to_string(i))->get("intense")->asFloat();
                float radius = sprites->get(std::to_string(i))->get("radius")->asFloat(); //falloff
                // TODO make lights for those sprites here
                // These could just go in the scene bc they never disappear
            }
            
            //get texture
            auto texkey = sprites->get(std::to_string(i))->getString("tex");
            std::shared_ptr<GameItem> decPtr = std::make_shared<GameItem>(loc, "deco" + std::to_string(i), _assets->get<Texture>(texkey), offsetAngle, scale);
            decPtr->setIsemit(isemit);
            decPtr->rotateSpriteSheet = SpriteSheet::alloc(_assets->get<Texture>(texkey), 6, 6);
            
            if(_assets->get<Texture>(texkey + "-normal") != nullptr){
                decPtr->rotateNormalSpriteSheet = SpriteSheet::alloc(_assets->get<Texture>(texkey), 6, 6);
            }
            else {
                decPtr->setIsemit(true);
            }
            
            model->_decorations.push_back(decPtr);
            
        }
        else {
            // its a poster
            Vec3 loc;
            loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();

            Vec3 norm;
            norm.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
            norm.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
            norm.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();
            // TODO: convert normal to an angle,
            float offsetAngle = getOffsetAngleDeg(norm);
            // use normal to orient poster

            //get sprite scale
            float scale = sprites->get(std::to_string(i))->get("scale")->asFloat();
            // TODO use scale to scale sprites differently @matt

            //get texture
            auto texkey = sprites->get(std::to_string(i))->getString("tex");


            // does the sprite emit light?
            auto haslight = !sprites->get(std::to_string(i))->get("color")->isNull();
            if (haslight){
                Vec3 color;
                color.x = sprites->get(std::to_string(i))->get("color")->get(0)->asFloat();
                color.y = sprites->get(std::to_string(i))->get("color")->get(1)->asFloat();
                color.z = sprites->get(std::to_string(i))->get("color")->get(2)->asFloat();

                float intensity = sprites->get(std::to_string(i))->get("intense")->asFloat();
                float radius = sprites->get(std::to_string(i))->get("radius")->asFloat(); //falloff
                // TODO make lights for those sprites here @jolene
            }
            //TODO add poster to model
            std::shared_ptr<GameItem> posPtr = std::make_shared<GameItem>(loc, "poster" + std::to_string(i), _assets->get<Texture>(texkey), offsetAngle, scale);
            posPtr->setIsemit(isemit);
            posPtr->setNormal(norm);
            posPtr->rotateSpriteSheet = SpriteSheet::alloc(_assets->get<Texture>(texkey), 6, 6);
            
            if(_assets->get<Texture>(texkey + "-normal") != nullptr){
                posPtr->rotateNormalSpriteSheet = SpriteSheet::alloc(_assets->get<Texture>(texkey), 6, 6);
            }
            else {
                posPtr->setIsemit(true);
            }
            model->_posters.push_back(posPtr);
            //TODO if(isemit) put it in the emissive posters
        }
    }
    if (col_locs.size() > 0) {
        model->_nav_target = col_locs[0];
    }
    else {
        model->_nav_target = model->_exit->getPosition();
    }
    model->setCollectibles(col_locs, col_texs, col_normal_texs);


    // emitters (sound points)
    std::shared_ptr<cugl::JsonValue> emitters = constants->get("sounds");
    for (int i = 0; i < emitters->size(); i++) {
        //get location
        Vec3 loc;
        loc.x = sprites->get(std::to_string(i))->get("loc")->get(0)->asFloat();
        loc.y = sprites->get(std::to_string(i))->get("loc")->get(1)->asFloat();
        loc.z = sprites->get(std::to_string(i))->get("loc")->get(2)->asFloat();

        //get the sound
        std::string sound = emitters->get(std::to_string(i))->get("sound")->asString();

        //get the radius
        float radius = emitters->get(std::to_string(i))->get("radius")->asFloat();

        //TODO @Gordi ^^^ use these, make emitters
        // rn the sounds are named sound_0 and sound_1, you will need to change them in the level json
        // we need to meet about what makes what sounds, and we can set this up properly later

    }


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
                std::string image = triggers->get(std::to_string(i))->getString("image");
                auto args = TriggerArgs();
                args.popup = model->_popup;
                args.image = image;
                trig->registerEnterCallback(Trigger::showPopup, args);
                trig->registerInBoundsCallback(Trigger::showPopup, args);
                trig->registerExitCallback(Trigger::stopPopups, args);
            }
            else if (trig_type == "MESSAGE") {
                std::string message = triggers->get(std::to_string(i))->getString("message");
                auto args = TriggerArgs();
                args.messages = model->_messages;
                args.text = message;
                trig->registerEnterCallback(Trigger::showMessage, args);
                trig->registerInBoundsCallback(Trigger::showMessage, args);
                trig->registerExitCallback(Trigger::stopMessages, args);
            }
            else if (trig_type == "EXITREGION") {
                auto args = TriggerArgs();
                args.messages = model->_messages;
                args.text = "I FEEL LIKE I'M MISSING SOMETHING...";
                trig->registerEnterCallback(Trigger::showExitMess, args);
                trig->registerInBoundsCallback(Trigger::showExitMess, args);
                trig->registerExitCallback(Trigger::stopMessages, args);

                // TODO @sarah you wanted this to check if you have enough collectibles
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
