//
//  GameplayController.cpp
//  Platformer
//
//  Controls the level play loop
//
//  Created by Sarah Fleischman on 2/23/23.
//

#include "GameplayController.h"
#include <cugl/cugl.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>

using namespace cugl;

/** This is adjusted by screen aspect ratio to get the height */
// TODO: fix dimentions to be choosen from device -Sarah
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

// TODO: make this dependent on scene width and height -Sarah
/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192
/** Threshold of the collecting distance */
#define COLLECTING_DIST   15
/** Threshold of the reaching exit distance */
#define EXITING_DIST   17
/** Number of glowsticks allowed to put */
#define NUM_GLOWSTICKS 4
/** Glowstick pickup distance*/
#define PICKING_DIST   15
/** Scale from player image to capsule */
#define CAP_SCALE   1.1f
/** Scale player capsule width */
#define WIDTH_SCALE   2.00f
/** Width of the player capsule */
#define PLAYER_WIDTH   10.0f
/** Height of the player capsule*/
#define PLAYER_HEIGHT  10.0f

#pragma mark Sound Constants
/** Cooldown (in animation frames) for playing the walking sfx */
#define WALK_COOLDOWN   5

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameplayController::GameplayController() : Scene2() {
    _model = nullptr;
    _worldnode = nullptr;
    _debugnode = nullptr;
    _debug = false;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameplayController::dispose() {
    if(_active){
        _input->dispose();
        _physics->dispose();
        _worldnode = nullptr;
        _debugnode = nullptr;
        _model = nullptr;
        Scene2::dispose();
    }
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 *
 * @return true if the controller is initialized properly, false otherwise.
 */

/*
bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Size& displaySize) {
    std::shared_ptr<SoundController> sound = std::make_shared<SoundController>();
    sound->init(assets);
    return init(assets, displaySize, sound);
}*/

bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Size& displaySize, std::shared_ptr<SoundController> sound) {
    _pipeline = std::make_shared<RenderPipeline>(SCENE_WIDTH, displaySize, assets);
    
    return init(assets,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT), sound);
}

/*
bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect) {
    std::shared_ptr<SoundController> sound = std::make_shared<SoundController>();
    sound->init(assets);
    return init(assets, rect, sound);
}*/

bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect, std::shared_ptr<SoundController> sound) {
    
    _state = NONE;
    
    // setup assets
    _dimen = computeActiveSize();
    
    Vec2 offset((_dimen.width-SCENE_WIDTH)/2.0f,(_dimen.height-SCENE_HEIGHT)/2.0f);

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(_dimen)) {
        return false;
    }
    
    _assets = assets;
    
    //set up the game model
    _model = std::make_shared<GameModel>(GameModel());
    _data = std::make_shared<DataController>(DataController());
    _data->init(_assets);
    
    //set up save file
    std::string savePath = Application::get()->getSaveDirectory();
    savePath.append("save.json");
    savePath = filetool::normalize_path(savePath);
    //_data->setupSave(savePath, filetool::file_exists(savePath));

    //set up the plane controller
    _plane = std::make_shared<PlaneController>();
    
#pragma mark SCENE GRAPH SETUP
    
    //set up physics world
    _physics = std::make_shared<PhysicsController>();
    _physics->init(_dimen, rect, SCENE_WIDTH);
    _physics->getWorld()->onBeginContact= [this](b2Contact* contact) {
      beginContact(contact);
    };
    _physics->getWorld()->onEndContact = [this](b2Contact* contact) {
      endContact(contact);
    };
    
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_physics->getScale()); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
    setDebug(_debug);

#pragma mark ADD UI
    
    _assets->loadDirectory("json/pivot_gameUI.json");
    
    auto layer = _assets->get<cugl::scene2::SceneNode>("lab");
    layer->setContentSize(_dimen);
    layer->doLayout();
    
    auto kids = layer->getChildren()[0];
    std::vector<std::string> butts = std::vector<std::string>();
    butts.push_back("jumpB");
    butts.push_back("leftB");
    butts.push_back("rightB");
    butts.push_back("glowstickB");
    butts.push_back("exitB");
    
    //initialize the game scene buttons
    for(int i = 0; i < butts.size(); ++i) {
        std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(kids->getChildByName(butts[i]));
        _buttons[butt->getName()] = butt;
    }
    
    _buttons["exitB"]->addListener([this](const std::string& name, bool down) {
        if (down) {
            _state = State::QUIT;
        }
    });
    
    _model->_glowstickCounter = std::dynamic_pointer_cast<scene2::Label>( _buttons["glowstickB"]->getChildByName("label"));
    
    _model->_compassNum = std::dynamic_pointer_cast<scene2::Label>(kids->getChildByName("compassNum"));

    _model->_navigator = std::dynamic_pointer_cast<scene2::Button>(kids->getChildByName("navigator"));
    
    addChild(layer);
    setActive(false);
    
    //set up the input handler
    _input = std::make_shared<InputController>();
    _input->init(getBounds(),_buttons["jumpB"], _buttons["leftB"], _buttons["rightB"],_buttons["glowstickB"]);

#pragma mark ADD PLAYER
    Vec2 dudePos = Vec2::ZERO;
    prevPlay2DPos = dudePos;
    
    
    std::shared_ptr<Texture> image = assets->get<Texture>(DUDE_TEXTURE);
    float height = image->getSize().height/CAP_SCALE;
    float width = height/WIDTH_SCALE;
    _model->setPlayer(PlayerModel::alloc(dudePos, Size(width, height)));
    
    int _framesize = 16;
    int _framecols = 4;
    int rows = _framesize/_framecols;

    auto sheet = SpriteSheet::alloc(assets->get<Texture>("player-walk"), _framecols, _framecols);
    auto normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-walk-normal"), _framecols, _framecols);
    
//    _model->_player->setSprite(SpriteSheet::alloc(sheet, rows, _framecols, _framesize));
//    _model->_player->setNormalSprite(SpriteSheet::alloc(normalSheet, rows, _framecols, _framesize));
    
    _model->_player->spriteSheets.emplace("walk", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-run"), _framecols, _framecols);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-run-normal"), _framecols, _framecols);
    
    _model->_player->spriteSheets.emplace("run", std::make_pair(sheet, normalSheet));

    sheet = SpriteSheet::alloc(assets->get<Texture>("player-jump"), 4, 3);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-normal"), 4, 3);
    
    _model->_player->spriteSheets.emplace("jump", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-idle"), 1, 1);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-idle-normal"), 1, 1);
    
    _model->_player->spriteSheets.emplace("idle", std::make_pair(sheet, normalSheet));
    
    _model->_player->setSprite(sheet);
    _model->_player->setNormalSprite(normalSheet);
    

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    _model->_player->setSceneNode(sprite);
    _model->_player->setDebugColor(DEBUG_COLOR);

    lastStablePlay2DPos = dudePos;
        
    addObstacle(_model->_player, true);
    
    addChild(_worldnode);
    addChild(_debugnode);

#pragma mark SOUND SETUP
    _sound = sound;
    //_sound->init(assets);
    //_sound->streamSounds({ "loopingtest_m", "cave_p"}, 1.0, true);

    
    _active = true;
    
    return true;
    
}

/**
* Generates obstacle instances from the given cut, specified by the list of Poly2s given by the GameModel _model
* (should I split this into 2 methods where I have one taking a cut for code cleanliness?)
*/
void GameplayController::createCutObstacles(){
    // TODO: clean this and add function comment -Gordi
    //remove previous poly nodes
    //removePolyNodes();
    createObstacleFromPolys(_model->getCut());
}

/**
 * Generates obstacle instances from a list of Poly2s. Each poly2 represents a new obstacle.
 * @param polys the list of Poly2s
 */
void GameplayController::createObstacleFromPolys(std::vector<cugl::Poly2> polys){
    //std::shared_ptr<Rect> bounds = std::make_shared<Rect>(Vec2::ZERO, getSize() / _physics->getScale());
    
    //std::shared_ptr<scene2::SceneNode> cutnode = scene2::SceneNode::alloc();
    
    for(Poly2 p : polys){
        std::vector<cugl::Vec2> vertices = p.getVertices();
        std::shared_ptr<cugl::physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(p);
        obstacle->setBodyType(b2_staticBody);
        //cutnode = scene2::SceneNode::alloc();
        //_cutnodes.insert(cutnode);
        //_cutobstacles.insert(obstacle);
        addObstacle(obstacle, true);
    }
    
}

/**
 * Removes all the nodes beloning to _polynodes from _worldnodes. In essence, this cleans up all the old collisions and SceneNodes pertaining to a previous cut to make room for the new cut's collisions.
 */
void GameplayController::removePolyNodes(){
    for(std::shared_ptr<cugl::scene2::SceneNode> node : _worldnode->getChildren()){
        
        bool is_in = _cutnodes.find(node) != _cutnodes.end();

        if(is_in){
            _cutnodes.erase(node);
            _worldnode->removeChild(node);
        }
    }
    //commented out because addressed in update with the physics.clear method -- remember to uncomment the cutobstacles insert line if youre uncommenting this again
    /*for(std::shared_ptr<cugl::physics2::Obstacle> obstacle : _physics->getWorld()->getObstacles()){
        
        bool is_in = _cutobstacles.find(obstacle) != _cutobstacles.end();

        //if obstacle is an obstacle generated from the cut (and not something like the player's body)
        if(is_in){
            _cutobstacles.erase(obstacle);
            _physics->markForRemoval(obstacle);
        }
    }*/
    _physics->garbageCollect(); //<- thought i needed this but it's working w it commented out? so im keeping it like this?
}


/**
 * Adds obstacle to both physics world and world node/debug node via an intermediary SceneNode
 * @param obj the obstacle
 * @param node the SceneNode you're adding the obj to (which will then be added to the world+debug scenenodes
 * @param useObjPosition whether you should use the object's local position when adding to scenes. Default is true.
 */
void GameplayController::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            //const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            bool useObjPosition) {
    _physics->getWorld()->addObstacle(obj);
    obj->setDebugColor(DEBUG_COLOR);
    obj->setDebugScene(_debugnode);
        
    // Position the scene graph node (enough for static objects)
      /*if (useObjPosition) {
          node->setPosition(obj->getPosition()/_physics->getScale());
      }*/
    
      //_worldnode->addChild(node);
    
    // Dynamic objects need constant updating
    /*if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_physics->getScale());
            weak->setAngle(obs->getAngle());
        });
    }*/
}


/**
 * Resets the current level
 */
void GameplayController::reset() {
    _state = NONE;
    // reset physics
    _physics->clear();
    // reset model
    _data->resetGameModel(_model->getName(), _model);
    // reset collectibles
    resetCollectibleUI(_model->getColNum());
    // add person object
    _model->_player->setPosition(Vec2::ZERO);
    prevPlay2DPos = Vec2::ZERO;
    _physics->getWorld()->addObstacle(_model->_player);
    // change plane for new model
    _plane->init(_model);
    _plane->calculateCut();
    // update physics for new cut
    createCutObstacles();
    _physics->update(0);
    // setup graphics pipeline
    _pipeline->sceneSetup(_model);
    _model->updateCompassNum();
    

    lastStablePlay2DPos = _model->_player->getPosition();
}

/**
 * Loads a new level into the game model
 *
 * @param name    the name of the level to be loaded (key in assets file)
 */
void GameplayController::load(std::string name){
    _state = NONE;
    // reset physics
    _physics->clear();
    // update model
    _data->loadGameModel(name, _model);
    // reset collectibles
    resetCollectibleUI(_model->getColNum());
    // add person object
    _model->_player->setPosition(Vec2::ZERO);
    prevPlay2DPos = Vec2::ZERO;
    _physics->getWorld()->addObstacle(_model->_player);
    // change plane for new model
    _plane->init(_model);
    _plane->calculateCut();
    // update physics for new cut
    createCutObstacles();
    _physics->update(0);
    // setup graphics pipeline
    _pipeline->sceneSetup(_model);
    
    _sound->streamSounds({ "cave_m" }, 1.0, true);
    //_sound->streamSounds({ "end" }, 1.0, true);
    _model->updateCompassNum();
    
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void GameplayController::setActive(bool value){
    Scene2::setActive(value);
    if(value){
        for(auto it = _buttons.begin(); it != _buttons.end(); ++it) {
            it->second->activate();
        }
        // turn on the render pipeline stuff
        glEnable(GL_DEPTH_TEST);
        // reset the state to none (fixes pauses for quitting)
        _state = State::NONE;
    } else {
        for(auto it = _buttons.begin(); it != _buttons.end(); ++it) {
            it->second->deactivate();
        }
        // turn off the render pipeline stuff
        glDisable(GL_DEPTH_TEST);
    }
}

/**
 * Sets collectible UI to only show the correct number of empty collectibles for the level
 */
void GameplayController::resetCollectibleUI(int col){
    if(col == 1 | col == 3){ // odd
        // turn off the even inventory and turn on the odd
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd")->setVisible(true);
        
        // turn off full items
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-one")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-two")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-three")->setVisible(false);
    } else { // even
        // turn on the even inventory and turn off the odd
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd")->setVisible(false);
        
        // turn off full items
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-one")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-two")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-three")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-four")->setVisible(false);
    }
    
    if(col == 1){
        // turn off items 1 and 3
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-one")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-three")->setVisible(false);
        
        // turn on item 2
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-two")->setVisible(true);
        
    } else if (col == 2){
        // turn off items 1 and 4
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-one")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-four")->setVisible(false);
        
        // turn on items 2 and 3
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-two")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-three")->setVisible(true);

    } else if (col == 3){
        // turn on items 1, 2 and 3
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-one")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-two")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-three")->setVisible(true);
        
    } else { // 4 collectibles
        // turn on items 1, 2, 3, and 4
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-one")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-two")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-three")->setVisible(true);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-four")->setVisible(true);
    }
}

void GameplayController::collectUI(int col, int got){
    switch(col) {
        case 1:
            if(got >= 1){
                // collect center of odd inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-two")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-two")->setVisible(true);
            }
            break;
        case 2:
            if(got >= 1){
                // collect center left of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-two")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-two")->setVisible(true);
            } else if(got >= 2){
                // collect center right of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-three")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-three")->setVisible(true);
            }
            break;
        case 3:
            if(got >= 1){
                // collect left of odd inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-one")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-one")->setVisible(true);
            } else if(got >= 2){
                // collect center of odd inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-two")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-two")->setVisible(true);
            } else if(got >= 3){
                // collect right of odd inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-three")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-three")->setVisible(true);
            }
            break;
        case 4:
            if(got >= 1){
                // collect left of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-one")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-one")->setVisible(true);
            } else if(got >= 2){
                // collect center left of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-two")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-two")->setVisible(true);
            } else if(got >= 3){
                // collect center right of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-three")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-three")->setVisible(true);
            } else if(got >= 4){
                // collect right of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-four")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-four")->setVisible(true);
            }
            break;
    }
}

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
float saveFloat = 0.0;
void GameplayController::update(float dt) {
    //CULog("loop on: %d", cugl::AudioEngine::get()->getMusicQueue()->isLoop());
    
    _model->_justFinishRotating = false;
#pragma mark INPUT
    _input->update(dt);
    
    if (_input->didDebug()) {
        setDebug(!isDebug());
        CULog("Debug mode is: %d, visibility: %d", isDebug(), _debugnode->isVisible());
        
    }

    //kill the player if marked dead
    if (_model->_player->isDead()) {
        _model->_player->setPosition(lastStablePlay2DPos);
        _model->_player->setDead(false);
    }

    //if (_input->didIncreaseCut() && (_model->_player->getX() > DEFAULT_WIDTH/2 - 1) && (_model->_player->getX() < DEFAULT_WIDTH/2 + 1)){
    //    if (_model->_player->isGrounded() && _input->didIncreaseCut()) {
    if (!_input->isRotating) {
        saveFloat = 0.0;
        _input->cutFactor = 0.0;
    }
    if (_input->isRotating) {
//        _plane->rotateNorm(_input->cutFactor/15000);
        //createCutObstacles();
        _plane->rotateNorm((_input->cutFactor - saveFloat)/1000);
        _model->updateCompassNum();
        
        saveFloat = _input->cutFactor;
        _rotating = true;
    }
    
    //else if (_input->didDecreaseCut() && (_model->_player->getX() > DEFAULT_WIDTH/2 - 1) && (_model->_player->getX() < DEFAULT_WIDTH/2 + 1)) {

//    else if (_model->_player->isGrounded() && _input->didDecreaseCut()) {
//        _plane->rotateNorm(_input->cutFactor/15000);
//        //createCutObstacles();
//        _rotating = true;
//    }
    //else if (_input->didKeepChangingCut() && (_model->_player->getX() > DEFAULT_WIDTH/2 - 1) && (_model->_player->getX() < DEFAULT_WIDTH/2 + 1)) {
    else if (_input->didKeepChangingCut()) {
        _plane->rotateNorm(_input->getMoveNorm() * 1.75);
        _model->updateCompassNum();
        
        std::cout<<"here:"<<_input->getMoveNorm()<<std::endl;
        //createCutObstacles();
        _rotating = true;
    }
    else {
        if (_rotating) {
            _physics->clear();
            _model->_player->setPosition(Vec2::ZERO);
            prevPlay2DPos = Vec2::ZERO;
            //            _physics->getWorld()->addObstacle(_model->_player);
            //            _plane->movePlaneToPlayer();
            //            _plane->calculateCut();//calculate cut here so it only happens when we finish rotating
            //            //_plane->debugCut(100);// enable this one to make a square of size 10 x 10 as the cut, useful for debugging
            //            createCutObstacles();
            _rotating = false;
            _model->_justFinishRotating = true;
        }
        if (_model->_justFinishRotating) {
            _physics->getWorld()->addObstacle(_model->_player);
            _plane->movePlaneToPlayer();
            _plane->calculateCut();//calculate cut here so it only happens when we finish rotating
            //_plane->debugCut(100);// enable this one to make a square of size 10 x 10 as the cut, useful for debugging
            createCutObstacles();
            lastStablePlay2DPos = _model->_player->getPosition();
        }
        _physics->update(dt);
        // std::cout<<"curr velocity (x,y): " << _model->_player->getVelocity().x << "," << _model->_player->getVelocity().y << std::endl;
    }
    
#pragma mark COLLECTIBLES
    for (auto itr = _model->_collectibles.begin(); itr != _model->_collectibles.end(); itr++) {
        if (itr->second.canBeSeen(_model->getPlayer3DLoc(), _model->getPlaneNorm()) &&
            _model->getPlayer3DLoc().distance(itr->second.getPosition())<= COLLECTING_DIST){
            itr->second.setCollected(true);
            _model->_backpack.insert(itr->first);
        }
    }
    
    // update collectible UI
    collectUI(_model->getColNum(), _model->getCurrColNum());
    
    if((_model->_exit->canBeSeen(_model->getPlayer3DLoc(),_model->getPlaneNorm()))
       &&(_model->getPlayer3DLoc().distance(_model->_exit->getPosition()) <= EXITING_DIST)) {
        if (_model->checkBackpack()) {
            _model->_endOfGame = true;
            _state = END;
        }
        else{
            // TODO: maybe saying find lost colletibles or something? - Sarah
        }
    }
    
#pragma mark Glowsticks
    if (_input->didGlowstick()) {
        Vec3 player3DPos = _model->getPlayer3DLoc();
        for(auto g =_model->_glowsticks.begin(); g!=_model->_glowsticks.end();){
            if (g->getPosition().distance(player3DPos) <= PICKING_DIST) {
                _model->_lightsFromItems.erase(std::string(g->getPosition()));
                g = _model->_glowsticks.erase(g);
                _model->updateGlowstickCount();
                _pickupGlowstick = true;
            }
            else{
                ++g;
            }
        }
        if (!_pickupGlowstick && _model->_glowsticks.size() < NUM_GLOWSTICKS) {
            auto g = Glowstick(player3DPos-(_model->getPlaneNorm()*10));
            // std::cout << "here name:" <<std::string(g.getPosition()) <<std::endl;
            _model->_glowsticks.push_back(g);
            _model->updateGlowstickCount();
            _model->_lightsFromItems[std::string(g.getPosition())] = GameModel::Light(g.getColor(), g.getIntense(), g.getPosition());
        }
        
        _pickupGlowstick = false;
    }
    
    
#pragma mark PLAYER
    
    _model->_player->setMovement(_input->getHorizontal() * _model->_player->getForce());
    
    _model->_player->setJumping(_input->didJump());
    
    _model->_player->setRunning(_input->isRun());
    
    _model->_player->applyForce();
    
    currPlay2DPos = _model->_player->getPosition();
    Vec2 displacement = currPlay2DPos - prevPlay2DPos;
    updatePlayer3DLoc(displacement);
    prevPlay2DPos = currPlay2DPos;

    // update triggers
    for (auto trig : _model->_triggers) {
        trig->update(_model->getPlayer3DLoc());
    }

    //update navigator
    _model->updateNavigator();
    
#pragma mark PLANE
    
    
#pragma mark SOUND CUES
    
    if(_model->_player->_jumpCue){
        //_sound->playSound("jump", 0.5, false);
        /*if(_model->_player->isGrounded()){
            _sound->playSound("jump_land", 0.5, false);
        }*/
        _model->_player->_jumpCue = false;
    }
    
    if(_model->_player->_walkCue){
        float walkNumber = rand() % 12;
        //CULog("walkNumber = %f", walkNumber);
        
        if(walkNumber < 3){
            //CULog("walk 1");
            _sound->playSound("walk_1", 1, false);
        } else {
            if(walkNumber < 6) {
                //CULog("walk 2");
                _sound->playSound("walk_2", 1, false);
            } else {
                if(walkNumber < 9) {
                    //CULog("walk 3");
                    _sound->playSound("walk_3", 1, false);
                } else {
                    //CULog("walk 4");
                    _sound->playSound("walk_4", 1, false);
                }            }
        }
        _model->_player->_walkCue = false;
    }
    
}

/**
 * Draws all this scene to the given SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch.  By overriding it, you can do custom drawing
 * in its place.
 *
 * @param batch     The SpriteBatch to draw with.
 */
void GameplayController::render(const std::shared_ptr<cugl::SpriteBatch>& batch) {
    _pipeline->render(_model);
    Scene2::render(batch);
}

Size GameplayController::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width/dimen.height;
    float ratio2 = ((float)SCENE_WIDTH)/((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) { //dimen has a higher width than scene_width : height ratio
        dimen *= SCENE_HEIGHT/dimen.height;
    } else {
        dimen *= SCENE_WIDTH/dimen.width;
    }
    
    return dimen;
}

/**Get the 2d coordinates relative to the cut plane of a 3d location
* it also returns the projected distance from that point to the cut plane, which can be used to threshold drawing of an object at that location
* RETURN: screen coordinates and projection distance pairs are returned as a std::tuple<Vec2,float>*/
std::tuple<cugl::Vec2, float> GameplayController::ScreenCoordinatesFrom3DPoint(cugl::Vec3 location) {
    // TODO: clean function comment -Jack
    //dot the point onto the plane normal to get the distance from the cut plane
    auto dist = location.dot(_model->getPlaneNorm());
    // get the point projected onto the plane basis vectors (unit_z is always y-axis and x-axis is to the right)
    auto xcoord = location.dot(cugl::Vec3(0,0,1).cross(_model->getPlaneNorm().negate()).normalize());
    auto ycoord = location.dot(cugl::Vec3(0,0,1));
    auto coords = cugl::Vec2(xcoord, ycoord);

    return(std::tuple<cugl::Vec2, float>(coords, dist));
}

void GameplayController::updatePlayer3DLoc(Vec2 displacement) {
    float x = displacement.x;
    float y = displacement.y;
    /*std::cout<<"_plane->getBasisRight().x: " << _plane->getBasisRight().x << std::endl;
    std::cout<<"_plane->getBasisRight().y: " << _plane->getBasisRight().x << std::endl;
    std::cout<<"_plane->getBasisRight().z: " << _plane->getBasisRight().x << std::endl;
    std::cout<<"2d-displacement x: " <<x<<std::endl;
    std::cout<<"2d-displacement y: " <<y<<std::endl;*/
    Vec3 temp = x * _plane->getBasisRight();
    Vec3 displacementIn3D = Vec3(temp.x, temp.y, y);
    /*std::cout<<"3d-displacement x: " <<displacementIn3D.x<<std::endl;
    std::cout<<"3d-displacement y: " <<displacementIn3D.y<<std::endl;
    std::cout<<"3d-displacement z: " <<displacementIn3D.z<<std::endl;
    std::cout<<"here x: " <<(_model->getPlayer3DLoc() + displacementIn3D).x<<std::endl;
    std::cout<<"here y: " <<(_model->getPlayer3DLoc() + displacementIn3D).y<<std::endl;
    std::cout<<"here z: " <<(_model->getPlayer3DLoc() + displacementIn3D).z<<std::endl;*/
    _model->setPlayer3DLoc(_model->getPlayer3DLoc() + displacementIn3D);
}

void GameplayController::beginContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    // See if we have landed on the ground.
    if ((_model->_player->getSensorName() == fd2 && _model->_player.get() != bd1) ||
        (_model->_player->getSensorName() == fd1 && _model->_player.get() != bd2)) {
        _model->_player->setGrounded(true);
        // Could have more than one ground
        _sensorFixtures.emplace(_model->_player.get() == bd1 ? fix2 : fix1);
    }

    // If we hit the "win" door, we are done
//    if((bd1 == _model->_player.get() && bd2 == _goalDoor.get()) ||
//        (bd1 == _goalDoor.get() && bd2 == _model->_player.get())) {
//        setComplete(true);
//    }
}

/**
 * Callback method for the start of a collision
 *
 * This method is called when two objects cease to touch.  The main use of this method
 * is to determine when the characer is NOT on the ground.  This is how we prevent
 * double jumping.
 */
void GameplayController::endContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    if ((_model->_player->getSensorName() == fd2 && _model->_player.get() != bd1) ||
        (_model->_player->getSensorName() == fd1 && _model->_player.get() != bd2)) {
        _sensorFixtures.erase(_model->_player.get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty()) {
            _model->_player->setGrounded(false);
        }
    }
}

void GameplayController::save(int maxLevel) {
    _data->save(maxLevel);
}

int GameplayController::getMaxLevel() {
    return 4; //jacko did this TODO SARAH
    //return _data->getMaxLevel();
}
