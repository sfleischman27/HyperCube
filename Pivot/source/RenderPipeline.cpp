//
//  RenderPipeline.cpp
//  Platformer
//
//  Does all of the graphics!
//
//  Created by Matthew Quinn on 3/5/23.
//

#include "RenderPipeline.h"
#include <cugl/cugl.h>

using namespace cugl;

RenderPipeline::RenderPipeline(int screenWidth, int screenHeight) : width(screenWidth), height(screenHeight) {
	return;
}

void RenderPipeline::render(const std::unique_ptr<GameModel>& model) {
	return;
}