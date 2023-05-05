//
//  PlaneController.h
//  Platformer
//
//  Controls the plane movement and updates the game model as needed
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef PlaneController_h
#define PlaneController_h
#include <cugl/cugl.h>
#include "GameModel.h"

/**
 * Have functions that take in the GameModel and do the following:
 *  - Update the plane info to rotate plane a specified amount of degrees then update the cut info to reflect the new plane
 */

class PlaneController {

	std::shared_ptr<GameModel> _model;

public:
	/**Constructor for an empty Plane Controller Object, must call init to allocate properties
	* */
	PlaneController() {}

	/**set up the properties of the plane controller
	* @param gamemodel the game model which is begin manipulated by this controller
	*/
	void init(std::shared_ptr<GameModel> gamemodel) 
	{
		_model = gamemodel;
		setPlane(_model->getInitPlayerLoc(), _model->getInitPlaneNorm());
		//calculateCut();
	}

    /**
    *  Sets the normal of the plane and recomputes the CUT
    *
    *  @param norm        The norm of the plane
    */
	void setPlane(Vec3 origin, Vec3 norm);

    /**Rotate the normal around the player by some angle in radians*/
	void rotateNorm(float radians);

	/**Move the planes origin to the location of the player*/
	void movePlaneToPlayer();

	/**Gets a vector of Poly2 objects which represent a CUT throught the map at the given angle
	* @param origin the origin of the cut plane
	* @param normal the normal vector of the cut plane
	*/
	void calculateCut();

	/**Debugging with crazy cuts is hard, so this sets the cut to be a box with given size
	* 
	* @param float size the length of the edge of the square
	*/
	void debugCut(float size);

	/**Get the right basis vector in 3d space
	*this is useful for moving the players 3d coordinates based on some motion in the 2d world
	*/
	Vec3 getBasisRight();

	/**Get the global rotation of the plane relative to world space vector (1,0,0) in degrees
	*/
//	float getGlobalAngleDeg();
};
#endif /* PlaneController_h */
