//
//  Mesh.cpp
//	Platformer
// 
//  Contains Methods for manipulating custom PivotMesh objects
//
//  Created by Jack on 3/5/23.
//

#include "Mesh.h"
#include <cugl/cugl.h>

/**Static Method To create a pivot mesh object from an .obj file path*/
std::shared_ptr<PivotMesh> PivotMesh::MeshFromOBJ(std::string path) {
    auto mesh = std::make_shared<PivotMesh>();
    auto V = Eigen::MatrixXd();
    auto TC = Eigen::MatrixXd();
    auto CN = Eigen::MatrixXd();
    auto F = Eigen::MatrixXi();
    auto FTC = Eigen::MatrixXi();
    auto FN = Eigen::MatrixXi();
    bool success = igl::readOBJ(path, V, TC, CN, F, FTC, FN); // lots of extra info we could use later
    if (success) {
        mesh->_vertices = V;
        mesh->_faces = F;
        CULog("Mesh was loaded successfully");
    }
    else { CULog("THE MESH WAS NOT LOADED PROPERLY"); }

    return mesh;

}