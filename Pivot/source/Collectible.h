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
    /** The drawing scale of the collectibles (can vary in size) */
    float _scale;
    /** The sprite sheet for the collectible */
    std::shared_ptr<SpriteSheet> _sprite;
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
    /**
     *  Sets the scale of the collectible
     *
     *  @param s
     */
    void setScale(float s) {
        _scale = s;
    }
    /**
     * Sets the sprite sheet for this collectible
     *
     * @param sprite  The sprite sheet for this collectible
     */
    void setSprite(const std::shared_ptr<SpriteSheet>& sprite) {
        _sprite = sprite;
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
     *  Returns the scale of the collectible
     */
    float getScale() { return _scale; }
    /**
     * Returns the sprite sheet for this collectible
     */
    const std::shared_ptr<SpriteSheet>& getSprite() const {
        return _sprite;
    }
    
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
    bool canBeSeen(Vec3 planeNorm);
    /**
     *  Determines if the collectibles can be collected by the player
     *  if the player position is within the collecting-enabled distance
     *
     *  @param playerPos    The current player position
     */
    bool canBeCollected(Vec2 playerPos, Vec2 clickPos);
    
    void setTexture(const std::shared_ptr<cugl::Texture>& value);
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,const std::shared_ptr<cugl::Texture>& value, cugl::Rect pos);
    
};

#endif /* Collectible_h */

