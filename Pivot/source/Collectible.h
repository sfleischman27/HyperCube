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
    
    Collectible(const Vec3 pos, const std::string name, const std::shared_ptr<cugl::Texture>& text, float offangle, float scale) : GameItem(pos, name, text, offangle, scale){}
    
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
     *
     *  @param playerPos    The current player 3D position
     *  @param planeNorm    The current plane norm vector
     */
    bool canBeSeen(Vec3 playerPos, Vec3 planeNorm) override {
        float dist = (getPosition()-playerPos).dot(planeNorm);
        if (!getCollected() && dist <= VISIBLE_DIST) {
            return true;
        }
        return false;
    }
};

#endif /* Collectible_h */

