//
//  PlayerModel.cpp
//  Platformer
//
//  Created by JD on 2/23/23.
//

#include "PlayerModel.h"

#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#define SIGNUM(x)  ((x > 0) - (x < 0))

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for shooting */
#define SHOOT_COOLDOWN  20
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.35f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.7f
/** The amount to shrink the sensor fixture (horizontally) relative to the image */
#define DUDE_SSHRINK  0.6f
/** Height of the sensor attached to the player's feet */
#define SENSOR_HEIGHT   0.1f
/** The density of the character */
#define DUDE_DENSITY    0.01f
/** The impulse for the character jump */
#define DUDE_JUMP       500.0f
/** Debug color for the sensor */
#define DEBUG_COLOR     Color4::RED


using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool PlayerModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    Size nsize = size / 4.0f;
//    nsize.width  *= DUDE_HSHRINK;
//    nsize.height *= DUDE_VSHRINK;
    _drawScale = scale;
    
    if (CapsuleObstacle::init(pos,nsize)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);      // HE WILL STICK TO WALLS IF YOU FORGET
        setFixedRotation(true); // OTHERWISE, HE IS A WEEBLE WOBBLE
        
        // Gameplay attributes
        _isGrounded = false;
        _isShooting = false;
        _isJumping  = false;
        _faceRight  = true;
        
        _shootCooldown = 0;
        _jumpCooldown  = 0;
        
        // Sound Cues
        _jumpCue = false;
        
        return true;
    }
    return false;
}


#pragma mark -
#pragma mark Attribute Properties

/**
 * Sets left/right movement of this character.
 *
 * This is the result of input times dude force.
 *
 * @param value left/right movement of this character.
 */
void PlayerModel::setMovement(float value) {
    _movement = value;
    bool face = _movement > 0;
    if (_movement == 0 || _faceRight == face) {
        return;
    }
    
    // Change facing
    scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
    if (image != nullptr) {
        image->flipHorizontal(!image->isFlipHorizontal());
    }
    _faceRight = face;
}


#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void PlayerModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    CapsuleObstacle::createFixtures();
    b2FixtureDef sensorDef;
    sensorDef.density = DUDE_DENSITY;
    sensorDef.isSensor = true;
    
    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -DUDE_SSHRINK*getWidth()/2.0f;
    corners[0].y = (-getHeight()+SENSOR_HEIGHT)/2.0f;
    corners[1].x = -DUDE_SSHRINK*getWidth()/2.0f;
    corners[1].y = (-getHeight()-SENSOR_HEIGHT)/2.0f;
    corners[2].x =  DUDE_SSHRINK*getWidth()/2.0f;
    corners[2].y = (-getHeight()-SENSOR_HEIGHT)/2.0f;
    corners[3].x =  DUDE_SSHRINK*getWidth()/2.0f;
    corners[3].y = (-getHeight()+SENSOR_HEIGHT)/2.0f;
    
    b2PolygonShape sensorShape;
    sensorShape.Set(corners,4);
    
    sensorDef.shape = &sensorShape;
    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());
    _sensorFixture = _body->CreateFixture(&sensorDef);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void PlayerModel::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }
    
    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }
}

/**
 * Disposes all resources and assets of this PlayerModel
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a PlayerModel may not be used until it is initialized again.
 */
void PlayerModel::dispose() {
    _core = nullptr;
    _node = nullptr;
    _sensorNode = nullptr;
}

/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void PlayerModel::applyForce() {
    if (!isEnabled()) {
        return;
    }
    
    // Don't want to be moving. Damp out player motion
    if (getMovement() == 0.0f) {
        if (isGrounded()) {
            // Instant friction on the ground
            b2Vec2 vel = _body->GetLinearVelocity();
            vel.x = 0; // If you set y, you will stop a jump in place
            _body->SetLinearVelocity(vel);
        } else {
            // Damping factor in the air
            b2Vec2 force(-getDamping()*getVX(),0);
            _body->ApplyForce(force,_body->GetPosition(),true);
        }
    }
    
    // Velocity too high, clamp it
    if (fabs(getVX()) >= getMaxSpeed()) {
        setVX(SIGNUM(getVX())*getMaxSpeed());
    } else {
        b2Vec2 force(getMovement(),0);
        _body->ApplyForce(force,_body->GetPosition(),true);
    }

    if (isJumping() && isGrounded()) {
        b2Vec2 force(0, DUDE_JUMP);
        _body->ApplyLinearImpulse(force,_body->GetPosition(),true);
        _jumpCue = true;
    }
//    if (isJumping()) {
//        b2Vec2 force(0, DUDE_JUMP);
//        _body->ApplyLinearImpulse(force,_body->GetPosition(),true);
//    }
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void PlayerModel::update(float dt) {
    // Apply cooldowns
    if (isJumping()) {
        _jumpCooldown = JUMP_COOLDOWN;
    } else {
        // Only cooldown while grounded
        _jumpCooldown = (_jumpCooldown > 0 ? _jumpCooldown-1 : 0);
    }
    
    if (isShooting()) {
        _shootCooldown = SHOOT_COOLDOWN;
    } else {
        _shootCooldown = (_shootCooldown > 0 ? _shootCooldown-1 : 0);
    }
    
    /** Animation AND SOUND logic!!! IM HIJACKING AGAIN :) - Gordi*/
    if(getVY() > 0.1 && !isGrounded()){
        if(animState != 2){
            animState = 2;
            setSpriteSheet("jump");
        } else{
            animState = 2;
        }
    }
    else if(abs(getVX()) > 2){
        if(animState != 1){
            animState = 1;
            setSpriteSheet("walk");
        } else{
            animState = 1;
            _walkCue = false;
        }
    }
    else {
        setSpriteSheet("idle");
        animState = 0;
    }
    
//    if(!_isGrounded){
//        setSpriteSheet("jump");
//    }
    
    
    if(animFrameCounter >= 2){
        animFrameCounter = 0;
        int frame = currentSpriteSheet->getFrame();
        if(isFacingRight()){
            frame++;
        }else{
            frame--;
        }
        if (frame >= currentSpriteSheet->getSize()) {
            frame = 0;
        }
        if(frame < 0){
            frame = currentSpriteSheet->getSize() - 1;
        }
        //CULog("%i",frame);
        currentSpriteSheet->setFrame(frame);
        currentNormalSpriteSheet->setFrame(frame);
        
        //sound cue logic
        switch(animState){
            //WALKING
            //frames where foot is down: 5, 13
            case 1:
                if(frame == 5 || frame == 13){
                    _walkCue = true;
                    CULog("Walk %i", frame);
                }
                break;
            default:
                break;
        }
    } else{
        animFrameCounter++;
    }
    
    
//    CULog("%i", currentSpriteSheet->getFrame()/currentSpriteSheet->getCols());

//    CULog("%i", currentSpriteSheet->getFrame());
    
//    CULog("%i",currentSpriteSheet->getFrameCoords().first);
//    CULog("%i",currentSpriteSheet->getFrameCoords().second);

    currentSpriteSheet->getFrameCoords();
    
    CapsuleObstacle::update(dt);
    
    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
    }
}


#pragma mark -
#pragma mark Scene Graph Methods
/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void PlayerModel::resetDebug() {
    CapsuleObstacle::resetDebug();
    float w = DUDE_SSHRINK*_dimension.width;
    float h = SENSOR_HEIGHT;
    Poly2 poly(Rect(-w/2.0f,-h/2.0f,w,h));

    _sensorNode = scene2::WireNode::allocWithTraversal(poly, poly2::Traversal::INTERIOR);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width/2.0f, 0.0f));
    _debug->addChild(_sensorNode);
}




