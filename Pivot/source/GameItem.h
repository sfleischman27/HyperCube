//
//  GameItem.h
//  Pivot
//
//  Created by Jolene Mei on 4/17/23.
//

#ifndef GameItem_h
#define GameItem_h
#include <cugl/cugl.h>

using namespace cugl;

/** Threshold of the visible distance */
#define VISIBLE_DIST  1

class GameItem{
    
protected:
    /** The name of the item. */
    std::string _name;
    /** 3D position of the item*/
    Vec3 _position;
    /** The texture for the item. */
    std::shared_ptr<cugl::Texture> _texture;
    /** the color of the light of the item*/
    Vec3 _color;
    /** The intensity of the light of the item. */
    float _intense;
    /** map of rotation textures of the item */
    std::unordered_map<float, std::shared_ptr<cugl::Texture>> _rotateTextures;
    
public:
    /**
     * Creates the game item
     *
     * @param pos          The position of the item
     * @param name        The name/identifier of the item
     * @param text       The teture of the item
     */
    GameItem(const Vec3 pos, const std::string name, const std::shared_ptr<cugl::Texture>& text) {
        setName(name);
        setPosition(pos);
        setTexture(text);
    }
    /**
     * Creates the game item
     *
     * @param pos          The position of the item
     * @param name        The name/identifier of the item
     */
    GameItem(const Vec3 pos, const std::string name) {
        setName(name);
        setPosition(pos);
    }
    /**
     * Creates the game item
     *
     * @param pos          The position of the item
     * @param text       The teture of the item
     */
    GameItem(const Vec3 pos, const std::shared_ptr<cugl::Texture>& text) {
        setPosition(pos);
        setTexture(text);
    }
    /**
     * Creates the game item
     *
     * @param pos          The position of the item
     */
    GameItem(const Vec3 pos) {
        setPosition(pos);
    }
    /**
     * default constructor with no parameter
     */
    GameItem(){}
    
#pragma mark Setters
public:
    /**
     *  Sets the name of the item
     *
     *  @param n
     */
    void setName(std::string n) {
        _name = n;
    }
    /**
     *  Sets the position of the collectible
     *
     *  @param pos
     */
    void setPosition(Vec3 pos) {
        _position = pos;
    }
    /**
     *  Sets the texture of the item
     *
     *  @param value
     */
    void setTexture(const std::shared_ptr<cugl::Texture>& value) {
        if (value) {
            _texture = value;
        } else {
            _texture = nullptr;
        }
    }
    /**
     *  Sets the color of the light
     *
     *  @param color
     */
    void setColor(Vec3 color) {
        _color = color;
    }
    /**
     *  Sets the intensity of the light
     *
     *  @param intense
     */
    void setIntense(float intense) {
        _intense = intense;
    }
    /**
     *  Sets the rotation texture map
     *  @param map
     */
    void setRotateTextureMap(std::unordered_map<float, std::shared_ptr<cugl::Texture>> map) {
        _rotateTextures = map;
        // TODO: Jolene use helper function to implement the map after it parsed in data controller
    }
    
#pragma mark Getters
public:
    /**
     *  Returns the name of the item
     */
    std::string getName() { return _name; }
    /**
     *  Returns the position of the item
     */
    Vec3 getPosition() { return _position; }
    /**
     *  Returns the texture
     */
    std::shared_ptr<Texture> getTexture() { return _texture; }
    /**
     *  Returns the rotation texture given the angle
     *  @param angle
     */
    std::shared_ptr<Texture> getTexture(float angle) {
        if (_rotateTextures.find(angle) != _rotateTextures.end()) {
            return _rotateTextures[angle];
        }
        else{
            std::cout << "Key not found in the _rotateTextures."<< std::endl;
            return nullptr;
        }
    }
    /**
     *  Gets the color of the light
     */
    Vec3 getColor() { return _color; }
    /**
     *  Gets the intensity of the light
     */
    float getIntense() { return _intense; }
    
#pragma mark Helper Methods
public:
    /**
     *  Format a map to store (angle, texture) mapping for the item
     *  @param
     */
    void setRotateTextureMap();
    /**
     *  Determines if the GameItem can be seen in player's current position and current cut
     *
     *  @param playerPos    The current player 3D position
     *  @param planeNorm    The current plane norm vector
     */
    virtual bool canBeSeen(Vec3 playerPos, Vec3 planeNorm) {
        float dist = (_position - playerPos).dot(planeNorm);
        if (dist <= VISIBLE_DIST) {
            return true;
        }
        return false;
    }
};

#endif /* GameItem_h */
