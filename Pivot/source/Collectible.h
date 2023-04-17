//
//  Collectible.h
//  Pivot
//
//  Created by Jolene Mei on 2/27/23.
//

#ifndef Collectible_h
#define Collectible_h
#include <cugl/cugl.h>
#include "GameItem.h"

using namespace cugl;

class Collectible : public GameItem{
    
protected:
    /** Collectible status */
    bool _collected;
    
public:
    /**
     * Creates the collectible.
     *
     * @param pos          The position of the collectible
     * @param name        The name/identifier of the collectible
     */
    Collectible(const Vec3 pos, const std::string name) : GameItem(pos, name) {
        setCollected(false);
    }
    
#pragma mark Setters
public:
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
     *  Returns the collectible status
     */
    bool getCollected() { return _collected; }
    
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

