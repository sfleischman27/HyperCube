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

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192
/** Threshold of the visible distance */
#define VISIBLE_DIST   0.15

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
    return init(assets,Rect(0,0,SCENE_WIDTH,SCENE_HEIGHT));
}


bool GameplayController::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect) {
//    _input.init(Rect());
    
    Size dimen = computeActiveSize();

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
//    _assets = assets;
    _input.init(getBounds());
    
    //set up physics world
    _physics.init(dimen, rect, SCENE_WIDTH);
    _physics.createPhysics(*_model,getSize());

    _collectibles = _model->getCollectibles();
//    TODO: How to draw collectibles
//    for (Collectible c : _collectibles) {
//        c.setTexture(assets->get<Texture>("bullet"));
//    }
    
    /** Player */
    
    // The initial position of the player
    float DUDE_POS[] = { 2.5f, 5.0f};
    
    Vec2 dudePos = DUDE_POS;
    
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);

        // TODO Gordi fill in your scene node right below
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    std::shared_ptr<Texture> image = assets->get<Texture>(DUDE_TEXTURE);

    _player = PlayerModel::alloc(dudePos,image->getSize()/_physics.getScale(),_physics.getScale());

    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    _player->setSceneNode(sprite);

    //TODO Gordi add the player as an obstacle. The original code does something like:
    addObstacle(_player, sprite, true);
    
    /* FOR THE DEBUG WIREFRAME STUFF.
     TODO: IMPLEMENT
     
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(dimen); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    //_debugnode->setPosition(offset);
     */

    addChild(_worldnode);
    //addChild(_debugnode);

    _active = true;
    
    return true;
    
}

void GameplayController::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            bool useObjPosition) {
    _physics.getWorld()->addObstacle(obj);
    //obj->setDebugScene(_debugnode);
    
    // Position the scene graph node (enough for static objects)
      if (useObjPosition) {
          node->setPosition(obj->getPosition()*_physics.getScale());
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

    if(_input.didIncreaseCut()){
        _model->rotateNorm(.03);
        _physics.createPhysics(*_model,getSize());
    }
    
    else if(_input.didDecreaseCut()){
        _model->rotateNorm(-.03);
        _physics.createPhysics(*_model,getSize());
    }
    else if(_input.didKeepChangingCut()) {
        _model->rotateNorm(_input.getMoveNorm());
        _physics.createPhysics(*_model,getSize());
    }
    else {
        _physics.update(dt);
    }
    
//    _player->setMovement(_input.getHorizontal()*_player->getForce());
//    _player->setJumping( _input.didJump());
//    _player->applyForce();
	
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
    
//    TODO: How to update collectibles: if still collectible
//    TODO: Update player bag what is collected

	
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

	//Begin Drawing
	batch->begin();
	batch->setColor(Color4::BLACK);
	auto cuts = _model->getCut();
	for (auto it = cuts.begin(); it != cuts.end(); it++) {
		batch->fill(*it);
	}
    
    std::vector<std::shared_ptr<scene2::SceneNode>> world = _worldnode->getChildren();
    for (std::shared_ptr<scene2::SceneNode> it : world) {
        /*it->setContentSize(
                           it->getWidth() * _physics.getScale(),
                           it->getHeight() *  _physics.getScale());*/
        it->render(batch);
    }
    
    
    /* TODO: Color in the obstacles for debug
    batch->setColor(Color4::GREEN);
    auto obstacles = _physics.getPolygonObstacles();
    for (auto it = obstacles.begin(); it != obstacles.end(); it++) {
        batch->fill(*it);
    }*/

    // draw exit
    // TODO: Jack maybe you want to double check the exit location cuz it's not inside cuts right now (lmk if im wrong)
    auto tupleExit = ScreenCoordinatesFrom3DPoint(_model->getLevel()->exitLoc);
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
    for (Collectible c : _collectibles) {
        if (!c.getCollected()) {
            auto tupleKey = ScreenCoordinatesFrom3DPoint(c.getPosition());
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
    auto xcoord = location.dot(cugl::Vec3(0,0,1).cross(_model->getPlaneNorm()));
    auto ycoord = location.dot(cugl::Vec3::UNIT_Z);
    auto coords = cugl::Vec2(xcoord, ycoord);

    return(std::tuple<cugl::Vec2, float>(coords, dist));
}
