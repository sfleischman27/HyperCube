//
//  Collectible.cpp
//  Platformer
//
//  Created by Jolene Mei on 2/28/23.
//

#include "Collectible.h"

using namespace cugl;
using namespace std;

/** Threshold of the visible distance */
#define VISIBLE_DIST  1

#pragma mark Collectible
/**
 *  Determines if the collectibles can be seen in current cut
 *
 *  @param playerPos    The current player 3D position
 *  @param planeNorm    The current plane norm vector
 */
bool Collectible::canBeSeen(Vec3 playerPos, Vec3 planeNorm) {
    // Maybe later, right now all check codes are in GamePlayController
    float dist = (getPosition()-playerPos).dot(planeNorm);
    if (!getCollected() && dist <= VISIBLE_DIST) {
        return true;
    }
    return false;
}

void Collectible::setTexture(const std::shared_ptr<cugl::Texture>& value) {
    if (value) {
        _texture = value;
    } else {
        _texture = nullptr;
    }
}

