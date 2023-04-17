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

RenderPipeline::RenderPipeline(int screenWidth, const Size& displaySize, const std::shared_ptr<AssetManager>& assets) : screenSize(displaySize* (screenWidth / displaySize.width)) {

    // Asset manager
    this->assets = assets;

    // For cut texture transform
    storePlayerPos = Vec2(0, 0);
    prevPlayerPos = Vec2(0, 0);

    // FBO setup
    fbo = std::make_shared<RenderTarget>();
    fbo2 = std::make_shared<RenderTarget>();
    fbopos = std::make_shared<RenderTarget>();
    fbo->init(screenSize.width, screenSize.height, 4);
    fbo->setClearColor(Color4f::WHITE);
    fbo2->init(screenSize.width, screenSize.height);
    fbo2->setClearColor(Color4f::BLACK);
    fbopos->init(screenSize.width, screenSize.height, {cugl::Texture::PixelFormat::RGBA16F});
    fbopos->setClearColor(Color4f::WHITE);

    // Camera setup
	_camera = OrthographicCamera::alloc(screenSize);
    _camera->setFar(10000);
    _camera->setZoom(2);
    _camera->update();

    // Shader setup
    constructShaders();

    // Raw OpenGL commands
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    //glEnable(GL_CULL_FACE);

    // Set up textures
    cobbleTex = assets->get<Texture>("cobble");
    backgrounds = {};
    for (int i = 1; i <= 14; i++) {
        backgrounds.push_back(assets->get<Texture>("section_cut (" + std::to_string(i) + ")"));
    }
}

void RenderPipeline::constructShaders() {

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
    _vertbuffBill->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffBill->attach(_shaderBill);

    // Cut shader
    _shaderCut = Shader::alloc(SHADER(cutVert), SHADER(cutFrag));

    _vertbuffCut = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffCut->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffCut->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffCut->attach(_shaderCut);

    // Pointlight shader
    _shaderPointlight = Shader::alloc(SHADER(pointlightVert), SHADER(pointlightFrag));

    _vertbuffPointlight = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffPointlight->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffPointlight->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffPointlight->attach(_shaderPointlight);

    // Fog shader
    _shaderFog = Shader::alloc(SHADER(fogVert), SHADER(fogFrag));

    _vertbuffFog = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffFog->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffFog->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffFog->attach(_shaderFog);

    // Screen shader
    _shaderScreen = Shader::alloc(SHADER(screenVert), SHADER(screenFrag));

    _vertbuffScreen = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffScreen->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffScreen->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffScreen->attach(_shaderScreen);

    // Position shader
    _shaderPosition = Shader::alloc(SHADER(positionVert), SHADER(positionFrag));

    _vertbuffPosition = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffPosition->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffPosition->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffPosition->attach(_shaderPosition);
}

void RenderPipeline::sceneSetup(const std::shared_ptr<GameModel>& model) {

    // Get mesh
    _mesh = *model->getMesh();

    // Add all FSQ-like vertices
    _meshFsq.clear();
    PivotVertex3 tempV;
    for (int n = 0; n < 2; n++) {
        for (int i = -1; i <= 1; i += 2) {
            for (int j = -1; j <= 1; j += 2) {
                tempV.position = Vec3(i, j, 0);
                tempV.texcoord = Vec2(i > 0 ? 1 : 0, j > 0 ? 1 : 0);
                _meshFsq.vertices.push_back(tempV);
            }
        }
    }

    // Add all FSQ-like indices
    for (int tri = 0; tri <= 1; tri++) {
        for (int i = 0; i < 3; i++) {
            _meshFsq.indices.push_back(tri + i);
        }
    }
}

void RenderPipeline::billboardSetup(const std::shared_ptr<GameModel>& model) {

    drawables.clear();

    // Player and exit
    drawables.push_back(DrawObject(model->getPlayer3DLoc(), Color4f::RED, assets->get<Texture>("dude"))); //TODO fix texture
    drawables.push_back(DrawObject(model->getExitLoc(), Color4f::BLUE, assets->get<Texture>("bridge"))); //TODO fix texture

    // Collectibles
    std::unordered_map<std::string, Collectible> colls = model->getCollectibles();
    for (std::pair<std::string, Collectible> c : colls) {
        if (!c.second.getCollected()) {
            drawables.push_back(DrawObject(c.second.getPosition(), Color4f::GREEN, c.second.getTexture()));
        }
    }

    // Glowsticks
    std::vector<Glowstick> glows = model->_glowsticks;
    for (Glowstick g : glows) {
        drawables.push_back(DrawObject(g.getPosition(), Color4f::YELLOW, assets->get<Texture>("spinner"))); //TODO fix texture
    }
    
    // Construct basis
    const Vec3 basisUp = _camera->getUp();
    const Vec3 basisRight = model->getPlaneNorm().cross(basisUp);

    // Set bind points
    for (int i = 0; i < drawables.size(); i++) {
        drawables[i].tex->setBindPoint(20 + i);
    }
}

void RenderPipeline::render(const std::shared_ptr<GameModel>& model) {

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
    glBlendFunc(GL_ONE, GL_ZERO);
    // Calculate voronoi angle
    const int numImages = backgrounds.size();
    const int repeat = 3;
    const int repeatAngle = numImages * repeat;
    const float degToRad = 0.0174533;
    Vec3 vInit = Vec3(-1, 0, 0);
    float ang = model->getPlaneNorm().getAngle(vInit);
    if (ang < 0) ang += M_PI;

    // Calculate correct index
    int degreeAng = int(ang / degToRad);
    int localAng = degreeAng % repeatAngle;
    int index = localAng / repeat;

    // Get texture objects
    earthTex = backgrounds[index];

    // Set bind points
    cobbleTex->setBindPoint(0);
    earthTex->setBindPoint(1);
    fbo2->getTexture()->setBindPoint(2);
    fbo->getTexture(0)->setBindPoint(3);
    fbo->getTexture(1)->setBindPoint(4);
    fbo->getTexture(2)->setBindPoint(5);
    fbo->getTexture(3)->setBindPoint(6);
    fbopos->getTexture(0)->setBindPoint(7);

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
    fbo->begin();
    cobbleTex->bind();

    _shader->setUniformMat4("uPerspective", _camera->getCombined());
    _shader->setUniformVec3("uDirection", n);
    _shader->setUniformMat4("Mv", _camera->getView());
    _shader->setUniform1i("uTexture", cobbleTex->getBindPoint());
    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuff->draw(GL_TRIANGLES, (int)_mesh.indices.size(), 0);

    cobbleTex->unbind();
    //fbo->end();
    _vertbuff->unbind();

    // --------------- Pass 2: Billboard --------------- //
    // Set up mesh indices
    for (int tri = 0; tri <= 1; tri++) {
        for (int i = 0; i < 3; i++) {
            _meshBill.indices.push_back(tri + i);
        }
    }

    // Bind buffer and iterate
    _vertbuffBill->bind();
    //fbo->begin();
    PivotVertex3 tempV;
    const Vec3 basisUp = _camera->getUp();
    const Vec3 basisRight = model->getPlaneNorm().cross(basisUp);
    for (DrawObject dro : drawables) {
        // Set up vertices
        _meshBill.vertices.clear();
        Size sz = dro.tex->getSize();
        for (float i = -sz.width / 2; i <= sz.width / 2; i += sz.width) {
            for (float j = -sz.height / 2; j <= sz.height / 2; j += sz.height) {
                tempV.position = dro.pos + i * basisRight + j * basisUp;
                tempV.color = dro.col.getPacked();
                tempV.texcoord = Vec2(i > 0 ? 1 : 0, j > 0 ? 0 : 1);
                _meshBill.vertices.push_back(tempV);
            }
        }

        // Draw
        dro.tex->bind();
        _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
        _shaderBill->setUniformMat4("Mv", _camera->getView());
        _shaderBill->setUniform1i("billTex", dro.tex->getBindPoint());
        _vertbuffBill->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
        _vertbuffBill->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
        _vertbuffBill->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);
        dro.tex->unbind();
    }

    fbo->end();
    _vertbuffBill->unbind();

    // --------------- Pass 2.5: Position --------------- //
    _vertbuffPosition->bind();
    fbopos->begin();

    _shaderPosition->setUniform1i("removeA", 0);
    //_shaderPosition->setUniform1i("billTex", drawables[0].tex->getBindPoint());
    _shaderPosition->setUniformMat4("uPerspective", _camera->getCombined());
    _vertbuffPosition->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuffPosition->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuffPosition->draw(GL_TRIANGLES, (int)_mesh.indices.size(), 0);

    for (DrawObject dro : drawables) {
        // Set up vertices
        _meshBill.vertices.clear();
        Size sz = dro.tex->getSize();
        for (float i = -sz.width / 2; i <= sz.width / 2; i += sz.width) {
            for (float j = -sz.height / 2; j <= sz.height / 2; j += sz.height) {
                tempV.position = dro.pos + i * basisRight + j * basisUp;
                tempV.color = dro.col.getPacked();
                tempV.texcoord = Vec2(i > 0 ? 1 : 0, j > 0 ? 0 : 1);
                _meshBill.vertices.push_back(tempV);
            }
        }

        // Draw
        dro.tex->bind();
        _shaderPosition->setUniform1i("removeA", 1);
        _shaderPosition->setUniform1i("billTex", dro.tex->getBindPoint());
        _shaderPosition->setUniformMat4("uPerspective", _camera->getCombined());
        _vertbuffPosition->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
        _vertbuffPosition->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
        _vertbuffPosition->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);
        dro.tex->unbind();
    }

    fbopos->end();
    _vertbuffPosition->unbind();

    // --------------- Pass 3: Pointlights --------------- //
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    _vertbuffPointlight->bind();
    fbo2->begin();
    fbo->getTexture(0)->bind();
    fbo->getTexture(1)->bind();
    fbo->getTexture(2)->bind();
    fbo->getTexture(3)->bind();
    fbopos->getTexture(0)->bind();

    _shaderPointlight->setUniform1i("cutTexture", fbo->getTexture(0)->getBindPoint());
    _shaderPointlight->setUniform1i("replaceTexture", fbo->getTexture(1)->getBindPoint());
    _shaderPointlight->setUniform1i("normalTexture", fbo->getTexture(2)->getBindPoint());

    _shaderPointlight->setUniform1i("posTexture", fbopos->getTexture(0)->getBindPoint());

    _shaderPointlight->setUniform1i("depthTexture", fbo->getTexture(3)->getBindPoint());
    _shaderPointlight->setUniform3f("vpos", _camera->getPosition().x, _camera->getPosition().y, _camera->getPosition().z);
    for (GameModel::Light &l : model->_lights) {
        _shaderPointlight->setUniform3f("color", l.color.x, l.color.y, l.color.z);
        _shaderPointlight->setUniform3f("lpos", l.loc.x, l.loc.y, l.loc.z);
        _shaderPointlight->setUniform1f("power", l.intensity);
        _vertbuffPointlight->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
        _vertbuffPointlight->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
        _vertbuffPointlight->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);
    }

    fbopos->getTexture(0)->unbind();
    fbo->getTexture(3)->unbind();
    fbo->getTexture(2)->unbind();
    fbo->getTexture(1)->unbind();
    fbo->getTexture(0)->unbind();
    _vertbuffPointlight->unbind();

    // --------------- Pass 4: Fog --------------- //
    _vertbuffFog->bind();
    fbo->getTexture(0)->bind();
    fbo->getTexture(1)->bind();
    fbo->getTexture(3)->bind();

    _shaderFog->setUniform1i("cutTexture", fbo->getTexture(0)->getBindPoint());
    _shaderFog->setUniform1i("normalTexture", fbo->getTexture(2)->getBindPoint());
    _shaderFog->setUniform1i("depthTexture", fbo->getTexture(3)->getBindPoint());
    _vertbuffFog->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffFog->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffFog->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    fbo->getTexture(3)->unbind();
    fbo->getTexture(1)->unbind();
    fbo->getTexture(0)->unbind();
    _vertbuffFog->unbind();

    // --------------- Pass 5: Cut --------------- //
    _vertbuffCut->bind();
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    fbo->getTexture(0)->bind();
    fbo->getTexture(1)->bind();
    earthTex->bind();

    _shaderCut->setUniform1i("cutTexture", fbo->getTexture(0)->getBindPoint());
    _shaderCut->setUniform1i("replaceTexture", fbo->getTexture(1)->getBindPoint());
    _shaderCut->setUniform1i("outsideTexture", earthTex->getBindPoint());
    _shaderCut->setUniformVec2("transOffset", transOffset);
    _shaderCut->setUniformVec2("screenSize", Vec2(screenSize.width, screenSize.height));
    _vertbuffCut->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffCut->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffCut->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    earthTex->unbind();
    fbo->getTexture(1)->unbind();
    fbo->getTexture(0)->unbind();
    fbo2->end();
    _vertbuffCut->unbind();

    // --------------- Pass 6: Screen --------------- //
    _vertbuffScreen->bind();
    fbo2->getTexture()->bind();

    _shaderScreen->setUniform1i("screenTexture", fbo2->getTexture()->getBindPoint());
    _vertbuffScreen->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffScreen->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffScreen->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    fbo2->getTexture()->unbind();
    _vertbuffScreen->unbind();
}
