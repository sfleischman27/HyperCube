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
#include <igl/isolines.h>
#include <igl/slice_sorted.h>
#include <igl/cut_mesh.h>

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


/**Slice the mesh with a plane defined by an origin and normal. The y axis is assumed to be coplanar with the up vector at the provided origin*/
std::vector<std::vector<Vec2>> PivotMesh::slice(Vec3 origin, Vec3 normal) {

    auto V = Eigen::MatrixXd();
    auto E = Eigen::MatrixXi();
    auto pln_eq = Eigen::MatrixXd(_vertices.rows(),1);

    for (int i = 0; i < _vertices.rows(); i++) {
        
        //do a dot product for each vertex to 
        pln_eq(i) =  _vertices(i, 0)* normal.x + _vertices(i, 1) * normal.y + _vertices(i, 2) * normal.z;
    }

    auto nrows = _vertices.rows();
    auto zzrows = pln_eq.rows();

    //get the verts on the contour
    igl::isolines(_vertices, _faces, pln_eq, 3, V, E);


    //print some stuff to read into rhino to figure out what the hell this does
    for (int i = 0; i < V.rows(); i++) {
        CULog("v%f,%f,%f", V(i, 0), V(i, 1), V(i, 2));
    }
    for (int i = 0; i < E.rows(); i++) {
        CULog("e%i,%i", E(i, 0), E(i, 1));
    }


    // Sad sad this makes stupide dashes I give up

    // IDK what this will do
    /*auto C = Eigen::MatrixXi();
    auto I = Eigen::VectorXi();
    igl::cut_mesh(_vertices, _faces,pln_eq, I);*/

    //try slicing instead
    




    // convert eigen stuff to cugl vecs TODO: figure out how to do dot products in eigen for performance
    auto scale = 0.01;
    auto verts = std::vector<std::vector<Vec2>>();

    // take pairs of edge indices, get the corresponding vertices, convert them to screen coords, return 2d screen coord pairs
    for (int i = 0; i < E.rows(); i++) {
        auto p0 = Vec3(V(E(i, 0), 0), V(E(i, 0), 1), V(E(i, 0), 2));
        auto p1 = Vec3(V(E(i, 1), 0), V(E(i, 1), 1), V(E(i, 1), 2));

        auto p0x = p0.dot(cugl::Vec3(0, 0, 1).cross(normal).normalize());
        auto p0y = p0.dot(cugl::Vec3(0, 0, 1));

        auto p1x = p1.dot(cugl::Vec3(0, 0, 1).cross(normal).normalize());
        auto p1y = p1.dot(cugl::Vec3(0, 0, 1));

        verts.push_back(std::vector<Vec2>{ Vec2(p0x, p0y), Vec2(p1x, p1y) });
    }

    return verts;
};