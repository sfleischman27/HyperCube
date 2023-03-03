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
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

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


/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameplayController::GameplayController() : Scene2()
{
	auto level = Level::loadLevel("temp");
	_model = std::make_unique<GameModel>(level);
    
    _worldnode = nullptr;
    _debugnode = nullptr;
    _debug = false;
    //_physics = std::shared_ptr<PhysicsController>();
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameplayController::dispose() {
    if(_active){
        _input.dispose();
        _physics.dispose();
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
bool GameplayController::init(const std::shared_ptr<AssetManager>& assets) {
    return init(assets,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT));
}

bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect) {
//    _input.init(Rect());
    
    _dimen = computeActiveSize();
    
    Vec2 offset((_dimen.width-SCENE_WIDTH)/2.0f,(_dimen.height-SCENE_HEIGHT)/2.0f);

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(_dimen)) {
        return false;
    }
    
    // Start up the input handler
//    _assets = assets;
    _input.init(getBounds());
    
    //set up physics world
    _physics.init(_dimen, rect, SCENE_WIDTH);

    _collectibles = _model->getCollectibles();
//    TODO: How to draw collectibles
//    for (Collectible c : _collectibles) {
//        c.setTexture(assets->get<Texture>("bullet"));
//    }
    
    /** Player */
    
    // The initial position of the player
    float DUDE_POS[] = { SCENE_WIDTH/(2 * _physics.getScale()), SCENE_HEIGHT/(2 * _physics.getScale()) - 4};
    
    Vec2 dudePos = DUDE_POS;
    
        //std::shared_ptr<Texture> image = assets->get<Texture>(DUDE_TEXTURE);
    
        //TODO Gordi fill in your scale right below
        //_player = PlayerModel::alloc(dudePos,image->getSize()/_scale,_scale);
    
        //std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
        //_player->setSceneNode(sprite);
    
        //TODO Gordi add the player as an obstacle. The original code does something like: addObstacle(_player,sprite);

        // TODO Gordi fill in your scene node right below
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_physics.getScale()); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
#pragma mark ADD WALL COLLIDERS FROM CUT
    
    //MUST be done before anything else is added to _worldnode
    createCutObstacles();
    
    //debug obstacle
//    std::shared_ptr<cugl::physics2::PolygonObstacle> ob = physics2::PolygonObstacle::alloc(Poly2(Rect(0,0,1,2)));
//    ob->setBodyType(b2_staticBody);
//    ob->setPosition(Vec2(SCENE_WIDTH/(2 * _physics.getScale()), SCENE_HEIGHT/(2 * _physics.getScale())-5));
//    ob->setSize(ob->getSize());
//    std::shared_ptr<cugl::scene2::SceneNode> n = scene2::SceneNode::alloc();
//
//    addObstacle(ob, n, true);

    
#pragma mark ADD DUDE
    std::shared_ptr<Texture> image = assets->get<Texture>(DUDE_TEXTURE);

    //_player = PlayerModel::alloc(dudePos,image->getSize()/_physics.getScale(),_physics.getScale());
    _player = PlayerModel::alloc(dudePos,image->getSize()/_physics.getScale(),_physics.getScale());
    

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    _player->setSceneNode(sprite);
    _player->setDebugColor(DEBUG_COLOR);
    
    //TODO Gordi add the player as an obstacle. The original code does something like:
    addObstacle(_player, sprite, true);
    
    addChild(_worldnode);
    addChild(_debugnode);

    _active = true;
    
    return true;
    
}

void GameplayController::createCutObstacles(){
    //remove previous poly nodes
    removePolyNodes();
    //_worldnode->removeAllChildren();
    //_physics.removeObstacles();
    
    std::shared_ptr<Rect> bounds = std::make_shared<Rect>(Vec2::ZERO, getSize() / _physics.getScale());
    
    //get the cut from the gamemodel
    std::vector<cugl::Poly2> polys = _model->getCut();
    
    std::shared_ptr<scene2::SceneNode> cutnode = scene2::SceneNode::alloc();
    
    for(Poly2 p : polys){
        
        //Vec2() * Vec2();
        std::vector<cugl::Vec2> vertices = p.getVertices();
                
        for(cugl::Vec2 v : vertices){
            v.x *= 1;
            v.y *= 1;
        }
        
        //p.set(vertices);
        
        float transformScale = _physics.getScale()/2;
        
        //for some reason multiply by aspect ratio?
        Poly2 bigp = p * Affine2(transformScale,0,0,transformScale * _dimen.height/_dimen.width,0,0);
        
        std::shared_ptr<cugl::physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(bigp);
        
        /*bool initialized = obstacle->init(p);
        CUAssertLog(initialized, "levelbounds polygonobstacle not initialized properly");*/

        obstacle->setBodyType(b2_staticBody);
        obstacle->setPosition(Vec2(SCENE_WIDTH/(2 * _physics.getScale()), SCENE_HEIGHT/(2 * _physics.getScale())));
        
        obstacle->setSize(obstacle->getSize());
        
        cutnode = scene2::SceneNode::alloc();
        
        _cutnodes.insert(cutnode);
        _cutobstacles.insert(obstacle);

        //_polynodes.insert(_polynodes.begin(),polynode);
        
        addObstacle(obstacle, cutnode, true);
    }
    
}

void GameplayController::removePolyNodes(){
    //_debugnode->removeAllChildren();
    
    for(std::shared_ptr<cugl::scene2::SceneNode> node : _worldnode->getChildren()){
        
        bool is_in = _cutnodes.find(node) != _cutnodes.end();

        if(is_in){
            _cutnodes.erase(node);
            _worldnode->removeChild(node);
        }
    }
    for(std::shared_ptr<cugl::physics2::Obstacle> obstacle : _physics.getWorld()->getObstacles()){
        
        bool is_in = _cutobstacles.find(obstacle) != _cutobstacles.end();

        if(is_in){
            _cutobstacles.erase(obstacle);
            _physics.markForRemoval(obstacle);
        } else {
            //CULog("Could not find cutobstacle %s", obstacle->toString().c_str());
            //CULog("Could not find cutobstacle %p", obstacle.get());
        }
    }
    _physics.garbageCollect();
}

void GameplayController::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            bool useObjPosition) {
    _physics.getWorld()->addObstacle(obj);
    obj->setDebugColor(DEBUG_COLOR);
    obj->setDebugScene(_debugnode);
        
    // Position the scene graph node (enough for static objects)
      if (useObjPosition) {
          node->setPosition(obj->getPosition()/_physics.getScale());
      }
    
      _worldnode->addChild(node);
    
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_physics.getScale());
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
    _physics.clear();
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

    _input.update(dt);
    
    if (_input.didDebug()) {
        setDebug(!isDebug());
        CULog("Debug mode is: %d, visibility: %d", isDebug(), _debugnode->isVisible());
        
    }

    if (_input.didIncreaseCut()) {
        _model->rotateNorm(.03);
        //createCutObstacles();
        _rotating = true;
    }

    else if (_input.didDecreaseCut()) {
        _model->rotateNorm(-.03);
        //createCutObstacles();
        _rotating = true;
    }
    else if (_input.didKeepChangingCut()) {
        _model->rotateNorm(_input.getMoveNorm());
        //createCutObstacles();
        _rotating = true;
    }
    else {
        if(_rotating){
            createCutObstacles();
            _rotating = false;
        }
        _physics.update(dt);
    }

    // TODO: Update player bag what is collected
    // TODO: check against player location to see if player can collect
    if (_input.didSelect()) {
        auto pos = _input.getSelection();
        pos = Vec2(screenToWorldCoords(pos)).subtract(getSize() / 2);
        //down scale it by screen size for comparison
        pos = Vec2(pos.x / (_dimen.width / 2), pos.y / (_dimen.height / 2));
        for (int i = 0; i < _collectibles.size(); i++) {
            auto tuplec = ScreenCoordinatesFrom3DPoint(_collectibles[i].getPosition());
            auto coords = std::get<0>(tuplec);
            auto dist = std::get<1>(tuplec);
            if (!_collectibles[i].getCollected() &&
                std::abs(dist) <= VISIBLE_DIST &&
                std::abs(pos.x - coords.x) <= CLICK_DIST &&
                std::abs(pos.y - coords.y) <= CLICK_DIST) {
                _collectibles[i].setCollected(true);
            }
        }
    }

        _player->setMovement(_input.getHorizontal()*_player->getForce());
        _player->setJumping( _input.didJump());
        _player->applyForce();

        /*animated by incrementing angle each frame*/
    //	_model->rotateNorm(.01);

        /*give a normal directly (only the x,y coords matter)*/
        //_model->setPlaneNorm(Vec3(1, 3, 0).normalize());

        /*pick a specific angle to cut at(relative to UNIT_X)*/
    //	float radians = M_PI / 3;
    //	Vec3 newNorm = Vec3(
    //		cos(radians),
    //		sin(radians),
    //		0
    //	);
    //	_model->setPlaneNorm(newNorm);
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

    Scene2::render(batch);
    
	//Begin Drawing
	batch->begin();
	batch->setColor(Color4::BLACK);
	auto cuts = _model->getCut();
	for (auto it = cuts.begin(); it != cuts.end(); it++) {
        batch->fill(*it);
	}
    
    //_debugnode->draw(batch, Affine2(), DEBUG_COLOR);
    //_debugnode->render(batch);

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

    // draw collectibles if the collectible is within certain distance to the plane
    // and if the collectibe has not been not collected yet
    for (int i = 0; i < _collectibles.size(); i++) {
        if (!_collectibles[i].getCollected()) {
            auto tupleKey = ScreenCoordinatesFrom3DPoint(_collectibles[i].getPosition());
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
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width/dimen.height;
    float ratio2 = ((float)SCENE_WIDTH)/((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH/dimen.width;
    } else {
        dimen *= SCENE_HEIGHT/dimen.height;
    }
    return dimen;
}

/**Get the 2d coordinates relative to the cut plane of a 3d location
* it also returns the projected distance from that point to the cut plane, which can be used to threshold drawing of an object at that location
* RETURN: screen coordinates and projection distance pairs are returned as a std::tuple<Vec2,float>*/
std::tuple<cugl::Vec2, float> GameplayController::ScreenCoordinatesFrom3DPoint(cugl::Vec3 location) {

    //dot the point onto the plane normal to get the distance from the cut plane
    auto dist = location.dot(_model->getPlaneNorm());
    // get the point projected onto the plane basis vectors (unit_z is always y-axis and x-axis is to the right)
    auto xcoord = location.dot(cugl::Vec3(0,0,1).cross(_model->getPlaneNorm().negate()).normalize());
    auto ycoord = location.dot(cugl::Vec3(0,0,1));
    auto coords = cugl::Vec2(xcoord, ycoord);

    return(std::tuple<cugl::Vec2, float>(coords, dist));
}
