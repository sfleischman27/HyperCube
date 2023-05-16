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

    // For cut texture transform
    storePlayerPos = Vec2(0, 0);
    prevPlayerPos = Vec2(0, 0);

    // FBO setup
    fbo = std::make_shared<RenderTarget>();
    fbofinal = std::make_shared<RenderTarget>();
    fbopos = std::make_shared<RenderTarget>();
    fbo->init(screenSize.width, screenSize.height, { cugl::Texture::PixelFormat::RGBA, cugl::Texture::PixelFormat::RGBA, cugl::Texture::PixelFormat::RGBA, cugl::Texture::PixelFormat::RGBA});
    fbo->setClearColor(Color4f::WHITE);
    fbofinal->init(screenSize.width, screenSize.height, { cugl::Texture::PixelFormat::RGBA16F });
    fbofinal->setClearColor(Color4f::BLACK);
    fbopos->init(screenSize.width, screenSize.height, {cugl::Texture::PixelFormat::RGBA16F});
    fbopos->setClearColor(Color4f::WHITE);

    // Camera setup
	_camera = OrthographicCamera::alloc(screenSize);
    _camera->setFar(farPlaneDist);
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

    // Set up voronoi texture
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
    _vertbuffBill->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffBill->attach(_shaderBill);

    // Position shader
    _shaderPosition = Shader::alloc(SHADER(positionVert), SHADER(positionFrag));
    _vertbuffPosition = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffPosition->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffPosition->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffPosition->attach(_shaderPosition);

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

    // Cut shader
    _shaderCut = Shader::alloc(SHADER(cutVert), SHADER(cutFrag));
    _vertbuffCut = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffCut->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffCut->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffCut->attach(_shaderCut);

    // Behind shader
    _shaderBehind = Shader::alloc(SHADER(behindVert), SHADER(behindFrag));
    _vertbuffBehind = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffBehind->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffBehind->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffBehind->attach(_shaderBehind);

    // Screen shader
    _shaderScreen = Shader::alloc(SHADER(screenVert), SHADER(screenFrag));
    _vertbuffScreen = VertexBuffer::alloc(sizeof(PivotVertex3));
    _vertbuffScreen->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, position));
    _vertbuffScreen->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(PivotVertex3, texcoord));
    _vertbuffScreen->attach(_shaderScreen);
}

void RenderPipeline::sceneSetup(const std::shared_ptr<GameModel>& model) {

    // Get mesh
    _mesh = *model->getRenderMesh();

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
    std::shared_ptr<Texture> charSheet = model->_player->currentSpriteSheet->getTexture();
    drawables.push_back(DrawObject(model->getPlayer3DLoc(), charSheet, model->_player->currentNormalSpriteSheet->getTexture(), true));
    drawables.push_back(DrawObject(model->_exit->getPosition(), model->_exit->getTexture(), NULL, false, 0, true));

    // Collectibles
    std::map<std::string, Collectible> colls = model->getCollectibles();
    for (std::pair<std::string, Collectible> c : colls) {
        if (!c.second.getCollected()) {
            drawables.push_back(DrawObject(c.second.getPosition(), c.second.getTexture(), NULL, false));
        }
    }

    // Glowsticks
    for (Glowstick g : model->_glowsticks) {
        drawables.push_back(DrawObject(g.getPosition(), model->_glowsticks[0].getTexture(), model->_glowsticks[0].getNorm(), false, 1));
    }

    // Decorations
    auto decor = model->getDecorations();
     for (auto d : decor) {
         drawables.push_back(DrawObject(d->getPosition(), d->getTexture(), NULL, false));
     }

    // Set bind points
    const int bindStart = 9;
    for (int i = 0; i < drawables.size(); i++) {
        drawables[i].tex->setBindPoint(bindStart + (2*i));
        if (drawables[i].normalMap != NULL) {
            drawables[i].normalMap->setBindPoint(bindStart + (2*i) + 1);
        }
    }

    // Set up mesh indices
    _meshBill.indices.clear();
    for (int tri = 0; tri <= 1; tri++) {
        for (int i = 0; i < 3; i++) {
            _meshBill.indices.push_back(tri + i);
        }
    }
}

void RenderPipeline::constructBillMesh(const std::shared_ptr<GameModel>& model, const RenderPipeline::DrawObject& dro) {

    _meshBill.vertices.clear();
    Size sz = dro.tex->getSize();
    int div = 4;
    PivotVertex3 tempV;
    for (float i = -sz.width / (2 * div); i <= sz.width / (2 * div); i += sz.width / div) {
        for (float j = -sz.height / (2 * div); j <= sz.height / (2 * div); j += sz.height / div) {
            Vec3 addOn = i * basisRight + j * basisUp;
            tempV.texcoord = Vec2(i > 0 ? 1 : 0, j > 0 ? 0 : 1);
            if (dro.isPlayer) {
                // assuming the spritesheet has square dimensions
                addOn /= model->_player->currentSpriteSheet->getDimen().first;
                tempV.texcoord.x += model->_player->currentSpriteSheet->getFrameCoords().first - 1;
                tempV.texcoord.y += model->_player->currentSpriteSheet->getFrameCoords().second - 1;
                tempV.texcoord.x /= model->_player->currentSpriteSheet->getDimen().first;
                tempV.texcoord.y /= model->_player->currentSpriteSheet->getDimen().second;
            }
            tempV.position = dro.pos + addOn;
            _meshBill.vertices.push_back(tempV);
        }
    }
}

void RenderPipeline::render(const std::shared_ptr<GameModel>& model) {

    // Update camera
    Vec3 n = model->getPlaneNorm();
    const Vec3 charPos = model->getPlayer3DLoc();
    const Vec3 camPos = charPos + (epsilon * n);
    _camera->setPosition(camPos);
    _camera->setDirection(-n);
    Vec3 toPrint = _camera->getDirection();
    //CULog("%f, %f, %f", toPrint.x, toPrint.y, toPrint.z);
    _camera->setUp(Vec3(0, 0, 1));
    _camera->update();
    basisUp = _camera->getUp();
    basisRight = model->getPlaneNorm().cross(basisUp);

    // Setup billboards
    billboardSetup(model);

    // --------------- Pass 0: Textures --------------- //
    // Calculate voronoi angle
    const int numImages = backgrounds.size();
    const int repeat = 3;
    const int repeatAngle = numImages * repeat;
    const float degToRad = 0.0174533;
    Vec3 vInit = Vec3(-1, 0, 0);
    float ang = model->getPlaneNorm().getAngle(vInit);
    if (ang < 0) ang += M_PI;

    // Get cut texture frame
    int degreeAng = int(ang / degToRad);
    int localAng = degreeAng % repeatAngle;
    int index = localAng / repeat;
    earthTex = backgrounds[index];

    // Set bind points
    cobbleTex->setBindPoint(0);
    earthTex->setBindPoint(1);
    fbo->getTexture(fboAlbedo)->setBindPoint(2);
    fbo->getTexture(fboReplace)->setBindPoint(3);
    fbo->getTexture(fboNormal)->setBindPoint(4);
    fbo->getTexture(fboDepth)->setBindPoint(5);
    fbofinal->getTexture()->setBindPoint(6);
    fbopos->getTexture(0)->setBindPoint(7);
    model->backgroundPic->setBindPoint(8);

    // Cut texture translation
    if (model->_justFinishRotating) {
        storePlayerPos += prevPlayerPos;
    }
    prevPlayerPos = model->_player->getPosition();
    Vec2 transOffset = storePlayerPos + model->_player->getPosition();
    transOffset.x /= (screenSize.width / 2);
    transOffset.y /= (screenSize.height / 2);

    // --------------- Pass 1: Mesh --------------- //
    // OpenGL Blending
    glBlendFunc(GL_ONE, GL_ZERO);

    // Binding
    _vertbuff->bind();
    fbo->begin();
    cobbleTex->bind();

    // Set uniforms and draw
    _shader->setUniformMat4("uPerspective", _camera->getCombined());
    _shader->setUniform1i("uTexture", cobbleTex->getBindPoint());
    _shader->setUniformVec3("uDirection", n);
    _shader->setUniform1f("farPlaneDist", farPlaneDist);
    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuff->draw(GL_TRIANGLES, (int)_mesh.indices.size(), 0);

    // Unbinding
    cobbleTex->unbind();
    _vertbuff->unbind();

    // --------------- Pass 2: Billboard --------------- //

    // Binding
    _vertbuffBill->bind();

    for (DrawObject dro : drawables) {
        // Construct vertices to be placed in the mesh
        constructBillMesh(model, dro);

        // Set uniforms and draw individual billboard
        dro.tex->bind();
        if (dro.normalMap != NULL) dro.normalMap->bind();
        _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
        _shaderBill->setUniform1i("flipXvert", dro.isPlayer && !model->_player->isFacingRight() ? 1 : 0);
        _shaderBill->setUniform1f("farPlaneDist", farPlaneDist);
        _shaderBill->setUniform1i("billTex", dro.tex->getBindPoint());
        _shaderBill->setUniform1i("flipXfrag", dro.isPlayer && !model->_player->isFacingRight() ? 1 : 0);
        _shaderBill->setUniformVec3("uDirection", n);
        _shaderBill->setUniformVec3("campos", _camera->getPosition());
        _shaderBill->setUniform1i("useNormTex", 0);
        _shaderBill->setUniform1i("id", dro.id);
        _shaderBill->setUniform1i("doLighting", dro.emission ? 0 : 1);
        if (dro.normalMap != NULL) {
            _shaderBill->setUniform1i("normTex", dro.normalMap->getBindPoint());
            _shaderBill->setUniform1i("useNormTex", 1);
        }
        _vertbuffBill->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
        _vertbuffBill->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
        _vertbuffBill->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);
        if (dro.normalMap != NULL) dro.normalMap->unbind();
        dro.tex->unbind();
    }

    // Unbinding
    fbo->end();
    _vertbuffBill->unbind();

    // --------------- Pass 3: Position --------------- //
    // OpenGL Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Binding
    _vertbuffPosition->bind();
    fbopos->begin();

    // Set uniforms and draw mesh
    _shaderPosition->setUniform1i("isBillboard", 0);
    _shaderPosition->setUniformMat4("uPerspective", _camera->getCombined());
    _shaderPosition->setUniform1i("flipXvert", 0);
    _vertbuffPosition->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuffPosition->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuffPosition->draw(GL_TRIANGLES, (int)_mesh.indices.size(), 0);

    for (DrawObject dro : drawables) {
        if (dro.emission) continue;
        // Construct vertices to be placed in the mesh
        constructBillMesh(model, dro);

        // Set uniforms and draw individual billboard
        dro.tex->bind();
        _shaderPosition->setUniform1i("isBillboard", 1);
        _shaderPosition->setUniformMat4("uPerspective", _camera->getCombined());
        _shaderPosition->setUniform1i("billTex", dro.tex->getBindPoint());
        _shaderPosition->setUniform1i("flipXvert", dro.isPlayer && !model->_player->isFacingRight() ? 1 : 0);
        _vertbuffPosition->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
        _vertbuffPosition->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
        _vertbuffPosition->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);
        dro.tex->unbind();
    }

    // Unbinding
    fbopos->end();
    _vertbuffPosition->unbind();

    // --------------- Pass 4: Pointlights --------------- //
    // OpenGL Blending
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);

    // Binding
    fbofinal->begin();
    _vertbuffPointlight->bind();
    fbo->getTexture(fboAlbedo)->bind();
    fbo->getTexture(fboReplace)->bind();
    fbo->getTexture(fboNormal)->bind();
    fbopos->getTexture()->bind();

    // Set uniforms and draw
    const float numLights = model->_lights.size() + model->_lightsFromItems.size();
    _shaderPointlight->setUniform1f("numLights", numLights);
    _shaderPointlight->setUniform1i("albedoTexture", fbo->getTexture(fboAlbedo)->getBindPoint());
    _shaderPointlight->setUniform1i("replaceTexture", fbo->getTexture(fboReplace)->getBindPoint());
    _shaderPointlight->setUniform1i("normalTexture", fbo->getTexture(fboNormal)->getBindPoint());
    _shaderPointlight->setUniform1i("posTexture", fbopos->getTexture()->getBindPoint());
    _shaderPointlight->setUniform3f("vpos", _camera->getPosition().x, _camera->getPosition().y, _camera->getPosition().z); // for specular only
    _shaderPointlight->setUniformMat4("Mv", _camera->getView()); // for specular only
    for (GameModel::Light &l : model->_lights) {
        _shaderPointlight->setUniform3f("color", l.color.x, l.color.y, l.color.z);
        _shaderPointlight->setUniform3f("lpos", l.loc.x, l.loc.y, l.loc.z);
        _shaderPointlight->setUniform1f("power", l.intensity);
        _vertbuffPointlight->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
        _vertbuffPointlight->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
        _vertbuffPointlight->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);
    }
    for (auto p : model->_lightsFromItems) {
        GameModel::Light &l = p.second;
        _shaderPointlight->setUniform3f("color", l.color.x, l.color.y, l.color.z);
        _shaderPointlight->setUniform3f("lpos", l.loc.x, l.loc.y, l.loc.z);
        _shaderPointlight->setUniform1f("power", l.intensity);
        _vertbuffPointlight->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
        _vertbuffPointlight->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
        _vertbuffPointlight->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);
    }

    // Unbinding
    fbopos->getTexture()->unbind();
    fbo->getTexture(fboAlbedo)->unbind();
    fbo->getTexture(fboReplace)->unbind();
    fbo->getTexture(fboNormal)->unbind();
    _vertbuffPointlight->unbind();

    // --------------- Pass 5: Fog --------------- //
    // OpenGL Blending
    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

    // Binding
    _vertbuffFog->bind();
    fbo->getTexture(fboReplace)->bind();
    fbo->getTexture(fboDepth)->bind();

    // Set uniforms and draw
    _shaderFog->setUniform1f("farPlaneDist", farPlaneDist);
    _shaderFog->setUniform1i("replaceTexture", fbo->getTexture(fboReplace)->getBindPoint());
    _shaderFog->setUniform1i("depthTexture", fbo->getTexture(fboDepth)->getBindPoint());
    _shaderFog->setUniform3f("fadeColor", model->fadeCol.x, model->fadeCol.y, model->fadeCol.z);
    _vertbuffFog->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffFog->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffFog->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    // Unbinding
    fbo->getTexture(fboReplace)->unbind();
    fbo->getTexture(fboDepth)->unbind();
    _vertbuffFog->unbind();

    // --------------- Pass 6: Stripped Billboards --------------- //

    // OpenGL Blending
    glDisable(GL_DEPTH_TEST);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Binding
    _vertbuffBehind->bind();
    fbo->getTexture(fboReplace)->bind();

    // Stripped Billboards
    for (DrawObject dro : drawables) {
        if (dro.isPlayer) continue;
        // Calculate distance from plane
        float distance = -n.dot(dro.pos - charPos);

        // If 0 < distance <= -c, then we will want to draw
        if (distance >= 0 || distance <= cutoff) continue;
        float alpha = (1.0 - (distance / cutoff)) * .5;

        // Change the drawObject position to be reflected along the plane
        Vec3 oldPos = dro.pos;
        dro.pos = dro.pos + (2 * distance * n);

        // Construct vertices to be placed in the mesh
        constructBillMesh(model, dro);
        dro.pos = oldPos;

        // Set uniforms and draw individual billboard
        dro.tex->bind();
        _shaderBehind->setUniformMat4("uPerspective", _camera->getCombined());
        _shaderBehind->setUniform1i("flipXvert", dro.isPlayer && !model->_player->isFacingRight() ? 1 : 0);
        _shaderBehind->setUniform1i("billTex", dro.tex->getBindPoint());
        _shaderBehind->setUniform1i("replaceTexture", fbo->getTexture(fboReplace)->getBindPoint());
        _shaderBehind->setUniform1f("alpha", alpha);
        _shaderBehind->setUniform1f("darken", 0.2f);
        _vertbuffBehind->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
        _vertbuffBehind->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
        _vertbuffBehind->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);
        dro.tex->unbind();
    }

    // Unbinding
    fbo->getTexture(fboReplace)->unbind();
    _vertbuffBehind->unbind();

    // --------------- Pass 7: Cut --------------- //

    // Binding
    _vertbuffCut->bind();
    fbo->getTexture(fboAlbedo)->bind();
    fbo->getTexture(fboReplace)->bind();
    fbo->getTexture(fboDepth)->bind();
    earthTex->bind();
    model->backgroundPic->bind();

    // Set uniforms and draw
    float angle = _camera->getDirection().angle(_camera->getDirection(), Vec3(0, 1, 0), _camera->getUp());
    if (angle < 0) angle += 2 * M_PI;
    angle /= M_PI;
    float speed = 1.0;
    _shaderCut->setUniform1i("albedoTexture", fbo->getTexture(fboAlbedo)->getBindPoint());
    _shaderCut->setUniform1i("replaceTexture", fbo->getTexture(fboReplace)->getBindPoint());
    _shaderCut->setUniform1i("depthTexture", fbo->getTexture(fboDepth)->getBindPoint());
    _shaderCut->setUniform1i("outsideTexture", earthTex->getBindPoint());
    _shaderCut->setUniformVec2("transOffset", transOffset);
    _shaderCut->setUniformVec2("screenSize", Vec2(screenSize.width, screenSize.height));
    _shaderCut->setUniform1i("background", model->backgroundPic->getBindPoint());
    _shaderCut->setUniform1f("angle", angle * speed);
    _vertbuffCut->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffCut->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffCut->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    // Unbinding
    model->backgroundPic->unbind();
    earthTex->unbind();
    fbo->getTexture(fboAlbedo)->unbind();
    fbo->getTexture(fboReplace)->unbind();
    fbo->getTexture(fboDepth)->unbind();
    _vertbuffCut->unbind();

    fbofinal->end();

    // --------------- Pass 8: Screen --------------- //
    // OpenGL Blending
    glEnable(GL_DEPTH_TEST);

    // Binding
    _vertbuffScreen->bind();
    fbofinal->getTexture()->bind();

    // Set uniforms and draw
    _shaderScreen->setUniform1i("screenTexture", fbofinal->getTexture()->getBindPoint());
    float blackFrac = (model->timeToNormalSinceDeath - model->_currentTime->ellapsedMillis(*model->_deathTime)) / model->timeToNormalSinceDeath;
    blackFrac = std::max(0.0f, blackFrac);
    _shaderScreen->setUniform1f("blackFrac", blackFrac);
    _vertbuffScreen->loadVertexData(_meshFsq.vertices.data(), (int)_meshFsq.vertices.size());
    _vertbuffScreen->loadIndexData(_meshFsq.indices.data(), (int)_meshFsq.indices.size());
    _vertbuffScreen->draw(GL_TRIANGLES, (int)_meshFsq.indices.size(), 0);

    // Unbinding
    fbofinal->getTexture()->unbind();
    _vertbuffScreen->unbind();
}
