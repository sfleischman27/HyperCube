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

	const Size screenSize;
	const std::string vertexShader =
#include "shaders/vertex.vert"
	;
	const std::string fragmentShader =
#include "shaders/fragment.frag"
	;

	int levelId;

	std::shared_ptr<cugl::OrthographicCamera> _camera;
	std::shared_ptr<cugl::Shader> _shader;
	std::shared_ptr<cugl::VertexBuffer> _vertbuff;
	cugl::Mesh<cugl::SpriteVertex3> _mesh;

	/**
	 * Construct the RenderPipeline
	 */
	RenderPipeline(int screenWidth, const Size& displaySize);

	/**
	 * Sets up the scene
	 */
	void sceneSetup(const std::unique_ptr<GameModel>& model);
	
	/**
	 * Renders a given gamemodel
	 */
	void render(const std::unique_ptr<GameModel>& model);
};

#endif /* RenderPipeline_h */
