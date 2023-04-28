//
//  PlayerModel.h
//  Platformer
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  Created by JD on 2/22/23.
//

#ifndef PlayerModel_h
#define PlayerModel_h
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define DUDE_TEXTURE    "dude"
/** Identifier to allow us to track the sensor in ContactListener */
#define SENSOR_NAME     "dudesensor"


#pragma mark -
#pragma mark Physics Constants
/** The factor to multiply by the input */
#define DUDE_FORCE      1000.0f
/** The amount to slow the character down */
#define DUDE_DAMPING    1.0f
/** The maximum character speed */
#define DUDE_MAXWALKSPEED   60.0f
/** The maximum character speed */
#define DUDE_MAXRUNSPEED   240.0f


#pragma mark -
#pragma mark Player Model
/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class PlayerModel : public cugl::physics2::CapsuleObstacle {
public:
    std::shared_ptr<cugl::SpriteSheet> currentSpriteSheet;
    
    std::shared_ptr<cugl::SpriteSheet> currentNormalSpriteSheet;
    
    /** the SpriteSheets for the various player animations*/
    std::unordered_map<std::string, std::pair<std::shared_ptr<cugl::SpriteSheet>, std::shared_ptr<cugl::SpriteSheet>>> spriteSheets;
    
    float movementValue;
    
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(PlayerModel);

protected:
    /** The current horizontal movement of the character */
    float _movement;
    /** Which direction is the character facing */
    bool _faceRight;
    /** How long until we can jump again */
    int  _jumpCooldown;
    /** Whether we are actively jumping */
    bool _isJumping;
    
    float fallAccelerationAcc;
    
    /** Whether we are actively jumping */
    bool _isRunning;
    /** How long until we can shoot again */
    int  _shootCooldown;
    /** Whether our feet are on the ground */
    bool _isGrounded;
    /** Whether we are actively shooting */
    bool _isShooting;
    /** Ground sensor to represent our feet */
    b2Fixture*  _sensorFixture;
    /** Reference to the sensor name (since a constant cannot have a pointer) */
    std::string _sensorName;
    /** The node for debugging the sensor */
    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;

    /** The scene graph node for the Dude. */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    
    int animFrameCounter = 0;
    
    int animState = 0;
    /** The current velocity of the player in 2D*/
    cugl::Vec2 _vel;
    /** The location of the player in 3D*/
    cugl::Vec3 _3DLoc;

    /**
    * Redraws the outline of the physics fixtures to the debug node
    *
    * The debug node is use to outline the fixtures attached to this object.
    * This is very useful when the fixtures have a very different shape than
    * the texture (e.g. a circular shape attached to a square texture).
    */
    virtual void resetDebug() override;

public:
#pragma mark Sound + Sound Cues
    bool _jumpCue;
    bool _walkCue;
    
    
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a PlayerModel, you must call init().
     */
    PlayerModel() : CapsuleObstacle(), _sensorName(SENSOR_NAME) { }
    
    /**
     * Destroys this PlayerModel, releasing all resources.
     */
    virtual ~PlayerModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this PlayerModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a PlayerModel may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Initializes a new dude at the origin.
     *
     * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(cugl::Vec2::ZERO, cugl::Size(1,1), 1.0f); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude is unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos) override { return init(pos, cugl::Size(1,1), 1.0f); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size) override {
        return init(pos, size, 1.0f);
    }
    
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
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    
#pragma mark -
#pragma mark Static Constructors
    /**
     * Creates a new dude at the origin.
     *
     * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  A newly allocated PlayerModel at the origin
     */
    static std::shared_ptr<PlayerModel> alloc() {
        std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
        return (result->init() ? result : nullptr);
    }

    /**
     * Creates a new dude at the given position.
     *
     * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  A newly allocated PlayerModel at the given position
     */
    static std::shared_ptr<PlayerModel> alloc(const cugl::Vec2& pos) {
        std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
        return (result->init(pos) ? result : nullptr);
    }

    /**
     * Creates a new dude at the given position.
     *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  A newly allocated PlayerModel at the given position with the given scale
     */
    static std::shared_ptr<PlayerModel> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
        std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
        return (result->init(pos, size) ? result : nullptr);
    }

    /**
     * Creates a new dude at the given position.
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
     * @return  A newly allocated PlayerModel at the given position with the given scale
     */
    static std::shared_ptr<PlayerModel> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    

#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this PlayerModel.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this PlayerModel.
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }

    /**
     * Sets the scene graph node representing this PlayerModel.
     *
     * Note that this method also handles creating the nodes for the body parts
     * of this PlayerModel. Since the obstacles are decoupled from the scene graph,
     * initialization (which creates the obstacles) occurs prior to the call to
     * this method. Therefore, to be drawn to the screen, the nodes of the attached
     * bodies must be added here.
     *
     * The bubbler also uses the world node when adding bubbles to the scene, so
     * the input node must be added to the world BEFORE this method is called.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this PlayerModel, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) {
        _node = node;
        _node->setPosition(getPosition() * _drawScale);
    }
    
    void setSpriteSheet(std::string animationName){
        //CULog(animationName.c_str());
        currentSpriteSheet = spriteSheets.find(animationName)->second.first;
        currentNormalSpriteSheet = spriteSheets.find(animationName)->second.second;
    }
    
    void resetOtherSpritesheets(std::string animationName) {
        for(auto it = spriteSheets.begin(); it != spriteSheets.end(); it++){
            if(it->first != animationName){
                it->second.first->setFrame(0);
                it->second.second->setFrame(0);
            }
        }
    }

    
#pragma mark -
#pragma mark Attribute Properties
    /**
     * Returns left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @return left/right movement of this character.
     */
    float getMovement() const { return _movement; }
    
    /**
     * Sets left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @param value left/right movement of this character.
     */
    void setMovement(float value);
    
    /**
     * Returns true if the dude is actively firing.
     *
     * @return true if the dude is actively firing.
     */
    bool isShooting() const { return _isShooting && _shootCooldown <= 0; }
    
    /**
     * Sets whether the dude is actively firing.
     *
     * @param value whether the dude is actively firing.
     */
    void setShooting(bool value) { _isShooting = value; }
    
    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    bool isJumping() const { return _isJumping && _jumpCooldown <= 0; }
    
    /**
     * Sets whether the dude is actively jumping.
     *
     * @param value whether the dude is actively jumping.
     */
    void setJumping(bool value) { _isJumping = value; }
    
    /**
     * Sets whether the dude is actively running.
     *
     * @param value whether the dude is actively running.
     */
    void setRunning(bool value) { _isRunning = value; }
    
    /**
     * Returns true if the dude is on the ground.
     *
     * @return true if the dude is on the ground.
     */
    bool isGrounded() const { return _isGrounded; }
    
    /**
     * Sets whether the dude is on the ground.
     *
     * @param value whether the dude is on the ground.
     */
    void setGrounded(bool value) { _isGrounded = value; fallAccelerationAcc = 1.1f;}
    
    /**
     * Returns how much force to apply to get the dude moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the dude moving
     */
    float getForce() const { return DUDE_FORCE; }
    
    /**
     * Returns ow hard the brakes are applied to get a dude to stop moving
     *
     * @return ow hard the brakes are applied to get a dude to stop moving
     */
    float getDamping() const { return DUDE_DAMPING; }
    
    /**
     * Returns the upper limit on dude left-right movement.
     *
     * This does NOT apply to vertical movement.
     *
     * @return the upper limit on dude left-right movement.
     */
    float getMaxWalkSpeed() const { return DUDE_MAXWALKSPEED; }
    
    float getMaxRunSpeed() const { return DUDE_MAXRUNSPEED; }
    
    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string* getSensorName() { return &_sensorName; }
    
    /**
     * Returns true if this character is facing right
     *
     * @return true if this character is facing right
     */
    bool isFacingRight() const { return _faceRight; }
    
    /**
     * Returns current velocity of this character.
     *
     * @return velocity of this character.
     */
    cugl::Vec2 getVelocity() const { return _vel; }
    
    /**
     * Sets velocity of this character.
     *
     * @param velocity velocity of this character.
     */
    void setVelocity(cugl::Vec2 velocity) { _vel = velocity; }
    
    /**
     * Returns current 3D location of the player
     *
     * @return 3D location of the player
     */
    cugl::Vec3 get3DLoc() const { return _3DLoc; }
    
    /**
     * Sets 3D locaiton of the player
     *
     * @param loc  3D locaiton of the player
     */
    void set3DLoc(cugl::Vec3 loc) { _3DLoc = loc; }
    
    void setSprite(const std::shared_ptr<cugl::SpriteSheet>& sprite) {
        currentSpriteSheet = sprite;
    }
    
    void setNormalSprite(const std::shared_ptr<cugl::SpriteSheet>& normalsprite) {
        currentNormalSpriteSheet = normalsprite;
    }

    
#pragma mark -
#pragma mark Physics Methods
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;
    
    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt) override;
    
    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();


    
};

#endif /* PlayerModel_h */
