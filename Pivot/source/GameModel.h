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

#pragma mark Graphics Settings
    /** The shade depth of the level */
    float shadeDepth;
    /** The shade color for the level */
    Color4f shadeColor;
    /** The background color for the level */
    Color4f bgColor;
    /** background pic of the level */
    std::shared_ptr<Texture> backgroundPic;
    /** ambient light color and level*/
    Color4f ambientLight;

    /** color of cutline*/
    Color4f cutLineColor;

    /** color tint of vorinoi fill*/
    Color4f cutFillColor;

    /** Boolean for the cut outline */
    bool drawOutline = true;

#pragma mark Player State
public:
    /** Player */
    std::shared_ptr<PlayerModel> _player;

    /** Player 3D Location */
    Vec3 _player3DLoc;

    /** If player just finishes rotating the cut */
    bool _justFinishRotating = false;

    bool _endOfGame = false;

#pragma mark Timestamps
    /** Player time of death */
    std::shared_ptr<Timestamp> _deathTime;
    /** Time to pixel in */
    std::shared_ptr<Timestamp> _pixelInTime;
    /** Time to pixel out */
    std::shared_ptr<Timestamp> _pixelOutTime;
    /** Current time */
    std::shared_ptr<Timestamp> _currentTime;
    /** Player just got a collectible */
    std::shared_ptr<Timestamp> _collectTime;
    
    /** True while pixeling in, false when pixeling out*/
    bool _pixelingIn;
    /** True when done pixeling out */
    bool _donePixelOut;
    /** True when done pixeling in */
    bool _donePixelIn;
    /** True when first starts level */
    bool _startOfLevel;
    
    const float timeToNormalSinceDeath = 500; // in milliseconds
    /** Time to pixel in */
    const float timeToPixelIn = 1250; // in milliseconds
    /** Time to pixel in */
    const float timeToPixelOut = 1250; // in milliseconds
    /** Time to pixel in */
    const float timeFromPixelInToOutline = 0; // in milliseconds
    /** Time to pixel in */
    const float timeToOutline = 750; // in milliseconds

#pragma mark Plane State
private:
    /** Plane normal vector */
    Vec3 _norm;

    /**The origin of the cut plane AND 3d origin from which physics world is defined*/
    Vec3 _origin;

#pragma mark Cut State
private:
    /** Vector of cut polygons */
    std::vector<std::shared_ptr<Poly2>> _cut;

#pragma mark Collectibles State
public:
    /** map of collectibles */
    std::map<std::string, Collectible> _collectibles;
    
    std::shared_ptr<cugl::scene2::SceneNode> _invent;
    
    std::shared_ptr<cugl::scene2::SceneNode> _inventodd;
    
    int initInventAlpha;
    
    int initOddAlpha;



#pragma mark Decorations and Poster State
public:
    /** Vector of decorations */
    std::vector<std::shared_ptr<GameItem>> _decorations;
    
    /** Vector of posters */
    std::vector<std::shared_ptr<GameItem>> _posters;

#pragma mark Triggers and Popups
public:
    /** Vector of triggers */
    std::vector<std::shared_ptr<Trigger>> _triggers;

    std::shared_ptr<Popups> _popup;

    std::shared_ptr<cugl::scene2::SceneNode> _rotatePopup;
    
    std::shared_ptr<cugl::scene2::SceneNode> _collectPopup;
    
    std::shared_ptr<cugl::scene2::SceneNode> _glowPopup;
    
    std::shared_ptr<cugl::scene2::SceneNode> _jumpPopup;
    
    std::shared_ptr<cugl::scene2::SceneNode> _runPopup;
    
    std::shared_ptr<cugl::scene2::SceneNode> _walkPopup;
    
    std::shared_ptr<Messages> _messages;
    
    std::shared_ptr<cugl::scene2::SceneNode> _messScene;
    
    std::shared_ptr<cugl::scene2::NinePatch> _messBack;
    
    std::shared_ptr<cugl::scene2::Label> _messText;

#pragma mark Backpack State
public:
    /** Vector of collectibles in player backpack */
    std::unordered_set<std::string> _backpack;
private:
    /** Vector of collectibles need to be collected*/
    std::unordered_set<std::string> _expectedCol;

#pragma mark Glowsticks State
public:
    /** Number of glowsticks */
    int _numGlowsticks;
    
    /** Vector of glowsticks */
    std::vector<Glowstick> _glowsticks;

    std::shared_ptr<cugl::scene2::Label> _glowstickCounter;
    
    std::map<int, std::shared_ptr<SpriteSheet>> _glowstickSprites;
    
    std::map<int, cugl::Vec3> _glowstickColors;
    
    int _glowstickOrder;

#pragma mark Compass State
public:
    std::shared_ptr<cugl::scene2::Label> _compassNum;
    /** compass degree setting true = 180, false = 360 */
    bool _degrees;


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
        float falloff;
        float pulse;

        Light(Vec3 color, float intensity, Vec3 loc, float falloff, float pulse) {
            this->color = color;
            this->intensity = intensity;
            this->loc = loc;
            this->falloff = falloff;
            this->pulse = pulse;
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
        _pixelInTime = std::make_shared<Timestamp>();
        _pixelOutTime = std::make_shared<Timestamp>();
        _currentTime = std::make_shared<Timestamp>();
        _collectTime = std::make_shared<Timestamp>();
        _popup = std::make_shared<Popups>(Popups());
        _messages = std::make_shared<Messages>(Messages());
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
    void setCut(std::vector<std::shared_ptr<Poly2>> cut) {
        _cut = cut;
    }

    /**
     *  Gets the cut
     */
    std::vector<std::shared_ptr<Poly2>> getCut() {
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
    void setCollectibles(std::vector<Vec3> locs, std::vector<std::shared_ptr<cugl::Texture>> texs, std::vector<std::shared_ptr<cugl::Texture>> normalTexs, std::vector<float> col_scales, std::vector<float> col_angles) {
        for(int i = 0; i < locs.size(); i++) {
            Collectible item = Collectible(locs[i], std::to_string(i));
            item.setScale(col_scales[i]);
            item.setOffsetAngle(col_angles[i]);
            item.setTexture(texs[i]);
            item.rotateSpriteSheet = SpriteSheet::alloc(texs[i], 6, 6);
            item.rotateNormalSpriteSheet = SpriteSheet::alloc(normalTexs[i], 6, 6);
            _collectibles.insert({std::to_string(i), item});
            _expectedCol.insert(std::to_string(i));
        }

    }

    void clearPopups() {
        _popup->clear();
    }
    
    void clearMessages() {
        _messages->clear();
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
    
    void clearExpectedCol(){
        _expectedCol.clear();
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
        _glowstickCounter->setText(std::to_string(_numGlowsticks - _glowsticks.size()));
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
        if (_degrees){ // 0 -> 180
            _compassNum->setText(std::to_string(angle));
        } else { // 0 -> 360
            if (angle < 0){ angle = angle + 360; }
            _compassNum->setText(std::to_string(angle));
        }
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
