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
#include "Mesh.h"

class RenderPipeline {
public:

	const Size screenSize;
	const std::string meshVert =
#include "shaders/mesh.vert"
	;
	const std::string meshFrag =
#include "shaders/mesh.frag"
	;
	const std::string billboardVert =
#include "shaders/billboard.vert"
	;
	const std::string billboardFrag =
#include "shaders/billboard.frag"
	;
	const std::string fsqVert =
#include "shaders/fsq.vert"
	;
	const std::string fsqFrag =
#include "shaders/fsq.frag"
	;

	int levelId;

	std::shared_ptr<AssetManager> assets;
	std::shared_ptr<cugl::OrthographicCamera> _camera;
	std::shared_ptr<cugl::Shader> _shader;
	std::shared_ptr<cugl::Shader> _shaderBill;
	std::shared_ptr<cugl::Shader> _shaderFsq;
	std::shared_ptr<cugl::VertexBuffer> _vertbuff;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffBill;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffFsq;
	cugl::Mesh<PivotVertex3> _mesh;
	cugl::Mesh<PivotVertex3> _meshBill;
	cugl::Mesh<PivotVertex3> _meshFsq;
	cugl::RenderTarget fbo;
	std::vector<std::shared_ptr<Texture>> backgrounds;

	/**
	 * Construct the RenderPipeline
	 */
	RenderPipeline(int screenWidth, const Size& displaySize, const std::shared_ptr<AssetManager>& assets);

	/**
	 * Sets up the scene
	 */
	void sceneSetup(const std::shared_ptr<GameModel>& model);
	
	/**
	 * Renders a given gamemodel
	 */
	void render(const std::shared_ptr<GameModel>& model);
};

#endif /* RenderPipeline_h */
