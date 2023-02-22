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

using namespace cugl;

/**
 * A class representing a mesh
 */
class Mesh{
    
#pragma mark State
private:
    /** Level mesh representation */
    int _mesh;
    
public:
    /** A public accessible, read-only version of the mesh */
    const int& mesh;
    
#pragma mark Main Functions
public:
    /**
     * Creates the mesh.
     */
    Mesh(): mesh(_mesh) {
        setMesh();
    }
    
#pragma mark Setters
public:
    /**
     *  Sets the starting position of the player
     *
     *  @param startLoc The starting location of the player
     */
    void setMesh() {
        _mesh = 42;
    }
    
    
};


#endif /* Mesh_h */
