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
    
#pragma mark Player State
public:
    /** Player */
    std::shared_ptr<PlayerModel> _player;

    /** Player 3D Location */
    Vec3 _player3DLoc;
    
    /** If player just finishes rotating the cut */
    bool _justFinishRotating = false;
    
    bool _endOfGame = false;
    
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
    /** Vector of collectibles */
    std::unordered_map<std::string, Collectible> _collectibles;
    
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
    };
    /** Vector of lights */
    std::vector<Light> _lights;
    
#pragma mark Mesh
public:
    /** Level mesh object */
    std::shared_ptr<PivotMesh> _mesh;
    
#pragma mark Main Functions
public:
    /**
     * Creates the model state.
     */
    GameModel() {
        _collectibles = std::unordered_map<std::string, Collectible>();
        _glowsticks = std::vector<Glowstick>();
        _lights = std::vector<Light>();
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
     * Sets the mesh
     */
    void setMesh(std::shared_ptr<PivotMesh> mesh){
        _mesh = mesh;
    }
    
    /**
     * Gets the mesh
     */
    std::shared_ptr<PivotMesh> getMesh() {
        return _mesh;
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
    std::unordered_map<std::string, Collectible> getCollectibles() {
        return _collectibles;
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
    
    void clearCollectibles() {
        _collectibles.clear();
    }
    
    void clearGlowsticks() {
        _glowsticks.clear();
    }
    

    void clearLights() {
        _lights.clear();
    }

    void setExpectedCol(std::unordered_set<std::string> expectedCol) {
        _expectedCol = expectedCol;
    }

    std::unordered_set<std::string> getExpectedCol() {
        return _expectedCol;
    }
    
    /**
     * check if player has collected all required collectibles
     * expectedCol right now is just all existing collectibles
     */
    bool checkBackpack() {
        return _expectedCol == _backpack;
    }
    
};

#endif /* GameModel_h */
