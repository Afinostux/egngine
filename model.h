/*
Copyright (c) 2014 Austin Fox (fostinaux@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#pragma once

#include "iqm.h"
//#include "util/array.h"
#include "util/egmath.h"
#include "GL/glew.h"

typedef struct egModel egModel;
typedef struct egVertex egVertex;
typedef struct egJoint egJoint;
typedef struct egMesh egMesh;
typedef struct egTriangle egTriangle;
typedef struct egMeshPattern egMeshPattern;
typedef struct egModelPattern egModelPattern;

struct egMesh {
    uint32_t ofs_vert, verts;
    uint32_t ofs_tri, tris;
    GLuint vao;
};

egMesh *        egMeshGet(uint32_t offset);
egTriangle *    egTriGet(uint32_t offset);
egVertex *      egVertGet(uint32_t offset);

struct egModel {
    uint32_t ofs_mesh, meshes;
    uint32_t ofs_joint, joints;
};

struct egJoint {
    unsigned int name;
    int parent; // parent < 0 means this is a root bone
    egV3 translate;
    egQuat rotate;
    egV3 scale;
    // translate is translation <Tx, Ty, Tz>, and rotate is quaternion rotation <Qx, Qy, Qz, Qw>
    // rotation is in relative/parent local space
    // scale is pre-scaling <Sx, Sy, Sz>
    // output = (input*scale)*rotation + translation
};

struct egTriangle {
    uint32_t indices[3];
};

struct egVertex {
    egV3 position;
    float texcoord[2];
    egV3 normal;
    float tangent[4];
    unsigned char blendindices[4], blendweights[4], color[4];
};

struct egMeshPattern {
    egVertex * vertices;
    egTriangle * triangles;
    size_t vert_count, triangle_count;
};

struct egModelPattern {
    egMeshPattern * meshes;
    egJoint * joints;
    size_t mesh_count, joint_count;
};

void        eg_initmodels();
void        eg_shutdownmodels();

void        eg_freeModelPattern(egModelPattern p);

egMesh *    eg_meshes_new(egMeshPattern p);
void        egModelNew(egModelPattern p, char name[16]);
egModel *   egModelGet(char name[16]);


