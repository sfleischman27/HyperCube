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
/** Threshold of the visible distance */
#define VISIBLE_DIST   .02
/** Threshold of the click distance */
#define CLICK_DIST   0.05
/** Threshold of the collecting distance */
#define COLLECTING_DIST   0.1

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameplayController::GameplayController() : Scene2() {
	auto level = Level::loadLevel("temp");
	_model = std::make_unique<GameModel>(level);
    
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
bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Size& displaySize) {
    _pipeline = std::make_shared<RenderPipeline>(SCENE_WIDTH, displaySize);
	
    return init(assets,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT));
}

bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect) {
    
    _dimen = computeActiveSize();
    
    Vec2 offset((_dimen.width-SCENE_WIDTH)/2.0f,(_dimen.height-SCENE_HEIGHT)/2.0f);

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(_dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    _input = std::make_shared<InputController>();
    _input->init(getBounds());
    
#pragma mark SCENE GRAPH SETUP
    
    //set up physics world
    _physics = std::make_shared<PhysicsController>();
    _physics->init(_dimen, rect, SCENE_WIDTH);
  
    
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_physics->getScale()); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
#pragma mark ADD WALL COLLIDERS FROM CUT
    
    //MUST be done before anything else is added to _worldnode
    createCutObstacles();

#pragma mark ADD PLAYER
//    float DUDE_POS[] = { SCENE_WIDTH/(2 * _physics->getScale()), SCENE_HEIGHT/(2 * _physics->getScale()) - 4};
    float DUDE_POS[] = { _dimen.width/(2 * _physics->getScale()), _dimen.height/(2 * _physics->getScale()) - 4};
    
    Vec2 dudePos = DUDE_POS;
    
    std::shared_ptr<Texture> image = assets->get<Texture>(DUDE_TEXTURE);

    _model->setPlayer(PlayerModel::alloc(dudePos,image->getSize()/_physics->getScale(),_physics->getScale()));
    

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    _model->_player->setSceneNode(sprite);
    _model->_player->setDebugColor(DEBUG_COLOR);
    
    addObstacle(_model->_player, sprite, true);
    
    addChild(_worldnode);
    addChild(_debugnode);

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
    removePolyNodes();
    createObstacleFromPolys(_model->getCut());
}

/**
 * Generates obstacle instances from a list of Poly2s. Each poly2 represents a new obstacle.
 * @param polys the list of Poly2s
 */
void GameplayController::createObstacleFromPolys(std::vector<cugl::Poly2> polys){
    //std::shared_ptr<Rect> bounds = std::make_shared<Rect>(Vec2::ZERO, getSize() / _physics->getScale());
    
    std::shared_ptr<scene2::SceneNode> cutnode = scene2::SceneNode::alloc();
    
    for(Poly2 p : polys){
        std::vector<cugl::Vec2> vertices = p.getVertices();
                
        for(cugl::Vec2 v : vertices){
            v.x *= 1;
            v.y *= 1;
        }
        
        float transformScale = _physics->getScale()/2;
        
        //for some reason multiply by aspect ratio to make the obstacles in-line with the drawn cut?
        
        //use this if you are transforming the original drawn poly2:
        //Poly2 bigp = p * Affine2(transformScale,0,0,transformScale,0,0);
        
        //use this if you are keeping the original drawn poly2 (vertically squished)
        Poly2 bigp = p * Affine2(transformScale,0,0,transformScale * _dimen.height/_dimen.width,0,0);
        
        std::shared_ptr<cugl::physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(bigp);

        obstacle->setBodyType(b2_staticBody);
        obstacle->setPosition(Vec2(SCENE_WIDTH/(2 * _physics->getScale()), SCENE_HEIGHT/(2 * _physics->getScale())));
        
        obstacle->setSize(obstacle->getSize());
        
        cutnode = scene2::SceneNode::alloc();
        
        _cutnodes.insert(cutnode);
        _cutobstacles.insert(obstacle);
        
        addObstacle(obstacle, cutnode, true);
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
    for(std::shared_ptr<cugl::physics2::Obstacle> obstacle : _physics->getWorld()->getObstacles()){
        
        bool is_in = _cutobstacles.find(obstacle) != _cutobstacles.end();

        //if obstacle is an obstacle generated from the cut (and not something like the player's body)
        if(is_in){
            _cutobstacles.erase(obstacle);
            _physics->markForRemoval(obstacle);
        }
    }
    _physics->garbageCollect(); //<- thought i needed this but it's working w it commented out? so im keeping it like this?
}


/**
 * Adds obstacle to both physics world and world node/debug node via an intermediary SceneNode
 * @param obj the obstacle
 * @param node the SceneNode you're adding the obj to (which will then be added to the world+debug scenenodes
 * @param useObjPosition whether you should use the object's local position when adding to scenes. Default is true.
 */
void GameplayController::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            bool useObjPosition) {
    _physics->getWorld()->addObstacle(obj);
    obj->setDebugColor(DEBUG_COLOR);
    obj->setDebugScene(_debugnode);
        
    // Position the scene graph node (enough for static objects)
      if (useObjPosition) {
          node->setPosition(obj->getPosition()/_physics->getScale());
      }
    
      _worldnode->addChild(node);
    
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_physics->getScale());
            weak->setAngle(obs->getAngle());
        });
    }
}


/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameplayController::reset() {
    _physics->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
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
void GameplayController::update(float dt) {

#pragma mark INPUT
    _input->update(dt);
    
    if (_input->didDebug()) {
        setDebug(!isDebug());
        CULog("Debug mode is: %d, visibility: %d", isDebug(), _debugnode->isVisible());
        
    }

    //if (_input->didIncreaseCut() && (_model->_player->getX() > DEFAULT_WIDTH/2 - 1) && (_model->_player->getX() < DEFAULT_WIDTH/2 + 1)){
    if (_input->didIncreaseCut()) {
        _model->rotateNorm(.01);
        //createCutObstacles();
        _rotating = true;
    }

    //else if (_input->didDecreaseCut() && (_model->_player->getX() > DEFAULT_WIDTH/2 - 1) && (_model->_player->getX() < DEFAULT_WIDTH/2 + 1)) {
    else if (_input->didDecreaseCut()) {
        _model->rotateNorm(-.01);
        //createCutObstacles();
        _rotating = true;
    }
    //else if (_input->didKeepChangingCut() && (_model->_player->getX() > DEFAULT_WIDTH/2 - 1) && (_model->_player->getX() < DEFAULT_WIDTH/2 + 1)) {
    else if (_input->didKeepChangingCut()) {
        _model->rotateNorm(_input->getMoveNorm());
        //createCutObstacles();
        _rotating = true;
    }
    else {
        if(_rotating){
            createCutObstacles();
            _rotating = false;
        }
        _physics->update(dt);
    }

#pragma mark COLLECTIBLES
    
    // TODO: Update player bag what is collected -Jolene
    if (_input->didSelect()) {
        auto pos = _input->getSelection();
        pos = Vec2(screenToWorldCoords(pos)).subtract(getSize() / 2);
        //down scale the click position by screen size for comparison
        pos = Vec2(pos.x / (_dimen.width / 2), pos.y / (_dimen.height / 2));
        for (auto itr = _model->_collectibles.begin(); itr != _model->_collectibles.end(); itr++) {
            auto tuplec = ScreenCoordinatesFrom3DPoint(itr->second.getPosition());
            auto coords = std::get<0>(tuplec);
            auto dist = std::get<1>(tuplec);
            //down scale the player position by screen size for comparison
            float w = (_dimen.width/_physics->getScale())/2;
            float h = (_dimen.height/_physics->getScale())/2;
            Vec2 playerpos = Vec2((_model->_player->getX()-w)/w,(_model->_player->getY()-h)/h);
            if (!itr->second.getCollected() &&
                std::abs(dist) <= VISIBLE_DIST &&
                std::abs(pos.x - coords.x) <= CLICK_DIST &&
                std::abs(pos.y - coords.y) <= CLICK_DIST &&
                std::abs(playerpos.x - coords.x) <= COLLECTING_DIST &&
                std::abs(playerpos.y - coords.y) <= COLLECTING_DIST) {
                itr->second.setCollected(true);
                _model->_backpack.insert(itr->first);
            }
        }
    }
    
#pragma mark PLAYER
    
    _model->_player->setMovement(_input->getHorizontal()*_model->_player->getForce());
    _model->_player->setJumping( _input->didJump());
    _model->_player->applyForce();

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

    bool useRP = true;
    if (useRP) {
        _pipeline->render(_model);
        return;
    }
    Scene2::render(batch);
    
#pragma mark DRAW CUT
	//Begin Drawing
	batch->begin();
	batch->setColor(Color4::BLACK);
    auto cuts = _model->getCut();
	for (auto it = cuts.begin(); it != cuts.end(); it++) {
        
        //TODO: temporary fix to stretch the level back to how it's "supposed" to be? take this out if its wrong
        //*it *= Affine2(1, 0, 0, _dimen.width/_dimen.height, 0, 0);
        //it.draw
        batch->fill(*it);
	}
    
    //_debugnode->draw(batch, Affine2(), DEBUG_COLOR);
    //_debugnode->render(batch);
#pragma mark DRAW EXIT
    
    // draw exit
    tupleExit = ScreenCoordinatesFrom3DPoint(_model->getLevel()->exitLoc);
    auto screencoordsExit = std::get<0>(tupleExit);
    auto distanceExit = std::get<1>(tupleExit);
    if (std::abs(distanceExit) <= VISIBLE_DIST) {
        std::shared_ptr<Texture> blue = std::make_shared<Texture>();
        static unsigned char bluecol[] = { 0, 0, 0xFF, 0xFF };
        blue->initWithData(bluecol, 1, 1);
        Vec2 bottomleftExit = screencoordsExit - Vec2(0.02, 0.02);
        batch->draw(blue, Rect(bottomleftExit, Vec2(0.025, 0.04)));
    }

#pragma mark DRAW COLLECTIBLES
    // draw collectibles if the collectible is within certain distance to the plane
    // and if the collectibe has not been not collected yet
//    std::unordered_map<std::string, Collectible>::iterator itr;
    for (auto itr = _model->_collectibles.begin(); itr != _model->_collectibles.end(); itr++) {
        if (!itr->second.getCollected()) {
            auto tupleKey = ScreenCoordinatesFrom3DPoint(itr->second.getPosition());
            auto screencoordsKey = std::get<0>(tupleKey);
            auto distanceKey = std::get<1>(tupleKey);
            if (std::abs(distanceKey) <= VISIBLE_DIST) {
                std::shared_ptr<Texture> yellow = std::make_shared<Texture>();
                static unsigned char yellowcol[] = { 0xFF, 0xFF, 0x00, 0xFF };
                yellow->initWithData(yellowcol, 1, 1);
                Vec2 bottomleft = screencoordsKey - Vec2(0.02, 0.02);
                batch->draw(yellow, Rect(bottomleft, Vec2(0.025, 0.04)));
            }
        }
    }
        
	// End Drawing
	batch->end();
}

Size GameplayController::computeActiveSize() const {
    // TODO: add function comment -Jolene
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
