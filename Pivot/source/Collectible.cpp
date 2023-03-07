//
//  Collectible.cpp
//  Platformer
//
//  Created by Jolene Mei on 2/28/23.
//

#include "Collectible.h"

using namespace cugl;
using namespace std;

///** Threshold of the click distance */
//#define CLICK_DIST   0.003
///** Threshold of the pickup distance */
//#define PICKUP_DIST   0.1

#pragma mark Collectible
/**
 *  Determines if the collectibles can be seen in current cut
 *  if the plane norm is within +/- 10 degrees of the
 *  perpendicular norm of the collectible position
 *
 *  @param planeNorm    The current plane norm vector
 */
bool Collectible::canBeSeen(Vec3 planeNorm) {
    // Maybe later, right now all check codes are in GamePlayController
    return true;
}
/**
 *  Determines if the collectibles can be collected by the player
 *  if the player position is within the collecting-enabled distance
 *  @param playerPos    The current player position
 */
bool Collectible::canBeCollected(Vec2 playerPos, Vec2 clickPos) {
    // Maybe later, right now all check codes are in GamePlayController
    return true;
}

// TODO Jolene: figure out why setTexture and draw doesn't work
// later switch to sceneNode
void Collectible::setTexture(const std::shared_ptr<cugl::Texture>& value) {
    if (value) {
        _texture = value;
    } else {
        _texture = nullptr;
    }
}

void Collectible::draw(const std::shared_ptr<SpriteBatch>& batch,const std::shared_ptr<cugl::Texture>& value, Rect pos) {
    batch->draw(value, pos);
}
