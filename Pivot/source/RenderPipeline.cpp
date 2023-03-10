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

    // FBO setup
    fbo.init(screenSize.width, screenSize.height);

    // Camera setup
	_camera = OrthographicCamera::alloc(screenSize);
    _camera->setFar(1000);
    _camera->setZoom(1);
    _camera->update();

    // Mesh shader
	_shader = Shader::alloc(SHADER(meshVert), SHADER(meshFrag));
	_shader->setUniformMat4("uPerspective", _camera->getCombined());

    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex3));
    _vertbuff->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(cugl::SpriteVertex3, position));
    _vertbuff->setupAttribute("aColor", 4, GL_UNSIGNED_BYTE, GL_TRUE,
        offsetof(cugl::SpriteVertex3, color));
    _vertbuff->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
        offsetof(cugl::SpriteVertex3, texcoord));
	_vertbuff->attach(_shader);

    // Billboard shader
    
    _shaderBill = Shader::alloc(SHADER(billboardVert), SHADER(billboardFrag));
    _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
    
    _vertbuffBill = VertexBuffer::alloc(sizeof(SpriteVertex3));
    
    _vertbuffBill->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(cugl::SpriteVertex3, position));
    _vertbuffBill->setupAttribute("aColor", 4, GL_UNSIGNED_BYTE, GL_TRUE,
        offsetof(cugl::SpriteVertex3, color));
    _vertbuffBill->attach(_shaderBill);

    // OpenGL commands to enable alpha blending (if needed)
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);

	return;
}

void RenderPipeline::sceneSetup(const std::shared_ptr<GameModel>& model) {
    levelId = 1; //TODO: set using model's level

    // Setup mesh vertices
    const float scale = 500;
    _mesh.clear();
    SpriteVertex3 tempV;
    Color4f color = { Color4f::BLACK };
    auto ourMesh = model->getLevel()->getMesh();
    for (int i = 0; i < ourMesh->cuglvertices.size(); i++) {
        tempV.position = ourMesh->cuglvertices[i] * scale;
        tempV.color = color.getPacked();
        tempV.texcoord = Vec2(0, 0); // TODO fill with ourMesh
        _mesh.vertices.push_back(tempV);
    }

    // Setup mesh indices
    for (int i = 0; i < ourMesh->cuglfaces.size(); i += 1) {
        for (int j = 0; j < 3; j++) {
            // first triangle 0 1 2
            _mesh.indices.push_back(ourMesh->cuglfaces[i][j]);
        }
        for (int j = 1; j < 4; j++) {
            // second triangle as .obj does quads 0 2 3
            if (j == 1) {
                _mesh.indices.push_back(ourMesh->cuglfaces[i][0]);
            } else {
                _mesh.indices.push_back(ourMesh->cuglfaces[i][j]);
            }
        }
    }
    
    _mesh.command = GL_TRIANGLES;
}

void RenderPipeline::render(const std::shared_ptr<GameModel>& model) {

    // If new level, reload vertex and index data
    if (levelId == 0) {
        sceneSetup(model);
    }

    // Compute rotation and position change
    const float epsilon = 0.001f;
    const float box2dToScreen = 32;
    Vec3 altNorm = Vec3(model->getPlaneNorm().y, model->getPlaneNorm().z, -model->getPlaneNorm().x);
    Vec3 charPos = (model->_player->getPosition() * box2dToScreen) - Vec3(screenSize / 2, 0);
    Vec3 newPos = charPos + (epsilon * altNorm);

    // Update camera
    _camera->setPosition(newPos);
    _camera->setDirection(-altNorm);
    _camera->update();

    // --------------- TEMP: Mesh pre-calculations --------------- //
    // Load cobblestone texture
    std::shared_ptr<Texture> cobbleTex = assets->get<Texture>("dude");

    // --------------- Pass 1: Mesh --------------- //
    _vertbuff->bind();
    //fbo.begin();
    cobbleTex->bind();

    _shader->setUniformMat4("uPerspective", _camera->getCombined());
    _shader->setUniformMat4("Mv", _camera->getView());

    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 0);

    cobbleTex->unbind();
    //fbo.end();
    _vertbuff->unbind();

    // --------------- TEMP: Billboard pre-calculations --------------- //

    // hard-coded billboards
    const int numBill = 2;
    Vec3 billboardOrigins[numBill];
    billboardOrigins[0] = charPos;
    billboardOrigins[1] = Vec3(350, 50, 0);
    Color4f billboardCols[numBill];
    billboardCols[0] = Color4f::RED;
    billboardCols[1] = Color4f::GREEN;

    // construct basis
    const Vec3 basisUp = Vec3(0, 1, 0);
    const Vec3 basisRight = altNorm.cross(basisUp);

    // add all billboard vertices
    _meshBill.clear();
    SpriteVertex3 tempV;
    for (int n = 0; n < numBill; n++) {
        for (int i = -9; i <= 9; i += 18) {
            for (int j = -9; j <= 9; j += 18) {
                // TODO: not a projection to the plane. to do this, find distance from character position in 3D and the plane.
                // set this as the z-coordinate
                tempV.position = billboardOrigins[n] + i * basisRight + j * basisUp;
                tempV.color = billboardCols[n].getPacked();
                _meshBill.vertices.push_back(tempV);
            }
        }
    }

    // add all billboard indices
    for (int c = 0; c < numBill; c++) {
        int startIndex = c * 4;
        for (int tri = 0; tri <= 1; tri++) {
            for (int i = 0; i < 3; i++) {
                _meshBill.indices.push_back(startIndex + tri + i);
            }
        }
    }

    // --------------- Pass 2: Billboard --------------- //

    _vertbuffBill->bind();

    _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
    _shaderBill->setUniformMat4("Mv", _camera->getView());

    _vertbuffBill->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
    _vertbuffBill->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());
    _vertbuffBill->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);

    _vertbuffBill->unbind();

	return;
}