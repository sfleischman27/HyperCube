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

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameplayController::GameplayController() : Scene2() {
	auto level = Level::loadLevel("temp");
	_model = std::make_unique<GameModel>(level);
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameplayController::dispose() {}

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
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameplayController::init(const std::shared_ptr<AssetManager>& assets) {
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
 
    _collectibles = _model->getCollectibles();
//    TODO: How to draw collectibles
//    for (Collectible c : _collectibles) {
//        c.setTexture(assets->get<Texture>("bullet"));
//    }

    return true;}

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameplayController::reset() {}

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
        CULog("Increased cut");
        _model->rotateNorm(.03);
    }
    
    if(_input.didDecreaseCut()){
        CULog("Decreased cut");
        _model->rotateNorm(-.03);
    }
	
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
	
    //    TODO: How to draw collectibles
    //    for (Collectible c : _collectibles) {
    //        if (c.canBeSeen(_model->getPlaneNorm()) && !c.getCollected()) {
    //            c.draw(batch);
    //        }
    //    }
    
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
