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
void PlaneController::setPlane(Vec3 origin, Vec3 norm) {
	_model->setPlaneOrigin(origin);
    norm = norm.normalize();
    _model->setPlaneNorm(norm);
}

/**Rotate the normal around the player by some angle in radians*/
void PlaneController::rotateNorm(float radians){   
	auto norm = _model->getPlaneNorm();
    Vec3 newNorm = Vec3(
        norm.x * cos(radians) - norm.y * sin(radians),
        norm.x * sin(radians) + norm.y * cos(radians),
        0
    );
    setPlane(_model->getPlaneOrigin(), newNorm);
}

/**Move the planes origin to the location of the player*/
void PlaneController::movePlaneToPlayer() {
	setPlane(_model->getPlayer3DLoc(), _model->getPlaneNorm());
}


void PlaneController::calculateCut() {

	// set the origin and normal
	//auto origin = Vec3(0, 0, 0);
    auto origin = _model->getPlaneOrigin();
	//auto normal = Vec3(-1, 0, 0);
    auto normal = _model->getPlaneNorm();

	// need the plane basis vectors to do plane projection
	auto upvec = Vec3(0,0,1);
	auto rightvec = getBasisRight();

	// how much to extrude the cut
	float width = 1;

	// init the extruder
	std::shared_ptr<SimpleExtruder> extruder = std::make_shared<SimpleExtruder>();

	// init the cut list
	std::vector<Poly2> cut;

	//do the cut
	auto cutdata = _model->getColMesh()->intersectPlane(origin, normal);
	auto Vcut = std::get<0>(cutdata);
	auto Ecut = std::get<1>(cutdata);

	//make origin at the player origin

	//for each edge get the corresponding start and end vertices
	//dot them with the basis vectors to get their plane projection
	for (int i = 0; i < Ecut.rows(); i++) {

		// Dot the first point to the x axis then the y axis;
		auto x0 = rightvec.x * (Vcut(Ecut(i,0), 0)-origin.x) + rightvec.y * (Vcut(Ecut(i, 0), 1)-origin.y) + rightvec.z * (Vcut(Ecut(i, 0), 2)-origin.z);
		auto y0 = upvec.x * (Vcut(Ecut(i, 0), 0)-origin.x) + upvec.y * (Vcut(Ecut(i, 0), 1) - origin.y) + upvec.z * (Vcut(Ecut(i, 0), 2)-origin.z);

		// Dot the second point to the x axis then the y axis;
		auto x1 = rightvec.x * (Vcut(Ecut(i, 1), 0) - origin.x) + rightvec.y * (Vcut(Ecut(i, 1), 1) - origin.y) + rightvec.z * (Vcut(Ecut(i, 1), 2) - origin.z);
		auto y1 = upvec.x * (Vcut(Ecut(i, 1), 0) - origin.x) + upvec.y * (Vcut(Ecut(i, 1), 1) - origin.y) + upvec.z * (Vcut(Ecut(i, 1), 2) - origin.z);

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
    auto path = Path2(verts);
    path.closed = true;
	extruder->set(path);
	extruder->calculate(width);
	cut.push_back(extruder->getPolygon());

	_model->setCut(cut);

}

/**Get the right basis vector in 3d space
	*this is useful for moving the players 3d coordinates based on some motion in the 2d world
	*/
Vec3 PlaneController::getBasisRight(){

	auto z = Vec3(0,0,1);
	z.cross(_model->getPlaneNorm());
	z.normalize();

	auto angle = z.getAngle(_model->getPlaneNorm());

	return z;
}

/**Get the global rotation of the plane relative to world space vector (1,0,0) in degrees*/
float PlaneController::getGlobalAngleDeg() {
	auto normal = _model->getPlaneNorm();
	auto basis = Vec3(1, 0, 0);
	auto dot = normal.dot(basis);     // Dot product between[x1, y1] and [x2, y2]
	auto det = normal.x * basis.y - normal.y * basis.x;      // Determinant
	auto rad = atan2(det, dot);
	return rad * 180 / 3.141592;
}
