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

class GameItem{
    
protected:
    /** The name of the item. */
    std::string _name;
    /** 3D position of the item*/
    Vec3 _position;
    /** The texture for the item. */
    std::shared_ptr<cugl::Texture> _texture;
    
public:
    /**
     * Creates the game item
     *
     * @param pos          The position of the collectible
     * @param name        The name/identifier of the collectible
     */
    GameItem(const Vec3 pos, const std::string name) {
        setName(name);
        setPosition(pos);
    }
    
    GameItem(const Vec3 pos) {
        setPosition(pos);
    }
    
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
};

#endif /* GameItem_h */
