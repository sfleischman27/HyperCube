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
private:
	// abstraction to draw billboards
	struct DrawObject {
		Vec3 pos;
		Color4f col;
		std::shared_ptr<cugl::Texture> tex;

		DrawObject(Vec3 pos, Color4f col, std::shared_ptr<cugl::Texture> tex) {
			this->pos = pos;
			this->col = col;
			this->tex = tex;
		}
	};

public:
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
	const std::string cutVert =
#include "shaders/cut.vert"
	;
	const std::string cutFrag =
#include "shaders/cut.frag"
	;
	const std::string pointlightVert =
#include "shaders/pointlight.vert"
	;
	const std::string pointlightFrag =
#include "shaders/pointlight.frag"
	;
	const std::string fogVert =
#include "shaders/fog.vert"
	;
	const std::string fogFrag =
#include "shaders/fog.frag"
	;
	const std::string screenVert =
#include "shaders/screen.vert"
	;
	const std::string screenFrag =
#include "shaders/screen.frag"
	;

	// constants
	const float epsilon = 0.001f;
	const int insideTex = 0;
	const int cutTex = 1;
	const int outsideTex = 2;

	// cugl utilized singletons
	std::shared_ptr<AssetManager> assets;
	std::shared_ptr<cugl::OrthographicCamera> _camera;

	// shaders, buffers, meshes, fbos
	std::shared_ptr<cugl::Shader> _shader;
	std::shared_ptr<cugl::Shader> _shaderBill;
	std::shared_ptr<cugl::Shader> _shaderCut;
	std::shared_ptr<cugl::VertexBuffer> _vertbuff;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffBill;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffCut;
	cugl::Mesh<PivotVertex3> _mesh;
	cugl::Mesh<PivotVertex3> _meshBill;
	cugl::Mesh<PivotVertex3> _meshCut;
	cugl::RenderTarget fbo;

	// textures
	std::shared_ptr<Texture> cobbleTex;
	std::shared_ptr<Texture> earthTex;
	std::vector<DrawObject> drawables;

	// misc variables
	const Size screenSize;
	std::vector<std::shared_ptr<Texture>> backgrounds;
	Vec2 prevPlayerPos;
	Vec2 storePlayerPos;

	/**
	 * Construct the RenderPipeline
	 */
	RenderPipeline(int screenWidth, const Size& displaySize, const std::shared_ptr<AssetManager>& assets);

	/**
	 * Sets up the scene
	 */
	void sceneSetup(const std::shared_ptr<GameModel>& model);

	/**
	 * Sets up the billboards
	 */
	void billboardSetup(const std::shared_ptr<GameModel>& model);
	
	/**
	 * Renders a given gamemodel
	 */
	void render(const std::shared_ptr<GameModel>& model);
};

#endif /* RenderPipeline_h */
