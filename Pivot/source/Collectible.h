//
//  Collectible.h
//  Platformer
//
//  Created by Jolene Mei on 2/27/23.
//

#ifndef Collectible_h
#define Collectible_h
#include <cugl/cugl.h>

using namespace cugl;

class Collectible{
    
protected:
    /** Collectible name/identifier */
    std::string _name;
    /** Collectible position */
    Vec3 _position;
    /** Collectible status */
    bool _collected;
    /** The texture for the asteroid sprite sheet. */
    std::shared_ptr<cugl::Texture> _texture;

    
public:
    /**
     * Creates the collectible.
     *
     * @param pos          The position of the collectible
     * @param name        The name/identifier of the collectible
     */
    Collectible(const Vec3 pos, const std::string name) {
        setName(name);
        setPosition(pos);
        setCollected(false);
    }
    
#pragma mark Setters
public:
    /**
     *  Sets the name of the collectible
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
     *  Sets the collectible status
     *
     *  @param v
     */
    void setCollected(bool v) {
        _collected = v;
    }
    
#pragma mark Getters
public:
    /**
     *  Returns the name of the collectible
     */
    std::string getName() { return _name; }
    /**
     *  Returns the position of the collectible
     */
    Vec3 getPosition() { return _position; }
    /**
     *  Returns the collectible status
     */
    bool getCollected() { return _collected; }
    /**
     *  Returns the texture
     */
    std::shared_ptr<Texture> getTexture() { return _texture; }
    
#pragma mark Gameplay Methods
public:
    /**
     *  Determines if the collectibles can be seen in current cut
     *  if the plane norm is within +/- 10 degrees of the
     *  perpendicular norm of the collectible position
     *  TODO: How to decide if the plane go through the collectibe
     *
     *  @param planeNorm    The current plane norm vector
     */
    bool canBeSeen(Vec3 playerPos, Vec3 planeNorm);
    
    void setTexture(const std::shared_ptr<cugl::Texture>& value);
};

#endif /* Collectible_h */

