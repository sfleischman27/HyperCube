//
//  GameModel.h
//  Platformer
//
//  Light model that holds the Player data, Plane data, and other game state data
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef GameModel_h
#define GameModel_h
#include <cugl/cugl.h>
#include "Collectible.h"
#include "PlayerModel.h"
#include "Mesh.h"
#include "Glowstick.h"
#include "GameItem.h"
#include "Trigger.h"

using namespace cugl;

/** Number of glowsticks allowed to put */
#define NUM_GLOWSTICKS 4

/**
 * A class representing an active level and its starting data
 */
class GameModel{
   
#pragma mark Initialization
private:
    /** level id */
    std::string _name;
    /** starting player location */
    Vec3 _startPlayerLoc;
    /** starting plane normal */
    Vec3 _startPlaneNorm;
public:
    /** exit of the game*/
    std::shared_ptr<GameItem> _exit;
    /** background pic of the level TODO JACK update this */
    std::shared_ptr<Texture> backgroundPic;
    
#pragma mark Player State
public:
    /** Player */
    std::shared_ptr<PlayerModel> _player;

    /** Player 3D Location */
    Vec3 _player3DLoc;
    
    /** If player just finishes rotating the cut */
    bool _justFinishRotating = false;
    
    bool _endOfGame = false;

    std::shared_ptr<Timestamp> _deathTime;
    std::shared_ptr<Timestamp> _currentTime;
    const float timeToNormalSinceDeath = 500; // in milliseconds
    
#pragma mark Plane State
private:
    /** Plane normal vector */
    Vec3 _norm;

    /**The origin of the cut plane AND 3d origin from which physics world is defined*/
    Vec3 _origin;
    
#pragma mark Cut State
private:
    /** Vector of cut polygons */
    std::vector<Poly2> _cut;
    
#pragma mark Collectibles State
public:
    /** map of collectibles */
    std::map<std::string, Collectible> _collectibles;



#pragma mark Decorations State
public:
    /** Vector of decorations */
    std::vector<std::shared_ptr<GameItem>> _decorations;

#pragma mark Triggers and Popups
public:
    /** Vector of triggers */
    std::vector<std::shared_ptr<Trigger>> _triggers;
    
    std::shared_ptr<Popups> _popup;
    
    std::shared_ptr<cugl::scene2::SceneNode> _rotatePopup;
    
#pragma mark Backpack State
public:
    /** Vector of collectibles in player backpack */
    std::unordered_set<std::string> _backpack;
private:
    /** Vector of collectibles need to be collected*/
    std::unordered_set<std::string> _expectedCol;
    
#pragma mark Glowsticks State
public:
    /** Vector of glowsticks */
    std::vector<Glowstick> _glowsticks;
    
    std::shared_ptr<cugl::scene2::Label> _glowstickCounter;

#pragma mark Compass State
public:
    std::shared_ptr<cugl::scene2::Label> _compassNum;


#pragma mark navigator State
    std::shared_ptr<cugl::scene2::Button> _navigator;

    /** navigator target location**/
    cugl::Vec3 _nav_target;
    
    /** the SpriteNode for the compass animations*/
    std::shared_ptr<cugl::scene2::SpriteNode> _compassSpin;
    
#pragma mark Lights
public:
    // Light objects
    struct Light {
        Vec3 color;
        float intensity;
        Vec3 loc;

        Light(Vec3 color, float intensity, Vec3 loc) {
            this->color = color;
            this->intensity = intensity;
            this->loc = loc;
        }
        Light(){}
    };
    /** Vector of lights */
    std::vector<Light> _lights;
    /** A map of active lights of the GameItem
     *  The key is the string representing the light's location
     */
    std::unordered_map<std::string, Light> _lightsFromItems;
    
#pragma mark Meshes
public:
    /** Level rendering mesh object */
    std::shared_ptr<PivotMesh> _renderMesh;
    /** Level collision mesh object */
    std::shared_ptr<PivotMesh> _colMesh;
    
#pragma mark Main Functions
public:
    /**
     * Creates the model state.
     */
    GameModel() {
        _collectibles = std::map<std::string, Collectible>();
        _glowsticks = std::vector<Glowstick>();
        _lights = std::vector<Light>();
        _decorations = std::vector<std::shared_ptr<GameItem>>();
        _deathTime = std::make_shared<Timestamp>();
        _currentTime = std::make_shared<Timestamp>();
        _popup = std::make_shared<Popups>(Popups());
    }
    
#pragma mark Getters and Setters
public:
    /**
     *  Sets the level name
     *
     *  @param name   The level name
     */
    void setName(std::string name) {
        _name = name;
    }

    /**
     *  Gets the level name
     */
    std::string getName() {
        return _name;
    }
    
    /**
     *  Sets the initial player location
     *
     *  @param loc   The player location
     */
    void setInitPlayerLoc(Vec3 loc) {
        _startPlayerLoc = loc;
    }

    /**
     *  Gets the initial player location
     */
    Vec3 getInitPlayerLoc() {
        return _startPlayerLoc;
    }
    
    /**
     *  Sets the initial plane norm
     *
     *  @param norm   The plane norm
     */
    void setInitPlaneNorm(Vec3 norm) {
        _startPlaneNorm = norm;
    }

    /**
     *  Gets the initial plane norm
     */
    Vec3 getInitPlaneNorm() {
        return _startPlaneNorm;
    }
    
    /**
     *  sets the exit object
     *
     *  @param exit
     */
    void setExit(std::shared_ptr<GameItem> exit) {
        _exit = exit;
    }
    
    /**
     * Sets the player model
     *
     * @param player  The player object
     */
    void setPlayer(std::shared_ptr<PlayerModel> player){
        _player = player;
    }
    
    /**
     *  Sets the player 3d location
     *
     *  @param player3DLoc   The player 3D loc
     */
    void setPlayer3DLoc(Vec3 player3DLoc) {
        _player3DLoc = player3DLoc;
    }

    /**
     *  Gets the player 3d location
     */
    Vec3 getPlayer3DLoc() {
        return _player3DLoc;
    }

    /**
     *  Sets the Norm
     * 
     * NOTE: YOU should not use this method to set the plane normal,
     * instead use the methods available through  the PLANE CONTROLLER
     * 
     * IF you do use this method it will NOT recompute the cut
     */
    void setPlaneNorm( Vec3 norm) {
        _norm = norm;
    }

    /**
     *  Gets the Current Norm
     */
    Vec3 getPlaneNorm() {
        return _norm;
    }

    /**
     *  Sets the plane origin
     *
     * NOTE: YOU should not use this method to set the plane origin,
     * instead use the methods available through  the PLANE CONTROLLER
     *
     * IF you do use this method it will NOT recompute the cut
     */
    void setPlaneOrigin(Vec3 origin) {
        _origin = origin;
    }

    /**
     *  Gets the Current plane origin
     */
    Vec3 getPlaneOrigin() {
        return _origin;
    }
    
    /**
     *  Sets the cut
     *
     *  @param cut          The cut
     */
    void setCut(std::vector<Poly2> cut) {
        _cut = cut;
    }

    /**
     *  Gets the cut
     */
    std::vector<Poly2> getCut() {
        return _cut;
    }
    
    /**
     * Gets the collision mesh
     */
    std::shared_ptr<PivotMesh> getColMesh() {
        return _colMesh;
    }
    
    /**
     * Gets the render mesh
     */
    std::shared_ptr<PivotMesh> getRenderMesh() {
        return _renderMesh;
    }
    
    // this should not be here -Jolene
    /**
     * Returns if the player is touching the ground
     */
    bool touchingGround(){
        return true;
    }

    /**
     * Gets the map of collectibles
     */
    std::map<std::string, Collectible> getCollectibles() {
        return _collectibles;
    }

    /**
     * Gets the vector of decorations
     */
    std::vector<std::shared_ptr<GameItem>> getDecorations() {
        return _decorations;
    }
    
    /**
     * Initializes collectibles with locations and textures
     *
     * @param locs  List of collectible locations
     * @param texs  List of collectible textures
     */
    void setCollectibles(std::vector<Vec3> locs, std::vector<std::shared_ptr<cugl::Texture>> texs) {
        for(int i = 0; i < locs.size(); i++) {
            Collectible item = Collectible(locs[i], std::to_string(i));
            item.setTexture(texs[i]);
            _collectibles.insert({std::to_string(i), item});
            _expectedCol.insert(std::to_string(i));
        }
        
    }
    
    void clearPopups() {
        _popup->clear();
    }
    
    void clearCollectibles() {
        _collectibles.clear();
    }
    
    void clearGlowsticks() {
        _glowsticks.clear();
        updateGlowstickCount();
    }
    
    void clearLights() {
        _lights.clear();
        _lightsFromItems.clear();
    }
    
    void clearDecorations() {
        _decorations.clear();
    }
    
    void clearBackpack(){
        _backpack.clear();
    }

    void clearTriggers() {
        _triggers.clear();
    }

    void setExpectedCol(std::unordered_set<std::string> expectedCol) {
        _expectedCol = expectedCol;
    }

    std::unordered_set<std::string> getExpectedCol() {
        return _expectedCol;
    }
    
    int getColNum() {
        return _collectibles.size();
    }
    
    int getCurrColNum() {
        return _backpack.size();
    }
    
    /**
     * check if player has collected all required collectibles
     * expectedCol right now is just all existing collectibles
     */
    bool checkBackpack() {
        return _expectedCol == _backpack;
    }
    
    void updateGlowstickCount() {
        _glowstickCounter->setText(std::to_string(NUM_GLOWSTICKS - _glowsticks.size()));
    }

#pragma mark Compass Methods
    /**Get the global rotation of the plane relative to world space vector (1,0,0) in degrees*/
    float getGlobalAngleDeg() {
        auto basis = Vec3(1, 0, 0);
        auto dot = _norm.dot(basis);     // Dot product between[x1, y1] and [x2, y2]
        auto det = _norm.x * basis.y - _norm.y * basis.x;      // Determinant
        return atan2(det, dot) * 180 / M_PI;
    }

    /**Get the 2d position and angle in radians which the navigator should rotate to point towards the exit*/
    std::pair<Vec2, std::vector<float>> getNavigatorTransforms() {
        //get basis right
        auto z = Vec3(0, 0, 1);
        z.cross(_norm);
        z.normalize();
        //get 2d screen coordinate
        auto location = _nav_target - getPlayer3DLoc();
        auto norm = _norm;
        
        //dot the point onto the plane normal to get the distance from the cut plane
        auto dist = location.dot(norm);
        // get the point projected onto the plane basis vectors (unit_z is always y-axis and x-axis is to the right)
        auto xcoord = location.dot(cugl::Vec3(0, 0, 1).cross(norm.negate()).normalize());
        auto ycoord = location.dot(cugl::Vec3(0, 0, 1));
        auto projected = cugl::Vec2(xcoord, ycoord);

        auto basis = Vec2(0,1);
        auto dot = projected.dot(basis);     // Dot product between[x1, y1] and [x2, y2]
        auto det = projected.x * basis.y - projected.y * basis.x;      // Determinant
        auto angle =  atan2(det, dot);

        std::vector<float> nums{ angle, dist };
        return std::pair<Vec2, std::vector<float>>(projected, nums);
    }
    
    void updateNavigator() {
        auto stuff = getNavigatorTransforms();
        _navigator->setAngle(stuff.second[0]);
        auto off = stuff.first;
        float rad = 100;
        if (off.length() < rad) { rad = off.length(); }
        //CULog(std::to_string(rad).c_str());
        _navigator->setScale(Vec2(0.5, rad/200));

        auto crad = 300.0f;
        auto tint = stuff.second[1];
        if (tint > 0) {
            auto t = std::min(1.0f, tint / crad);
            _navigator->setColor(cugl::Color4(255, 160, 0, 255) * t + cugl::Color4::WHITE * (1-t));
        }
        else {
            auto t = std::min(1.0f, -tint / crad);
            _navigator->setColor(cugl::Color4(0, 255, 0, 255) * t + cugl::Color4::WHITE * (1-t));
        }

        

    }
    
    void updateCompassNum() {
        int angle = static_cast<int>(getGlobalAngleDeg());
        _compassNum->setText(std::to_string(angle));
        int frame = (angle + 360) % 10;
        _compassSpin->setFrame(frame);
        // fade in
        auto color = _compassSpin->getColor();
        auto newColor = Color4(color.r, color.g, color.b, std::min(color.a + 10, 255));
        _compassSpin->setColor(newColor);
        // set visible
        _compassSpin->setVisible(true);
    }    
};

#endif /* GameModel_h */
