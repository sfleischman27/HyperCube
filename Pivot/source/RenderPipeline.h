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
	// An abstraction to draw billboards
	struct DrawObject {
		Vec3 pos;
		std::shared_ptr<cugl::Texture> tex;
		std::shared_ptr<cugl::Texture> normalMap;
		std::shared_ptr<cugl::SpriteSheet> sheet;
		bool isPlayer;
		bool emission;
		bool fade;
		bool isPoster;
		Vec3 posterNormal;

		DrawObject(Vec3 pos, std::shared_ptr<cugl::Texture> tex, std::shared_ptr<cugl::Texture> normalMap, bool isPlayer, std::shared_ptr<cugl::SpriteSheet> sheet, bool fade, bool isPoster = false, Vec3 posterNormal = Vec3(0, 0, 0)) {
			this->pos = pos;
			this->tex = tex;
			this->normalMap = normalMap;
			this->isPlayer = isPlayer;
			this->emission = normalMap == NULL;
			this->sheet = sheet;
			this->fade = fade;
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
	const std::string positionVert =
#include "shaders/position.vert"
	;
	const std::string positionFrag =
#include "shaders/position.frag"
	;
	const std::string pointlightVert =
#include "shaders/pointlight.vert"
	;
	const std::string pointlightFrag =
#include "shaders/pointlight.frag"
	;
	const std::string cutVert =
#include "shaders/cut.vert"
	;
	const std::string cutFrag =
#include "shaders/cut.frag"
	;
	const std::string behindVert =
#include "shaders/behind.vert"
	;
	const std::string behindFrag =
#include "shaders/behind.frag"
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

	// Constants
	const float epsilon = 0.001f;
	const int farPlaneDist = 10000;
	const int fboAlbedo = 0;
	const int fboReplace = 1;
	const int fboNormal = 2;
	const int fboDepth = 3;
	const Size screenSize;
	const float cutoff = -30.0; // negative number, more negative = further behind can be shown

	// Camera
	std::shared_ptr<cugl::OrthographicCamera> _camera;

	// Shaders
	std::shared_ptr<cugl::Shader> _shader;
	std::shared_ptr<cugl::Shader> _shaderBill;
	std::shared_ptr<cugl::Shader> _shaderPosition;
	std::shared_ptr<cugl::Shader> _shaderPointlight;
	std::shared_ptr<cugl::Shader> _shaderCut;
	std::shared_ptr<cugl::Shader> _shaderBehind;
	std::shared_ptr<cugl::Shader> _shaderFog;
	std::shared_ptr<cugl::Shader> _shaderScreen;

	// Buffers
	std::shared_ptr<cugl::VertexBuffer> _vertbuff;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffBill;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffPosition;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffPointlight;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffCut;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffBehind;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffFog;
	std::shared_ptr<cugl::VertexBuffer> _vertbuffScreen;

	// Meshes
	cugl::Mesh<PivotVertex3> _mesh;
	cugl::Mesh<PivotVertex3> _meshBill;
	cugl::Mesh<PivotVertex3> _meshFsq;

	// FBOs
	std::shared_ptr<cugl::RenderTarget> fbo;
	std::shared_ptr<cugl::RenderTarget> fbofinal;
	std::shared_ptr<cugl::RenderTarget> fbopos;

	// Textures
	std::shared_ptr<Texture> cobbleTex;
	std::shared_ptr<Texture> earthTex;
	std::vector<DrawObject> drawables;
	std::vector<std::shared_ptr<Texture>> backgrounds;

	// Replace texture translation variables
	Vec2 prevPlayerPos;
	Vec2 storePlayerPos;

	// Basis for current coordinate system
	Vec3 basisUp;
	Vec3 basisRight;

	/**
	 * Construct the RenderPipeline
	 */
	RenderPipeline(int screenWidth, const Size& displaySize, const std::shared_ptr<AssetManager>& assets);

	/**
	 * Initializes shaders and vertex buffers
	 */
	void constructShaders();

	/**
	 * Sets up the scene
	 */
	void sceneSetup(const std::shared_ptr<GameModel>& model);

	/**
	 * Sets up the billboards
	 */
	void billboardSetup(const std::shared_ptr<GameModel>& model);

	/**
	 * Sets up the mesh to draw one drawable. Returns true if the billboard is visible on-screen (regardless of depth), and false otherwise.
	 */
	bool constructBillMesh(const std::shared_ptr<GameModel>& model, const DrawObject& dro);
	
	/**
	 * Renders a given gamemodel
	 */
	void render(const std::shared_ptr<GameModel>& model);
};

#endif /* RenderPipeline_h */
