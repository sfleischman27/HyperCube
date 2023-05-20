//
//  Glowstick.h
//  Pivot
//
//  Created by Jolene Mei on 3/23/23.
//

#ifndef Glowstick_h
#define Glowstick_h
#include <cugl/cugl.h>
#include "GameItem.h"

using namespace cugl;

class Glowstick : public GameItem{
    
public:
    /**
     * Creates the glowstick.
     *
     * @param pos          The position of the glowstick
     */
    Glowstick(const Vec3 pos) : GameItem(pos) {
        setIsemit(true);
        setIntense(0.8);
    }
    Glowstick() :  GameItem() {}
};

#endif /* Glowstick_h */

