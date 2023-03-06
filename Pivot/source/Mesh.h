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

/**
 * A class representing a mesh
 */
class PivotMesh{
    
#pragma mark State
private:
    /** vertices of the mesh*/
    Eigen::MatrixXd _vertices;

    /** index face lists of the mesh */
    Eigen::MatrixXi _faces;
    
public:
    /** A public accessible, read-only version list of vertices */
    const Eigen::MatrixXd& vertices;

    /** A public accessible, read-only version list of face indices */
    const Eigen::MatrixXi& faces;
    
#pragma mark Main Functions
public:
    /**
     * Creates an empty mesh object. Recommended that you use MeshFromOBJ() to create Meshes instead
     */
    PivotMesh(): vertices(_vertices), faces(_faces) {
    }

public:
    
    /**Static Method To create a pivot mesh object from an .obj file path*/
    static std::shared_ptr<PivotMesh> MeshFromOBJ(std::string path);
    
};


#endif /* Mesh_h */
