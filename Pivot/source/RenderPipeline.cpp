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

	_camera = OrthographicCamera::alloc(screenSize);
    _camera->setPosition(_camera->getPosition() + Vec3(0, 0, 0)); //depth of camera
    _camera->update();
	_shader = Shader::alloc(SHADER(vertexShader), SHADER(fragmentShader));
    CULog("initPos");
    CULog(_camera->getPosition().toString().c_str());
    CULog("initDir");
    CULog(_camera->getDirection().toString().c_str());
    CULog("initUp");
    CULog(_camera->getUp().toString().c_str());
	_shader->setUniformMat4("uPerspective", _camera->getCombined());
    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex3));
	
    _vertbuff->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE,
        offsetof(cugl::SpriteVertex3, position));
	
    _vertbuff->setupAttribute("aColor", 4, GL_UNSIGNED_BYTE, GL_TRUE,
        offsetof(cugl::SpriteVertex3, color));

	_vertbuff->attach(_shader);

    // Make a triangle
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
    //int depth = -1;
    for (int ii = 0; ii < 3; ii++) {
        vert.position = Vec3(triang.vertices[ii].x, triang.vertices[ii].y, 0);
        // depth++;
        vert.color = colors[ii].getPacked();  // Converts color to int

        _mesh.vertices.push_back(vert);
        _mesh.indices.push_back(ii);
    }

    // Only one triangle, so this is best command
    _mesh.command = GL_TRIANGLES;

    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());

	return;
}

void RenderPipeline::render(const std::unique_ptr<GameModel>& model) {

    Vec3 altNorm(model->getPlaneNorm().y, 0, model->getPlaneNorm().x);
    CULog("\n\n");
    CULog("altnorm");
    CULog(altNorm.toString().c_str());
    CULog("altpos");
    float r = 60.0;
    Vec3 nPos = Vec3(512 + (7 * screenSize.height / 16), 288, 0) - (r * altNorm);
    CULog(nPos.toString().c_str());
    _camera->setFar(5000);
    CULog("camFar");
    CULog("%f", _camera->getFar());

    _camera->setPosition(nPos);
    _camera->setDirection(altNorm);
    _camera->update();
    _shader->setUniformMat4("uPerspective", _camera->getCombined());

    // OpenGL commands to enable alpha blending (if needed)
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Since we only have one shader and one vertex buffer
    // we never need to bind or unbind either of these
    _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 0);
	
	return;
}