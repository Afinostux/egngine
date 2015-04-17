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
#include "model.h"
#include "util/array.h"
#include "util/egmath.h"

egVec eg_vertices;
egVec eg_triangles;
egVec eg_joints;
egVec eg_meshes;
egMap eg_models;

int egModstrcmp( const void *a, const void *b ){
	return strcmp( (char*)a, (char*)b);
}

void eg_initmodels()
{
    eg_vertices = eg_vec_new(0,sizeof(egVertex));
    eg_triangles = eg_vec_new(0,sizeof(egTriangle));
    eg_joints = eg_vec_new(0,sizeof(egJoint));
    eg_meshes = eg_vec_new(0,sizeof(egJoint));
    eg_models = eg_map_new(0,(sizeof(char)*16), sizeof(egModel),egModstrcmp);
}

void eg_shutdownmodels()
{
    eg_vec_free(&eg_vertices);
    eg_vec_free(&eg_triangles);
    eg_vec_free(&eg_joints);
    eg_vec_free(&eg_meshes);
    eg_map_free(&eg_models);
}

egMesh * egMeshGet(uint32_t offset)
{
    return eg_vec_at(&eg_meshes, offset);
}

egTriangle * egTriGet(uint32_t offset)
{
    return eg_vec_at(&eg_triangles, offset);
}

egVertex * egVertGet(uint32_t offset)
{
    return eg_vec_at(&eg_vertices, offset);
}

void eg_freeModelPattern(egModelPattern p)
{
    //egMeshPattern m;
    free(p.meshes);
}

egMesh * eg_meshes_new(egMeshPattern p)
{
    egMesh m;
    m.ofs_vert = eg_vertices.element_count;
    m.ofs_tri = eg_triangles.element_count;

    m.verts = p.vert_count;
    m.tris = p.triangle_count;
    for (int i = 0; i < p.vert_count; ++i) {
        eg_vec_push(&eg_vertices, (p.vertices) + i);
        //printf("\n\tVertex %u\n", i);
        //egV3Print((p.vertices + i)->position);
    }
    for (int i = 0; i < p.triangle_count; ++i) {
        eg_vec_push(&eg_triangles, (p.triangles) + i);
        //printf("\tTriangle %u\n\t%u\t%u\t%u\n", i, (p.triangles + i)->indices[0], (p.triangles + i)->indices[1], (p.triangles + i)->indices[2]);
    }
    //eg_map_at(&eg_meshes, eg_map_insert(&eg_meshes, name, &m),0,&result);
    return eg_vec_at(&eg_meshes, eg_vec_push(&eg_meshes, &m));
}

void egModelNew(egModelPattern p, char name[16])
{
    egModel m;

    m.ofs_mesh = eg_meshes.element_count;
    m.ofs_joint = eg_joints.element_count;

    m.joints = p.joint_count;
    m.meshes = p.mesh_count;
    //printf("loading model %s\njoints:\t%u\nmeshes:\t%u\n--------------------------------\n", name, p.joint_count, p.mesh_count);

    for (int i = 0; i < p.joint_count; ++i) {
        eg_vec_push(&eg_joints, p.joints + i);
        //printf("\nJoint %u\nparent %d\n", i, (p.joints + i)->parent);
        //egV3Print((p.joints + i)->translate);
        //egQuatPrint((p.joints + i)->rotate);
        //egV3Print((p.joints + i)->scale);
    }
    for (int i = 0; i < p.mesh_count; ++i) {
        //printf("\nMesh %u\n", i);
        eg_meshes_new(*(p.meshes + i));
    }
    eg_map_insert(&eg_models, &name[0], &m);
}

egModel * egModelGet(char name[16])
{
    return eg_map_get(&eg_models, &name[0], 0);
}
