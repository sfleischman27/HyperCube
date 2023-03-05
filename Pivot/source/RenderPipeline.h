//
//  RenderPipeline.h
//  Platformer
//
//  Does all of the graphics!
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef RenderPipeline_h
#define RenderPipeline_h
#include <cugl/cugl.h>
#include "GameModel.h"

class RenderPipeline {
public:

	/**
	 * Draws all this scene to the given SpriteBatch.
	 *
	 * The default implementation of this method simply draws the scene graph
	 * to the sprite batch.  By overriding it, you can do custom drawing
	 * in its place.
	 *
	 * @param batch     The SpriteBatch to draw with.
	 */

	static void render(const std::unique_ptr<GameModel>& model);

};


#endif /* RenderPipeline_h */