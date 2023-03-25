//
//  Glowstick.h
//  Pivot
//
//  Created by Jolene Mei on 3/23/23.
//

#ifndef Glowstick_h
#define Glowstick_h
#include <cugl/cugl.h>

using namespace cugl;

class Glowstick{
    
protected:
    /** glowstick position */
    Vec3 _position;
    /** The texture for the glowstick. */
    std::shared_ptr<cugl::Texture> _texture;
    
    
public:
    /**
     * Creates the glowstick.
     *
     * @param pos          The position of the glowstick
     */
    Glowstick(const Vec3 pos) {
        setPosition(pos);
    }
    
#pragma mark Setters
public:
    /**
     *  Sets the position of the glowstick
     *
     *  @param pos
     */
    void setPosition(Vec3 pos) {
        _position = pos;
    }
    
#pragma mark Getters
public:
    /**
     *  Returns the position of the glowstick
     */
    Vec3 getPosition() { return _position; }
    
};

#endif /* Glowstick_h */

