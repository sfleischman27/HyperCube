//
//  RenderPipeline.h
//  Platformer
//
//  Does all of the graphics!
//
//  Created by Matthew Quinn on 3/5/23.
//

#ifndef RenderPipeline_h
#define RenderPipeline_h
#include <cugl/cugl.h>
#include "GameModel.h"

class RenderPipeline {
public:

	const int width;
	const int height;

//	const std::string vertexShader =
//#include "shaders/vertex.vert"
//	;

//	const std::string fragmentShader =
//#include "shaders/fragment.frag"
//	;

	/** 
	 * Construct the RenderPipeline
	 */

	RenderPipeline(int screenWidth, int screenHeight);

	/**
	 * Draws all this scene to the given SpriteBatch.
	 *
	 * The default implementation of this method simply draws the scene graph
	 * to the sprite batch.  By overriding it, you can do custom drawing
	 * in its place.
	 *
	 * @param batch     The SpriteBatch to draw with.
	 */

	void render(const std::unique_ptr<GameModel>& model);

};


#endif /* RenderPipeline_h */