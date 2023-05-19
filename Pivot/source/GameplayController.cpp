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
#define COLLECTING_DIST   25
/** Threshold of the reaching exit distance */
#define EXITING_DIST   25
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
/** Maximum portal distance where it will still play noise */
#define MAX_PORTAL_DIST 250.0

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
    _data->setupSave(savePath, filetool::file_exists(savePath));

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
    _layer = layer;
    
    /** set apha to 0.0 so it can fade in */
    auto color = _layer->getColor();
    auto newColor = Color4(color.r, color.g, color.b, 0.0);
    _layer->setColor(newColor);
    
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
    
    // store UI elements for later usage
    _model->_glowstickCounter = std::dynamic_pointer_cast<scene2::Label>( _buttons["glowstickB"]->getChildByName("label"));
    
    _model->_compassNum = std::dynamic_pointer_cast<scene2::Label>(kids->getChildByName("compassNum")->getChildByName("compassnum"));

    _model->_navigator = std::dynamic_pointer_cast<scene2::Button>(kids->getChildByName("navigator"));
    
    _model->_compassSpin = std::dynamic_pointer_cast<scene2::SpriteNode>(kids->getChildByName("compassBar"));
    
    // popups
    _model->_rotatePopup = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("rotateTutorial"));
    
    _model->_collectPopup = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("collectTutorial"));
    
    _model->_glowPopup = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("glowTutorial"));
    
    _model->_jumpPopup = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("jumpTutorial"));
    
    _model->_runPopup = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("runTutorial"));
    
    _model->_walkPopup = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("walkTutorial"));
    
    // messages
    _model->_messScene = std::dynamic_pointer_cast<scene2::SceneNode>(kids->getChildByName("messagePop"));
    
    _model->_messText = std::dynamic_pointer_cast<scene2::Label>(kids->getChildByName("messagePop")->getChildByName("messagePopup")->getChildByName("label"));
    
    // collectibles
    _model->_invent = std::dynamic_pointer_cast<scene2::SceneNode>(_assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory"));
    
    _model->_inventodd = std::dynamic_pointer_cast<scene2::SceneNode>(_assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd"));
    
    _model->initInventAlpha = _model->_invent->getColor().a;
    
    _model->initOddAlpha = _model->_inventodd->getColor().a;
    
    // set compass alpha to 0.0 so it can fade in
    color = _model->_compassSpin->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_compassSpin->setColor(newColor);
    
    // turns off and sets alphas for popups
    resetPopups();
    
    // turn off and set alpha for message
    resetMessages();
    
    // final UI setup
    layer->setContentSize(_dimen);
    layer->doLayout();

    addChild(layer);
    setActive(false);
    
    // get run image scene2s so they can be modified in input
    auto leftRun = std::dynamic_pointer_cast<scene2::SceneNode>(_assets->get<scene2::SceneNode>("lab_gameUIScreen_leftB_dashactive"));
    auto rightRun = std::dynamic_pointer_cast<scene2::SceneNode>(_assets->get<scene2::SceneNode>("lab_gameUIScreen_rightB_dashactive"));
    
    //set up the input handler
    _input = std::make_shared<InputController>();
    _input->init(getBounds(),_buttons["jumpB"], _buttons["leftB"], leftRun, _buttons["rightB"], rightRun, _buttons["glowstickB"]);

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

    sheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-launch"), 7, 7);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-launch-normal"), 7, 7);
    
    _model->_player->spriteSheets.emplace("jump-launch", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-hold"), 7, 7);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-hold-normal"), 7, 7);
    
    _model->_player->spriteSheets.emplace("jump-hold", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-land"), 7, 7);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-jump-land-normal"), 7, 7);
    
    _model->_player->spriteSheets.emplace("jump-land", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-flip"), 7, 7);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-flip-normal"), 7, 7);
    
    _model->_player->spriteSheets.emplace("flip", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-idle"), 1, 1);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-idle-normal"), 1, 1);
    
    _model->_player->setSprite(sheet);
    _model->_player->setNormalSprite(normalSheet);
    
    _model->_player->spriteSheets.emplace("idle", std::make_pair(sheet, normalSheet));
    
    sheet = SpriteSheet::alloc(assets->get<Texture>("player-rotate"), 7, 7);
    normalSheet = SpriteSheet::alloc(assets->get<Texture>("player-rotate-normal"), 7, 7);
    
    _model->_player->rotateSpriteSheet = sheet;
    _model->_player->rotateNormalSpriteSheet = normalSheet;
    
    _model->_player->lastRotateAngle = _model->getGlobalAngleDeg();
    _model->_player->setRotationalSprite(_model->getGlobalAngleDeg());
    
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    _model->_player->setSceneNode(sprite);
    _model->_player->setDebugColor(DEBUG_COLOR);

    lastStablePlay2DPos = dudePos;
        
    addObstacle(_model->_player, true);
    
    addChild(_worldnode);
    addChild(_debugnode);
    
    _initInertia = _model->_player->getInertia();
    _initFriction = _model->_player->getFriction();

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
    // reset popups
    resetPopups();
    // reset messages
    resetMessages();
    // add person object
    _model->_player->setPosition(Vec2::ZERO);
    _model->_player->setVelocity(Vec2::ZERO);
    _model->_player->doneFlipping = false;
    _model->_player->shouldStartFlipping = false;
    _model->_pixelingIn = true;
    _model->_donePixelOut = false;
    _model->_donePixelIn = false;
    _model->_startOfLevel = true;
    _model->_player->setFriction(_initFriction);
    _model->_player->setInertia(_initInertia);
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
    resetCollectibleAlpha();
    _model->_endOfGame = false;
    
    /** set UI apha to 0.0 so it can fade in */
    auto color = _layer->getColor();
    auto newColor = Color4(color.r, color.g, color.b, 0.0);
    _layer->setColor(newColor);

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
    // reset popups
    resetPopups();
    // reset messages
    resetMessages();
    // add person object
    _model->_player->setPosition(Vec2::ZERO);
    _model->_player->setVelocity(Vec2::ZERO);
    _model->_player->doneFlipping = false;
    _model->_player->shouldStartFlipping = false;
    _model->_pixelingIn = true;
    _model->_donePixelOut = false;
    _model->_donePixelIn = false;
    _model->_startOfLevel = true;
    _model->_player->setFriction(_initFriction);
    _model->_player->setInertia(_initInertia);
    prevPlay2DPos = Vec2::ZERO;
    _physics->getWorld()->addObstacle(_model->_player);
    // change plane for new model
    _plane->init(_model);
    _plane->calculateCut();
    _model->_player->lastRotateAngle = _model->getGlobalAngleDeg();
    _model->_player->setRotationalSprite(_model->getGlobalAngleDeg());
    // update physics for new cut
    createCutObstacles();
    _physics->update(0);
    // setup graphics pipeline
    _pipeline->sceneSetup(_model);
    
    _sound->streamSounds({ "lab_m", "lab_p" }, { 1.0, 0.0 }, true);
    
    //_sound->streamSounds({ "end" }, 1.0, true);
    _model->updateCompassNum();
    _model->_compassSpin->setVisible(false);
    resetCollectibleAlpha();
    
    /** set UI apha to 0.0 so it can fade in */
    auto color = _layer->getColor();
    auto newColor = Color4(color.r, color.g, color.b, 0.0);
    _layer->setColor(newColor);
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
    if(col == 0){
        // turn off the even and odd inventories
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory")->setVisible(false);
        _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd")->setVisible(false);
        return;
    }
    
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
            }
            if(got >= 2){
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
            }
            if(got >= 2){
                // collect center of odd inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_empty-two")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory-odd_key-two")->setVisible(true);
            }
            if(got >= 3){
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
            }
            if(got >= 2){
                // collect center left of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-two")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-two")->setVisible(true);
            }
            if(got >= 3){
                // collect center right of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-three")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-three")->setVisible(true);
            }
            if(got >= 4){
                // collect right of inventory
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_empty-four")->setVisible(false);
                _assets->get<scene2::SceneNode>("lab_gameUIScreen_inventory_key-four")->setVisible(true);
            }
            break;
    }
}


/**
 * Turns off and resets all popups
 */
void GameplayController::resetPopups() {
    _model->_popup->setState("none");
    // turn the popup off
    _model->_rotatePopup->setVisible(false);
    _model->_collectPopup->setVisible(false);
    _model->_glowPopup->setVisible(false);
    _model->_jumpPopup->setVisible(false);
    _model->_runPopup->setVisible(false);
    _model->_walkPopup->setVisible(false);
    
    // set popup alphas to 0.0 so it can fade in
    auto color = _model->_rotatePopup->getColor();
    auto newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_rotatePopup->setColor(newColor);
    
    color = _model->_collectPopup->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_collectPopup->setColor(newColor);
    
    color = _model->_glowPopup->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_glowPopup->setColor(newColor);
    
    color = _model->_jumpPopup->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_jumpPopup->setColor(newColor);
    
    color = _model->_runPopup->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_runPopup->setColor(newColor);
    
    color = _model->_walkPopup->getColor();
    newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_walkPopup->setColor(newColor);
}

/**
 * Turns off and resets the message popups
 */
void GameplayController::resetMessages() {
    _model->_messages->clear();
    // turn the message off
    _model->_messScene->setVisible(false);
    // set the alpha to 0.0 so it can fade in
    auto color = _model->_messScene->getColor();
    auto newColor = Color4(color.r, color.g, color.b, 0.0);
    _model->_messScene->setColor(newColor);
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
float lastFrameAngle;
void GameplayController::update(float dt) {
    // update time and global angle
    lastFrameAngle = _model->getGlobalAngleDeg();
    _model->_currentTime->mark();
    
#pragma mark SCENE TRANSITIONS
    
    // Fade in/out the UI
    if (_model->_pixelingIn){
        // all UI fade in
        auto color = _layer->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 5, 255));
        _layer->setColor(newColor);
    } else {
        // all UI fade out
        auto color = _layer->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _layer->setColor(newColor);
    }
    
    // done pixeling out
    if(_model->_donePixelOut){
        _model->_donePixelOut = false;
        _state = END;
        return;
    }
    
    // done backflip and starting to pixel out
    if(_model->_player->doneFlipping){
        _model->_player->doneFlipping = false;
        _model->_pixelingIn = false;
        _model->_pixelOutTime->mark();
        return;
    }
    
    // start to pixel in
    if(_model->_startOfLevel){
        _model->_startOfLevel = false;
        _model->_pixelingIn = true;
        _model->_pixelInTime->mark();
        return;
    }
    
    // not done pixeling in
    if(!_model->_donePixelIn){ return; }
    
    // not done pixeling out
    if(!_model->_pixelingIn){ return; }
    
#pragma mark -----
    
    if(_justCollected) {
        fadeinCollectibles();
    }else{
        fadeoutCollectibles();
    }
    
    _model->_justFinishRotating = false;
    
    _model->_player->animate();
    
    //CULog("loop on: %d", cugl::AudioEngine::get()->getMusicQueue()->isLoop());
    
#pragma mark INPUT
    // if the player shouldn't flip and the level isn't fading out
    if(!(_model->_player->shouldStartFlipping)){
        _input->update(dt);
    }
    
    if (_input->didDebug()) {
        setDebug(!isDebug());
        CULog("Debug mode is: %d, visibility: %d", isDebug(), _debugnode->isVisible());
        
    }

    // kill the player if marked dead
    if (_model->_player->isDead()) {
        _model->_player->setPosition(lastStablePlay2DPos);
        _model->_player->setDead(false);
        _model->_deathTime->mark();
    }
    
    // update popups
    updatePopups();
    
    // update messages
    updateMessages();

    if (!_input->isRotating) {
        saveFloat = 0.0;
        _input->cutFactor = 0.0;
    }
    
    if(_input->getHorizontal() != 0.0f){
        _model->_player->lastRotateAngle = _model->getGlobalAngleDeg();
        _model->_player->setRotationalSprite(_model->getGlobalAngleDeg());
    }
    
    if (_model->_player->isGrounded() && _input->isRotating) {
//        _plane->rotateNorm(_input->cutFactor/15000);
        //createCutObstacles();
        _plane->rotateNorm((_input->cutFactor - saveFloat)/1000 * _input->settings_invertRotate);
        _model->updateCompassNum();
        //only recalculate the rotational sprite if we changed our angle from the last frame
        if (_model->getGlobalAngleDeg() != lastFrameAngle) {
            _model->_player->setRotationalSprite(_model->getGlobalAngleDeg());
        }
        _model->_player->isRotating = true;
        saveFloat = _input->cutFactor;
        _rotating = true;
    }
    
    else if (_model->_player->isGrounded() && _input->didKeepChangingCut()) {
        _plane->rotateNorm(_input->getMoveNorm() * 1.75);
        _model->updateCompassNum();
        _model->_player->setRotationalSprite(_model->getGlobalAngleDeg());
        _model->_player->isRotating = true;
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
        else if(_model->_compassSpin->isVisible()) {
            auto color = _model->_compassSpin->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
            _model->_compassSpin->setColor(newColor);
            if(_model->_compassSpin->getColor().a <= 0) {
                _model->_compassSpin->setVisible(false);
            }
        }
        if (_model->_justFinishRotating) {
            _physics->getWorld()->addObstacle(_model->_player);
            _model->_player->setRotationalSprite(_model->getGlobalAngleDeg());
            _model->_player->isRotating = false;
            _plane->movePlaneToPlayer();
            _plane->calculateCut();//calculate cut here so it only happens when we finish rotating
            //_plane->debugCut(100);// enable this one to make a square of size 10 x 10 as the cut, useful for debugging
            createCutObstacles();
            //lastStablePlay2DPos = _model->_player->getPosition();
        }
        _physics->update(dt);
        // std::cout<<"curr velocity (x,y): " << _model->_player->getVelocity().x << "," << _model->_player->getVelocity().y << std::endl;
    }
    
#pragma mark COLLECTIBLES
    for (auto itr = _model->_collectibles.begin(); itr != _model->_collectibles.end(); itr++) {
        if (_model->getGlobalAngleDeg() != lastFrameAngle) {
            itr->second.setRotationalSprite(_model->getGlobalAngleDeg());
        }
        if (_model->getPlayer3DLoc().distance(itr->second.getPosition())<= COLLECTING_DIST && !itr->second.getCollected()) {
            _sound->playSound("collect", 0.75);
            itr->second.setCollected(true);
            _justCollected = true;
            _model->_backpack.insert(itr->first);
            if (_model->_nav_target == itr->second.getPosition()) {
                //need a new nav target, exit unless there are collectibles left
                Vec3 new_target = _model->_exit->getPosition();
                for (auto it_nav = _model->_collectibles.begin(); it_nav != _model->_collectibles.end(); it_nav++) {
                    if (_model->_backpack.count(it_nav->first) == 0) {
                        new_target = it_nav->second.getPosition();
                    }
                }
                _model->_nav_target = new_target;

            }
        }
    }
    
    _model->_exit->setRotationalSprite(_model->getGlobalAngleDeg());
    
    // update collectible UI
    collectUI(_model->getColNum(), _model->getCurrColNum());
    
    if(_model->getPlayer3DLoc().distance(_model->_exit->getPosition()) <= EXITING_DIST) {
        if (_model->checkBackpack()) {
            fadeinCollectibles();
            _model->_endOfGame = true;
            _model->_player->shouldStartFlipping = true;
            if(_model->_player->_isFlipping){
                _sound->playSound("portal", 0.75);
                _model->_player->_isFlipping = false;
            }
        }
    }
    
#pragma mark DECORATIONS
    if (_model->getGlobalAngleDeg() != lastFrameAngle) {
        for (auto itr = _model->_decorations.begin(); itr != _model->_decorations.end(); itr++) {
            (*itr)->setRotationalSprite(_model->getGlobalAngleDeg());
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
                
                _sound->playSound("glowstick_pickup", 0.75);
            }
            else{
                ++g;
            }
        }
        if (!_pickupGlowstick && _model->_glowsticks.size() < _model->_numGlowsticks) {
            auto g = Glowstick();
            if(_model->_player->isFacingRight()){
                g = Glowstick(player3DPos+(_plane->getBasisRight()*10)-(_model->getPlaneNorm()*1));
            }else{
                g = Glowstick(player3DPos-(_plane->getBasisRight()*10)-(_model->getPlaneNorm()*1));
            }
            _model->_glowsticks.push_back(g);
            _model->updateGlowstickCount();
            _model->_lightsFromItems[std::string(g.getPosition())] = GameModel::Light(g.getColor(), g.getIntense(), g.getPosition(), 2000.0); // hard coded for now
            
            _sound->playSound("glowstick_place", 0.75);
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
    
    //change portal sfx the closer you get to the portal
    Vec3 distance = _model->_exit->getPosition() - _model->getPlayer3DLoc();

    _portalDistance = distance.length();   //_model->getNavigatorTransforms().first.length();
    //CULog("portal distance: %f",_portalDistance);
    _sound->setTrackVolume(1, clampf(1-_portalDistance/MAX_PORTAL_DIST, 0.0, 1.0)); //slot 1 = cave_p
}

void GameplayController::updatePopups() {
    // turn all visible popups down in transparency
    if (_model->_rotatePopup->isVisible()){
        auto color = _model->_rotatePopup->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _model->_rotatePopup->setColor(newColor);
        // if it is completely faded out, turn it off
        if (_model->_rotatePopup->getColor().a <= 0) {
            _model->_rotatePopup->setVisible(false);
        }
    }
    if (_model->_collectPopup->isVisible()){
        auto color = _model->_collectPopup->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _model->_collectPopup->setColor(newColor);
        // if it is completely faded out, turn it off
        if (_model->_collectPopup->getColor().a <= 0) {
            _model->_collectPopup->setVisible(false);
        }
    }
    if (_model->_glowPopup->isVisible()){
        auto color = _model->_glowPopup->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _model->_glowPopup->setColor(newColor);
        // if it is completely faded out, turn it off
        if (_model->_glowPopup->getColor().a <= 0) {
            _model->_glowPopup->setVisible(false);
        }
    }
    if (_model->_jumpPopup->isVisible()){
        auto color = _model->_jumpPopup->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _model->_jumpPopup->setColor(newColor);
        // if it is completely faded out, turn it off
        if (_model->_jumpPopup->getColor().a <= 0) {
            _model->_jumpPopup->setVisible(false);
        }
    }
    if (_model->_runPopup->isVisible()){
        auto color = _model->_runPopup->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _model->_runPopup->setColor(newColor);
        // if it is completely faded out, turn it off
        if (_model->_runPopup->getColor().a <= 0) {
            _model->_runPopup->setVisible(false);
        }
    }
    if (_model->_walkPopup->isVisible()){
        auto color = _model->_walkPopup->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
        _model->_walkPopup->setColor(newColor);
        // if it is completely faded out, turn it off
        if (_model->_walkPopup->getColor().a <= 0) {
            _model->_walkPopup->setVisible(false);
        }
    }
    // turn the active popup (if there is one) up in transparency (x2)
    switch (_model->_popup->getState()) {
        case Popups::NONE:
            break;
        case Popups::ROTATE: {
            _model->_rotatePopup->setVisible(true);
            auto color = _model->_rotatePopup->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 20, 255));
            _model->_rotatePopup->setColor(newColor);
            break; }
        case Popups::COLLECT: {
            _model->_collectPopup->setVisible(true);
            auto color = _model->_collectPopup->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 20, 255));
            _model->_collectPopup->setColor(newColor);
            break; }
        case Popups::GLOW: {
            _model->_glowPopup->setVisible(true);
            auto color = _model->_glowPopup->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 20, 255));
            _model->_glowPopup->setColor(newColor);
            break; }
        case Popups::JUMP: {
            _model->_jumpPopup->setVisible(true);
            auto color = _model->_jumpPopup->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 20, 255));
            _model->_jumpPopup->setColor(newColor);
            break; }
        case Popups::RUN: {
            _model->_runPopup->setVisible(true);
            auto color = _model->_runPopup->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 20, 255));
            _model->_runPopup->setColor(newColor);
            break; }
        case Popups::WALK: {
            _model->_walkPopup->setVisible(true);
            auto color = _model->_walkPopup->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 20, 255));
            _model->_walkPopup->setColor(newColor);
            break; }
    }
    
}

void GameplayController::updateMessages() {
    switch (_model->_messages->getState()) {
        case Messages::NONE: {
            // fade out any active message
            auto color = _model->_messScene->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 10, 0));
            _model->_messScene->setColor(newColor);
            // if it is completely faded out, turn it off
            if (_model->_messScene->getColor().a <= 0) {
                _model->_messScene->setVisible(false);
            }
            break; }
        case Messages::EXIT: {
            // if the backpack is not full show exit message
            if(!_model->checkBackpack()) {
                // set message to text
                _model->_messText->setText(_model->_messages->getText());
                // turn on message
                _model->_messScene->setVisible(true);
                // fade in active message
                auto color = _model->_messScene->getColor();
                auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
                _model->_messScene->setColor(newColor);
            }
            break; }
        case Messages::MESS: {
            // set message to text
            _model->_messText->setText(_model->_messages->getText());
            // turn on message
            _model->_messScene->setVisible(true);
            // fade in active message
            auto color = _model->_messScene->getColor();
            auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
            _model->_messScene->setColor(newColor);
            break; }
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
    // turn off the render pipeline stuff
    glDisable(GL_DEPTH_TEST);
    Scene2::render(batch);
    // turn on the render pipeline stuff
    glEnable(GL_DEPTH_TEST);
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
        _model->_player->startTrackingAirTime = false;
        // Could have more than one ground
        _sensorFixtures.emplace(_model->_player.get() == bd1 ? fix2 : fix1);
    }
    
    if ((_model->_player->getLandSensorName() == fd2 && _model->_player.get() != bd1) ||
        (_model->_player->getLandSensorName() == fd1 && _model->_player.get() != bd2)) {
        if(_model->_player->getVY() < 0.0f){
            _model->_player->setLanding(true);
            //CULog("Landing");
        }
        // Could have more than one ground
        //_sensorFixtures.emplace(_model->_player.get() == bd1 ? fix2 : fix1);
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

void GameplayController::fadeoutCollectibles(){
    auto color = _model->_invent->getColor();
    auto colorodd = _model->_inventodd->getColor();
    auto newColor = Color4(color.r, color.g, color.b, std::max(color.a - 1, 0));
    auto newColorodd = Color4(color.r, color.g, color.b, std::max(colorodd.a - 1, 0));
    _model->_invent->setColor(newColor);
    _model->_inventodd->setColor(newColorodd);
//    if(_model->_invent->getColor().a <= 0 || _model->_inventodd->getColor().a <= 0) {
//        _justCollected = false;
//    }
}

void GameplayController::fadeinCollectibles(){
    auto color = _model->_invent->getColor();
    auto colorodd = _model->_inventodd->getColor();
    auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, _model->initInventAlpha));
    auto newColorodd = Color4(color.r, color.g, color.b, std::min(colorodd.a + 10, _model->initOddAlpha));
    _model->_invent->setColor(newColor);
    _model->_inventodd->setColor(newColorodd);
    if(_model->_invent->getColor().a == _model->initInventAlpha
       || _model->_inventodd->getColor().a == _model->initOddAlpha) {
        _justCollected = false;
    }
}

void GameplayController::resetCollectibleAlpha() {
    auto color = _model->_invent->getColor();
    auto colorodd = _model->_inventodd->getColor();
    _model->_invent->setColor(Color4(color.r, color.g, color.b, _model->initInventAlpha));
    _model->_inventodd->setColor(Color4(colorodd.r, colorodd.g, colorodd.b, _model->initOddAlpha));
    _justCollected = false;
}

