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
    _camera->setFar(5000);
    _camera->setZoom(1);
    _camera->update();

	_shader = Shader::alloc(SHADER(vertexShader), SHADER(fragmentShader));
	_shader->setUniformMat4("uPerspective", _camera->getCombined());

    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex3));
    _vertbuff->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(cugl::SpriteVertex3, position));
    _vertbuff->setupAttribute("aColor", 4, GL_UNSIGNED_BYTE, GL_TRUE,
        offsetof(cugl::SpriteVertex3, color));
	_vertbuff->attach(_shader);

    // OpenGL commands to enable alpha blending (if needed)
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return;
}

void RenderPipeline::sceneSetup(const std::unique_ptr<GameModel>& model) {
    levelId = 1; //TODO: set using model's level

    // Make a triangle
    /*
    float radius = 7 * screenSize.height / 16;
    Vec2 up(0, radius);
    Vec2 center(screenSize.width / 2, 3 * screenSize.height / 7);

    Path2 triang;
    triang.vertices.push_back(center + up);
    up.rotate(-M_PI * 2 / 3);
    triang.vertices.push_back(center + up);
    up.rotate(-M_PI * 2 / 3);
    triang.vertices.push_back(center + up);
    triang.closed = true;

    // Convert it into a mesh
    _mesh.clear();
    SpriteVertex3 vert; // To define individual vertices
    Color4f colors[3] = { Color4f::RED,  Color4f::GREEN, Color4f::BLUE };
    for (int ii = 0; ii < 3; ii++) {
        vert.position = Vec3(triang.vertices[ii].x, triang.vertices[ii].y, 0);
        vert.color = colors[ii].getPacked();  // Converts color to int

        _mesh.vertices.push_back(vert);
        _mesh.indices.push_back(ii);
    }
    */

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
        for (int j = 0; j < 3; j++) {
            _mesh.indices.push_back(ourMesh->cuglfaces[i][j]);
        }
    }

    // Only one triangle, so this is best command
    //glEnable(GL_CULL_FACE);
    _mesh.command = GL_TRIANGLES;

    // Print out mesh indices and vertex data
    for (int i = 0; i < ourMesh->cuglvertices.size(); i += 3) {
        CULog("%f, %f, %f", _mesh.vertices[i].position.x, _mesh.vertices[i].position.y, _mesh.vertices[i].position.z);
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

    // Compute rotation and position change
    //Vec3 altNorm(model->getPlaneNorm().x, model->getPlaneNorm().z, model->getPlaneNorm().y); // must rotate
    Vec3 altNorm(model->getPlaneNorm().y, model->getPlaneNorm().z, -model->getPlaneNorm().x);
    const float r = 0.0f;
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
    _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 0);
	
	return;
}