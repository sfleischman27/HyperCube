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
	* Renders a given gamemodel
	*/
	void RenderPipeline::render(const std::unique_ptr<GameModel>& model);
};

#endif /* RenderPipeline_h */
