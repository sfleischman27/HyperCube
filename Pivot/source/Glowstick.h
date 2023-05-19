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
private:
    int rand = 1;
    
public:
    /**
     * Creates the glowstick.
     *
     * @param pos          The position of the glowstick
     */
    Glowstick(const Vec3 pos) : GameItem(pos) {
        std::shared_ptr<cugl::Texture> tex =Texture::allocWithFile("textures/glowstick" + std::to_string(rand) + ".png");
        std::shared_ptr<cugl::SpriteSheet> sprite = SpriteSheet::alloc(tex, 6, 6);
        
        rotateSpriteSheet = sprite;
        
        setColor(Vec3(0.0, 1.0, 0.2));
        setIntense(0.35);
        setIsemit(true);
        
        if(rand == 4){ rand = 1; }
        else { rand += 1; }
    }
    Glowstick() :  GameItem() {}
};

#endif /* Glowstick_h */

