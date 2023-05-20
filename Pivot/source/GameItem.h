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
    /** The norm for the item. */
    std::shared_ptr<cugl::Texture> _norm;
    /** the color of the light of the item*/
    Vec3 _color;
    /** The intensity of the light of the item. */
    float _intense;
    /** The radius of the light of the item. */
    float _radius;
    /** The pulse of the light of the item. */
    float _pulse;
    /** map of rotation textures of the item */
    std::unordered_map<float, std::shared_ptr<cugl::Texture>> _rotateTextures;
    /** If the GameItem is emissive */
    bool _isemit = false;
    /** If the GameItem is a billboard */
    bool _isbill;
    /** the scale of the sprite being rendered */
    float _spriteScale;
    /** the offset angle of the item */
    float _offsetAngle;
    /** the normal of the item */
    Vec3 _normal;
    
public:
    std::shared_ptr<cugl::SpriteSheet> rotateSpriteSheet;
    std::shared_ptr<cugl::SpriteSheet> rotateNormalSpriteSheet;
    /**
     * Creates the game item
     *
     * @param pos          The position of the item
     * @param name        The name/identifier of the item
     * @param text       The teture of the item
     * @param offangle      The initial offset angle of the item
     * @param scale      The  scale of the item sprite
     */
    GameItem(const Vec3 pos, const std::string name, const std::shared_ptr<cugl::Texture>& text, float offangle, float scale) {
        setName(name);
        setPosition(pos);
        setTexture(text);
        setOffsetAngle(offangle);
        setScale(scale);
    }
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
     *  Sets the norm of the item
     *
     *  @param value
     */
    void setNorm(const std::shared_ptr<cugl::Texture>& value) {
        if (value) {
            _norm = value;
        } else {
            _norm = nullptr;
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
     *  Sets the radius of the light
     *
     *  @param radius
     */
    void setRadius(float radius) {
        _radius = radius;
    }
    /**
     *  Sets the rotation texture map
     *  @param map
     */
    void setRotateTextureMap(std::unordered_map<float, std::shared_ptr<cugl::Texture>> map) {
        _rotateTextures = map;
        // TODO: Jolene use helper function to implement the map after it parsed in data controller
    }
    
    void setRotationalSprite(float currentAngle){
        float repeat = 360.0f / ((float) rotateSpriteSheet->getSize());
        // float repeat = 10.0f;
        int neg = 1;
        int localAng = currentAngle;
        localAng = localAng % 360;
        localAng *= neg;
        localAng = localAng < 0 ? localAng + 360 : localAng;
        int index = (int) localAng / repeat;
        
        rotateSpriteSheet->setFrame(index);
        if(!_isemit){
            rotateNormalSpriteSheet->setFrame(index);
        }        
    }
    
    void setIsemit(bool isemit) {
        _isemit = isemit;
    }
    
    void setIsbill(bool isbill) {
        _isbill = isbill;
    }
    
    void setScale(float scale) {
        _spriteScale = scale;
    }
    
    void setOffsetAngle(float offangle) {
        _offsetAngle = offangle;
    }
    
    void setNormal(Vec3 normal) {
        _normal = normal;
    }
    
    void setPulse(float pulse) {
        _pulse = pulse;
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
     *  Returns the norm
     */
    std::shared_ptr<Texture> getNorm() { return _norm; }
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
    /**
     *  Gets the radiue of the light
     */
    float getRadius() { return _radius; }
    
    bool isEmissive() {return _isemit; }
    
    bool isBill() { return _isbill; }
    
    float getScale() { return _spriteScale; }
    
    float getOffsetAngle() { return _offsetAngle; }
    
    Vec3 getNormal() { return _normal; }
    
    float getPulse() { return _pulse; }
    
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
