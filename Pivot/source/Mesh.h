//
//  Mesh.h
//  Platformer
//
//  Wrapper for Mesh object so we can code without the Mesh loading functioning
//
//  Created by Sarah Fleischman on 2/20/23.
//

#ifndef Mesh_h
#define Mesh_h
#include <cugl/cugl.h>
#include <igl/isolines.h>
#include <igl/readOBJ.h>
#include <igl/readPLY.h>

using namespace cugl;

class PivotVertex3 : public cugl::SpriteVertex3 {

public:
    Vec3 normal;
};

/**
 * A class representing a mesh
 */
class PivotMesh: public cugl::Mesh<PivotVertex3> {
    
private:
    /**Eigen representation of vertices*/
    Eigen::MatrixXd Everts;

    /**Eigen representation of face indices*/
    Eigen::MatrixXi Einds;
    
    
#pragma mark Main Functions
public:
    /**
     * Creates an empty mesh object. Recommended that you use MeshFromOBJ() to create Meshes instead
     */
    PivotMesh() {}

public:
    
    /**Static Method To create a pivot mesh object from an .obj file path
    *
    *@param path the filepath to the desired .obj file
    */
    static std::shared_ptr<PivotMesh> MeshFromOBJ(std::string path);


    /**Slice the mesh with a plane
    *
    * @param origin the origin of the plane
    * @param normal the normal of the plane
    *
    * @note the plane will be oriented such that the y-axis is coplanar with the up-vector*/

    std::tuple<Eigen::MatrixXd, Eigen::MatrixXi> intersectPlane(Vec3 origin, Vec3 normal);
};


#endif /* Mesh_h */
