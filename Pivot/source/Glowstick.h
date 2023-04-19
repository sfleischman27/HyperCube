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
        setColor(Vec3(1.0, 1.0, 0.0));
        setIntense(0.15);
    }
};

#endif /* Glowstick_h */

