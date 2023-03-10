//
// PlaneController.cpp
//
// Contains methods for setting and rotating the plane and generating cuts through the level mesh
//
// Created by Jack Otto 3/9/2023

#include "PlaneController.h"

/**
    *  Sets the normal of the plane and recomputes the CUT
    *
    *  @param norm        The norm of the plane
    */
void PlaneController::setPlaneNorm(Vec3 norm) {
    norm = norm.normalize();
    _model->setPlaneNorm(norm);
	//calculateCut();//too heavy to calculate every frame
}

/**Rotate the normal around the player by some angle in radians*/
void PlaneController::rotateNorm(float radians) {
    auto norm = _model->getPlaneNorm();
    Vec3 newNorm = Vec3(
        norm.x * cos(radians) - norm.y * sin(radians),
        norm.x * sin(radians) + norm.y * cos(radians),
        0
    );
    setPlaneNorm(newNorm);
}


void PlaneController::calculateCut() {

	// set the origin and normal
	auto origin = Vec3(0, 0, 0);
	auto normal = Vec3(-1, 0, 0);

	// need the plane basis vectors to do plane projection
	auto upvec = Vec3(0,0,1);
	auto rightvec = getBasisRight();

	// how much to extrude the cut
	float width = .05;

	// init the extruder
	std::shared_ptr<SimpleExtruder> extruder = std::make_shared<SimpleExtruder>();

	// init the cut list
	std::vector<Poly2> cut;

	//do the cut
	auto cutdata = _model->getMesh()->intersectPlane(origin, normal);
	auto Vcut = std::get<0>(cutdata);
	auto Ecut = std::get<1>(cutdata);

	//for each edge get the corresponding start and end vertices
	//dot them with the basis vectors to get their plane projection
	//create a path2 object from them
	std::vector<Path2> segments;
	for (int i = 0; i < Ecut.rows(); i++) {

		// Dot the first point to the x axis then the y axis;
		auto x0 = rightvec.x * Vcut(Ecut(i,0), 0) + rightvec.y * Vcut(Ecut(i, 0), 1) + rightvec.z * Vcut(Ecut(i, 0), 2);
		auto y0 = upvec.x * Vcut(Ecut(i, 0), 0) + upvec.y * Vcut(Ecut(i, 0), 1) + upvec.z * Vcut(Ecut(i, 0), 2);

		// Dot the second point to the x axis then the y axis;
		auto x1 = rightvec.x * Vcut(Ecut(i, 1), 0) + rightvec.y * Vcut(Ecut(i, 1), 1) + rightvec.z * Vcut(Ecut(i, 1), 2);
		auto y1 = upvec.x * Vcut(Ecut(i, 1), 0) + upvec.y * Vcut(Ecut(i, 1), 1) + upvec.z * Vcut(Ecut(i, 1), 2);

		// Make Vec2 Objects, make a path, add the path to segments
		auto v0 = Vec2(x0, y0);
		auto v1 = Vec2(x1, y1);
		auto verts = std::vector<Vec2>{ v0, v1 };
		extruder->set(Path2(verts));
		extruder->calculate(width);
		cut.push_back(extruder->getPolygon());

		//CULog("v%f,%f,%f", v0.x, v0.y, 0.0f);
		//CULog("v%f,%f,%f", v1.x, v1.y, 0.0f);
	}

	_model->setCut(cut);
	return;
}

/**Debugging with crazy cuts is hard, so this sets the cut to be a box with given size
	*
	* @param float size the length of the edge of the square
	*/
void PlaneController::debugCut(float size) {

	// init the cut list
	std::vector<Poly2> cut;

	auto width = 1;

	// init the extruder
	std::shared_ptr<SimpleExtruder> extruder = std::make_shared<SimpleExtruder>();

	auto verts = std::vector<Vec2>{
		Vec2(-size/2, -size/2),
		Vec2(size / 2, -size / 2),
		Vec2(size / 2, size / 2),
		Vec2(-size / 2, size / 2)
	};
	extruder->set(Path2(verts));
	extruder->calculate(width);
	cut.push_back(extruder->getPolygon());

	_model->setCut(cut);

}

/**Get the right basis vector in 3d space
	*this is useful for moving the players 3d coordinates based on some motion in the 2d world
	*/
Vec3 PlaneController::getBasisRight(){
	auto z = Vec3(0,0,0);
	z.cross(_model->getPlaneNorm());
	return z.normalize();
}
