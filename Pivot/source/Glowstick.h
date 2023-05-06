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
        std::shared_ptr<cugl::Texture> tex =Texture::allocWithFile("textures/barrier.png");
        setTexture(tex);
        setColor(Vec3(0.0, 1.0, 0.2));
        setIntense(0.15);
    }
};

#endif /* Glowstick_h */

