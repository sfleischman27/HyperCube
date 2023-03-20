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

RenderPipeline::RenderPipeline(int screenWidth, const Size& displaySize, const std::shared_ptr<AssetManager>& assets) : screenSize(displaySize * (screenWidth / displaySize.width)) {

    // Asset manager
    this->assets = assets;

    // Setup current level id
    levelId = 0;

    // For cut texture transform
    storePlayerPos = Vec2(0, 0);
    prevPlayerPos = Vec2(0, 0);

    // FBO setup
    fbo.init(screenSize.width, screenSize.height);
    fbo.setClearColor(Color4f::MAGENTA);

    // Camera setup
	_camera = OrthographicCamera::alloc(screenSize);
    _camera->setFar(10000);
    _camera->setZoom(2);
    _camera->update();

    // Mesh shader
	_shader = Shader::alloc(SHADER(meshVert), SHADER(meshFrag));
	_shader->setUniformMat4("uPerspective", _camera->getCombined());

    _vertbuff = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuff->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuff->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuff->setupAttribute("aNormal", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, normal));
	_vertbuff->attach(_shader);

    // Billboard shader
    _shaderBill = Shader::alloc(SHADER(billboardVert), SHADER(billboardFrag));
    _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
    
    _vertbuffBill = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffBill->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffBill->setupAttribute("aColor", 4, GL_UNSIGNED_BYTE, GL_TRUE,
        offsetof(PivotVertex3, color));
    _vertbuffBill->attach(_shaderBill);

    // FSQ shader
    _shaderFsq = Shader::alloc(SHADER(fsqVert), SHADER(fsqFrag));

    _vertbuffFsq = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffFsq->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffFsq->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffFsq->attach(_shaderFsq);

    // Raw OpenGL commands
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    //glEnable(GL_CULL_FACE);

    // Set up Voronoi backgrounds
    backgrounds = {};
    for (int i = 1; i <= 15; i++) {
        backgrounds.push_back(assets->get<Texture>("section_cut (" + std::to_string(i) + ")"));
    }
}

void RenderPipeline::sceneSetup(const std::shared_ptr<GameModel>& model) {

    // Hardcode level id
    levelId = 1;

    // Get mesh
    _mesh = *model->getMesh();

    // Add all FSQ vertices
    _meshFsq.clear();
    PivotVertex3 tempV;
    for (int n = 0; n < 2; n++) {
        for (int i = -1; i <= 1; i += 2) {
            for (int j = -1; j <= 1; j += 2) {
                tempV.position = Vec3(i, j, 0.01);
                tempV.texcoord = Vec2(i > 0 ? 1 : 0, j > 0 ? 1 : 0);
                _meshFsq.vertices.push_back(tempV);
            }
        }
    }

    // Add all FSQ indices
    for (int tri = 0; tri <= 1; tri++) {
        for (int i = 0; i < 3; i++) {
            _meshFsq.indices.push_back(tri + i);
        }
    }
}

void RenderPipeline::billboardSetup(const std::shared_ptr<GameModel>& model) {

    billboardOrigins = {}; // should have a Billboard class (a model class already exists, just use that)
    billboardCols = {}; // also this should be done in sceneSetup (kept here for drawing physics cut)

    // Collectibles
    std::unordered_map<std::string, Collectible> colls = model->getCollectibles();
    billboardOrigins.push_back(model->getPlayer3DLoc());
    billboardOrigins.push_back(model->getExitLoc());
    billboardCols.push_back(Color4f::RED);
    billboardCols.push_back(Color4f::BLUE);
    for (std::pair<std::string, Collectible> c : colls) {
        billboardOrigins.push_back(c.second.getPosition());
        billboardCols.push_back(Color4f::GREEN);
    }

    // Construct basis
    const Vec3 basisUp = _camera->getUp();
    const Vec3 basisRight = model->getPlaneNorm().cross(basisUp);

    // Optionally collect the physics object
    if (drawCut) {
        auto cut = model->getCut();
        auto o = model->getPlayer3DLoc();
        for (int i = 0; i < cut.size(); i++) {
            for (int j = 0; j < cut[i].vertices.size(); j++) {
                auto unplane = (cut[i].vertices[j].x * basisRight);
                billboardOrigins.push_back(Vec3(-unplane.x, -unplane.y, cut[i].vertices[j].y) + model->getPlaneOrigin());
                billboardCols.push_back(Color4f::BLUE);
            }
        }
    }

    // Add all billboard vertices
    _meshBill.clear();
    PivotVertex3 tempV;
    for (int n = 0; n < billboardOrigins.size(); n++) {
        for (float i = -5; i <= 5; i += 10) {
            for (float j = -5; j <= 5; j += 10) {
                tempV.position = billboardOrigins[n] + i * basisRight + j * basisUp;
                tempV.color = billboardCols[n].getPacked();
                _meshBill.vertices.push_back(tempV);
            }
        }
    }

    // Add all billboard indices
    for (int c = 0; c < billboardCols.size(); c++) {
        int startIndex = c * 4;
        for (int tri = 0; tri <= 1; tri++) {
            for (int i = 0; i < 3; i++) {
                _meshBill.indices.push_back(startIndex + tri + i);
            }
        }
    }
}

void RenderPipeline::render(const std::shared_ptr<GameModel>& model) {

    // --------------- Pass -1: Setup --------------- //
    // If new level, reload vertex and index data
    if (levelId == 0) {
        sceneSetup(model); // sceneSetup should only be called through gameplayController; this should not be here
    }

    // Update camera
    Vec3 n = model->getPlaneNorm();
    const Vec3 charPos = model->getPlayer3DLoc();
    const Vec3 camPos = charPos + (epsilon * n);
    _camera->setPosition(camPos);
    _camera->setDirection(-n);
    _camera->setUp(Vec3(0, 0, 1));
    _camera->update();

    // Setup billboards
    billboardSetup(model);

    // --------------- Pass 0: Textures --------------- //
    // Calculate voronoi angle
    const int numImages = 15;
    const int repeatAngle = 45;
    const float degToRad = 0.0174533;
    Vec3 vInit = Vec3(-1, 0, 0);
    float ang = model->getPlaneNorm().getAngle(vInit);
    if (ang < 0) ang += M_PI;
    int index = int(fmod(ang, repeatAngle * degToRad) / (degToRad));

    // Get texture objects
    std::shared_ptr<Texture> cobbleTex = assets->get<Texture>("cobble"); // should be passed in through model and not loaded every frame
    std::shared_ptr<Texture> earthTex = backgrounds[index / (repeatAngle / numImages)];

    // Set bind points
    cobbleTex->setBindPoint(insideTex);
    fbo.getTexture()->setBindPoint(fsqTex);
    earthTex->setBindPoint(outsideTex);

    // Outside texture translation
    if (model->_justFinishRotating) {
        storePlayerPos += prevPlayerPos;
    }
    prevPlayerPos = model->_player->getPosition();
    Vec2 transOffset = storePlayerPos + model->_player->getPosition();
    transOffset.x /= (screenSize.width / 2);
    transOffset.y /= (screenSize.height / 2);

    // --------------- Pass 1: Mesh --------------- //
    _vertbuff->bind();
    fbo.begin();
    cobbleTex->bind();

    _shader->setUniformMat4("uPerspective", _camera->getCombined());
    _shader->setUniformVec3("uDirection", n);
    _shader->setUniformMat4("Mv", _camera->getView());
    _shader->setUniform1i("uTexture", insideTex);
    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuff->draw(GL_TRIANGLES, (int)_mesh.indices.size(), 0);

    cobbleTex->unbind();
    _vertbuff->unbind();

    // --------------- Pass 2: Billboard --------------- //
    _vertbuffBill->bind();

    _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
    _shaderBill->setUniformMat4("Mv", _camera->getView());
    _vertbuffBill->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
    _vertbuffBill->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
    _vertbuffBill->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);

    fbo.end();
    _vertbuffBill->unbind();

    // --------------- Pass 3: FSQ --------------- //
    _vertbuffFsq->bind();
    fbo.getTexture()->bind();
    earthTex->bind();

    _shaderFsq->setUniformMat4("uPerspective", _camera->getCombined());
    _shaderFsq->setUniform1i("fsqTexture", fsqTex);
    _shaderFsq->setUniform1i("outsideTexture", outsideTex);
    _shaderFsq->setUniformVec2("transOffset", transOffset);
    _shaderFsq->setUniformVec2("screenSize", Vec2(screenSize.width, screenSize.height));
    _vertbuffFsq->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffFsq->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffFsq->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    earthTex->unbind();
    fbo.getTexture()->unbind();
    _vertbuffFsq->unbind();
}
