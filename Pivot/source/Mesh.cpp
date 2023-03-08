//
//  Mesh.cpp
//	Platformer
// 
//  Contains Methods for manipulating custom PivotMesh objects
//
//  Created by Jack on 3/5/23.
//

#include "mesh.h"
#include <cugl/cugl.h>
#include <igl/per_vertex_normals.h>

/**Static Method To create a pivot mesh object from an .obj file path*/
std::shared_ptr<PivotMesh> PivotMesh::MeshFromOBJ(std::string path, float scale) {
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

        auto N = Eigen::MatrixXd();
        igl::per_vertex_normals(V, F, N);

        

        // convert everything into cugl types
        auto cuv = std::vector<Vec3>();
        for (int i = 0; i < V.rows(); i++) {
            cuv.push_back(Vec3(V(i, 0), V(i, 1), V(i, 2))*scale);
        }
        mesh->_cuglvertices = cuv;

        auto cuf = std::vector<std::vector<int>>();
        for (int i = 0; i < F.rows(); i++) {
            cuf.push_back(std::vector<int>{ F(i, 0), F(i, 1), F(i, 2)});
        }
        mesh->_cuglfaces = cuf;

        auto cun = std::vector<Vec3>();
        for (int i = 0; i < N.rows(); i++) {
            cun.push_back(Vec3(V(i, 0), V(i, 1), V(i, 2)));
        }
        mesh->_cuglnormals = cuf;

        CULog("Mesh was loaded successfully");
    }
    else { CULog("THE MESH WAS NOT LOADED PROPERLY"); }

    return mesh;

}