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

RenderPipeline::RenderPipeline(int screenWidth, const Size& displaySize) : screenSize(displaySize * (screenWidth / displaySize.width)) {

    // Setup current level id
    levelId = 0;

	_camera = OrthographicCamera::alloc(screenSize);
    _camera->setPosition(_camera->getPosition() + Vec3(0, 0, 0)); //depth of camera
    _camera->setFar(1000);
    _camera->setZoom(1);
    _camera->update();

    // Mesh shader
	_shader = Shader::alloc(SHADER(vertexShader), SHADER(fragmentShader));
	_shader->setUniformMat4("uPerspective", _camera->getCombined());

    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex3));
    _vertbuff->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(cugl::SpriteVertex3, position));
    _vertbuff->setupAttribute("aColor", 4, GL_UNSIGNED_BYTE, GL_TRUE,
        offsetof(cugl::SpriteVertex3, color));
	_vertbuff->attach(_shader);

    // Billboard shader
    _shaderBill = Shader::alloc(SHADER(vertexShader), SHADER(billboardShader));
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

	return;
}

void RenderPipeline::sceneSetup(const std::unique_ptr<GameModel>& model) {
    levelId = 1; //TODO: set using model's level
    fbo.init(screenSize.width, screenSize.height);

    _mesh.clear();
    SpriteVertex3 tempV;
    Color4f color = { Color4f::BLACK };
    auto ourMesh = model->getLevel()->getMesh();
    for (int i = 0; i < ourMesh->cuglvertices.size(); i++) {
        tempV.position = ourMesh->cuglvertices[i] * 500;
        tempV.color = color.getPacked();
        _mesh.vertices.push_back(tempV);
    }

    for (int i = 0; i < ourMesh->cuglfaces.size(); i += 1) {
        // first triangle 0 1 2
        for (int j = 0; j < 3; j++) {
            _mesh.indices.push_back(ourMesh->cuglfaces[i][j]);
        }
        // second triangle as .obj does quads 0 2 3
        for (int j = 1; j < 4; j++) {
            if (j == 1) {
                _mesh.indices.push_back(ourMesh->cuglfaces[i][0]);
            }
            else {
                _mesh.indices.push_back(ourMesh->cuglfaces[i][j]);
            }
        }
    }

    // Only one triangle, so this is best command
    //glEnable(GL_CULL_FACE);
    _mesh.command = GL_TRIANGLES;

    // Print out mesh indices and vertex data
    for (int i = 0; i < ourMesh->cuglvertices.size(); i += 3) {
        //CULog("%f, %f, %f", _mesh.vertices[i].position.x, _mesh.vertices[i].position.y, _mesh.vertices[i].position.z);
    }
    for (int i = 0; i < ourMesh->cuglfaces.size(); i += 3) {
        //CULog("%i, %i, %i", _mesh.indices[i], _mesh.indices[i+1], _mesh.indices[i+2]);
    }

    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
}

void RenderPipeline::render(const std::unique_ptr<GameModel>& model) {

    // If new level, reload vertex and index data
    if (levelId == 0) {
        sceneSetup(model);
    }
    _shader->bind();
    //fbo.begin();

    // Compute rotation and position change
    Vec3 altNorm(model->getPlaneNorm().y, model->getPlaneNorm().z, -model->getPlaneNorm().x);
    const float r = 0.001f;
    Vec3 charPos = model->_player->getPosition() * 32;
    charPos -= Vec3(screenSize / 2, 0);
    //CULog("%f, %f, %f", charPos.x, charPos.y, charPos.z);
    Vec3 newPos = charPos + (r * altNorm);

    _camera->setPosition(newPos);
    _camera->setDirection(-altNorm);
    _camera->update();
    _shader->setUniformMat4("uPerspective", _camera->getCombined());
    _shader->setUniformMat4("Mv", _camera->getView());

    // Since we only have one shader and one vertex buffer
    // we never need to bind or unbind either of these
    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 0);

    //fbo.end();
    _shader->unbind();

    _shaderBill->bind();

    // construct mesh for the player
    _meshBill.clear();
    Vec3 basisUp = Vec3(0, 1, 0);
    Vec3 basisRight = altNorm.cross(basisUp);
    SpriteVertex3 tempV;
    for (int i = -9; i <= 9; i += 18) {
        for (int j = -9; j <= 9; j += 18) {
            // not a projection to the plane. to do this, find distance from character position in 3D and the plane.
            // set this as the z-coordinate
            tempV.position = charPos + i * basisRight + j * basisUp; // replace "charPos" with an arbitrary Vec3 to demo real billboarding
            tempV.color = Color4f::GREEN.getPacked();
            _meshBill.vertices.push_back(tempV);
        }
    }

    _shaderBill->setUniformMat4("uPerspective", _camera->getCombined());
    _shaderBill->setUniformMat4("Mv", _camera->getView());
    
    _meshBill.indices.push_back(0);
    _meshBill.indices.push_back(1);
    _meshBill.indices.push_back(2);
    _meshBill.indices.push_back(1);
    _meshBill.indices.push_back(2);
    _meshBill.indices.push_back(3);

    _vertbuffBill->loadVertexData(_meshBill.vertices.data(), (int)_meshBill.vertices.size());
    _vertbuffBill->loadIndexData(_meshBill.indices.data(), (int)_meshBill.indices.size());

    _vertbuffBill->draw(GL_TRIANGLES, (int)_meshBill.indices.size(), 0);
    _shaderBill->unbind();

	return;
}