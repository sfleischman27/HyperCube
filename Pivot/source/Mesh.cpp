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
#include <igl/isolines.h>

/**Static Method To create a pivot mesh object from an .obj file path*/
std::shared_ptr<PivotMesh> PivotMesh::MeshFromOBJ(std::string path) {

    //make an empty mesh
    auto mesh = std::make_shared<PivotMesh>();

    // init some matrices to hold imported info
    auto V = Eigen::MatrixXd();
    auto TC = Eigen::MatrixXd();
    auto CN = Eigen::MatrixXd();
    auto F = Eigen::MatrixXi();
    auto FTC = Eigen::MatrixXi();
    auto FN = Eigen::MatrixXi();

    //read obj into those matrices
    bool success = igl::readOBJ(path, V, TC, CN, F, FTC, FN);

    if (success) {

        Color4f color = { Color4f::BLACK };

        //calculate vertex normals
        auto N = Eigen::MatrixXd();
        igl::per_vertex_normals(V, F, N);

        // convert everything into cugl friendly types and assign it to the mesh
        for (int i = 0; i < V.rows(); i++) {
            PivotVertex3 temp;
            temp.position = Vec3(V(i, 0), V(i, 1), V(i, 2));
            temp.color = color.getPacked();
            temp.normal = Vec3(N(i, 0), N(i, 1), N(i, 2));
            CULog("%f, %f, %f", temp.normal.x, temp.normal.y, temp.normal.z);
            temp.texcoord = Vec2(TC(i, 0), TC(i, 1));
            // Below 3 lines have been added by Matt to support texture tiling (for now)
            const int numTexOneSide = 14;
            temp.texcoord.x = fmod(temp.texcoord.x * numTexOneSide, 1.0);
            temp.texcoord.y = fmod(temp.texcoord.y * numTexOneSide, 1.0);
            // End what Matt has added
            mesh->vertices.push_back(temp);
        }

        auto test = F.rows();
        for (int i = 0; i < F.rows(); i ++) {
            mesh->indices.push_back(F(i,0));
            mesh->indices.push_back(F(i,1));
            mesh->indices.push_back(F(i,2));
        }


        //Make the vertices list two longer to hold superverts for domain control
        //If this is confusing as Jack about it bc it is confusing
        V.conservativeResize(V.rows() + 2, V.cols()); 
        mesh->Everts = V;
        mesh->Einds = F;

        CULog("Mesh was loaded successfully");

        
    }
    else { CULog("THE MESH WAS NOT LOADED PROPERLY"); }

    return mesh;

}

/**Slice the mesh with a plane
    *
    * @param origin the origin of the plane
    * @param normal the normal of the plane
    *
    * @note the plane will be oriented such that the y-axis is coplanar with the up-vector*/

std::tuple<Eigen::MatrixXd, Eigen::MatrixXi> PivotMesh::intersectPlane(Vec3 origin, Vec3 normal) {

    //add super verts at the end to recenter the cut to the plane
    //if the min and max values are sysmetrical around the plane the isocut will be on the plane
    //this is a genius hack!!!
    auto superneg = normal * 100000 + origin;
    auto superpos = normal * -100000 + origin;

    //Set the last two vertices (which we made extra space for) as these superverts
    Everts.row(Everts.rows() - 2) = Eigen::Vector3d(static_cast<double>(superneg.x), static_cast<double>(superneg.y), static_cast<double>(superneg.z)).transpose();
    Everts.row(Everts.rows() - 1) = Eigen::Vector3d(static_cast<double>(superpos.x), static_cast<double>(superpos.y), static_cast<double>(superpos.z)).transpose();

    // Where to put the cut info
    auto Vcut = Eigen::MatrixXd();
    auto Ecut = Eigen::MatrixXi();
    auto pln_eq = Eigen::MatrixXd(Everts.rows(), 1);

    //Evaluate the plane equation for all vertices
    for (int i = 0; i < Everts.rows(); i++) {
        //do a dot product for each vertex to the normal
        pln_eq(i) = (i, 0) * normal.x + Everts(i, 1) * normal.y + Everts(i, 2) * normal.z;
    }

    //get the vert edge structure of the contour
    igl::isolines(Everts, Einds, pln_eq, 2, Vcut, Ecut);


    ////print some stuff to read into rhino to figure out what the hell this does
    //for (int i = 0; i < Vcut.rows(); i++) {
    //    CULog("v%f,%f,%f", Vcut(i, 0), Vcut(i, 1), Vcut(i, 2));
    //}
    //for (int i = 0; i < Ecut.rows(); i++) {
    //    CULog("e%i,%i", Ecut(i, 0), Ecut(i, 1));
    //}

    return std::tuple<Eigen::MatrixXd, Eigen::MatrixXi>{Vcut, Ecut};
    


    // Debug square
    //return std::vector<Path2>{Path2(std::vector<Vec2>{Vec2(-1.5, -1.5), Vec2(1.5, -1.5), Vec2(1.5, 1.5), Vec2(-1.5, 1.5)})};
}