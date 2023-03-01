//
//  Collectible.cpp
//  Platformer
//
//  Created by Jolene Mei on 2/28/23.
//

#include "Collectible.h"

using namespace cugl;
using namespace std;

#pragma mark Collectible
/**
 *  Determines if the collectibles can be seen in current cut
 *  if the plane norm is within +/- 10 degrees of the
 *  perpendicular norm of the collectible position
 *
 *  @param planeNorm    The current plane norm vector
 */
bool Collectible::canBeSeen(Vec3 planeNorm) {
    //TODO: How to decide if the plane go through the collectibe
//    Vec2 collectible = Vec2(_position.x, _position.y);
//    float angle = collectible.getAngle(Vec2(planeNorm.x, planeNorm.y)) * 180/M_PI;
//    if (angle < 0) {
//        angle += 360;
//    }
//    cout << "angle is equal to " << angle;
//    if ((angle >= 80 and angle <= 100) or (angle >= 260 and angle <= 280)) {
//        return true;
//    }
//    else {
//        return false;
//    }
    return true;
}
/**
 *  Determines if the collectibles can be collected by the player
 *  if the player position is within the collecting-enabled distance
 *  @param playerPos    The current player position
 */
bool Collectible::canBeCollected(Vec3 playerPos) {
    //TODO: check if within certain distance to pick up
    return true;
}

void Collectible::setTexture(const std::shared_ptr<cugl::Texture>& value) {
    if (value) {
        _texture = value;
    } else {
        _texture = nullptr;
    }
}

void Collectible::draw(const std::shared_ptr<SpriteBatch>& batch) {
    if (_texture) {
        batch->draw(_texture, Vec2(_position.x, _position.y));
    }
}
