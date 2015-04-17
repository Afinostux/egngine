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
#include "iqm.h"
#include "model.h"
#include <physfs.h>
#include <stddef.h>
#include <string.h>


int eg_iqmvalidate(iqmheader * head)
{
    int magic = !strcmp(head->magic, "INTERQUAKEMODEL");
    int version = head->version == 2;
    if (magic && version) {
        return 1;
    }
    sprintf(iqmerror,"header mismatch:%s%s%s",!magic?" magic string mismatch":"",!magic&&!version?",":"",!version?" version mismatch":"");
    return 0;
}

int eg_iqmload(const char * const iqmfilename)
{
    PHYSFS_File * fiqm = PHYSFS_openRead(iqmfilename);
    egModelPattern modelpattern = {0};
    egVertex * vertices = 0;
    //FILE * fiqm = fopen(iqmfilename, "rb");
    int ecode = 1;//, isle = islittleendian();
    //printf("%s, %d\n",isle?"environment is little endian":"environment is big endian",isle);
    void * buffer = malloc(PHYSFS_fileLength(fiqm));
    //printf("header size: %u\n", sizeof(iqmheader));
    //printf("file length: %ul\n", PHYSFS_fileLength(fiqm));

    //for now, grabbing the whole file into memory is probably good enough. For later, fuck that horrible noise;
    PHYSFS_read(fiqm, buffer, sizeof(char), PHYSFS_fileLength(fiqm));

    //load header
    iqmheader * head = (iqmheader*)buffer;
    if (!eg_iqmvalidate(head)) {
        ecode = 0;
        goto error;
    }

    modelpattern.mesh_count = head->num_meshes;

    //load joints
    modelpattern.joint_count = head->num_joints;
    modelpattern.joints = (egJoint*)(buffer + head->ofs_joints);

    //load interleaved vertices from vertex arrays
    vertices = malloc(sizeof(egVertex) * head->num_vertexes);
    iqmvertexarray * varray = (iqmvertexarray*)(buffer + head->ofs_vertexarrays);
    for (int j = 0; j < head->num_vertexarrays; ++j) {
        void * vcopyto;
        void * vcopyfrom;
        size_t velement_size = 0, velement_count = 0, velement_offset = 0;
        switch (varray->type) {
        case IQM_POSITION     : // float, 3
            velement_size = sizeof(float);
            velement_count = 3;
            velement_offset = offsetof(egVertex, position);
            break;
        case IQM_TEXCOORD     : // float, 2
            velement_size = sizeof(float);
            velement_count = 2;
            velement_offset = offsetof(egVertex, texcoord);
            break;
        case IQM_NORMAL       : // float, 3
            velement_size = sizeof(float);
            velement_count = 3;
            velement_offset = offsetof(egVertex, normal);
            break;
        case IQM_TANGENT      : // float, 4
            velement_size = sizeof(float);
            velement_count = 4;
            velement_offset = offsetof(egVertex, tangent);
            break;
        case IQM_BLENDINDEXES : // ubyte, 4
            velement_size = sizeof(char);
            velement_count = 4;
            velement_offset = offsetof(egVertex, blendindices);
            break;
        case IQM_BLENDWEIGHTS : // ubyte, 4
            velement_size = sizeof(char);
            velement_count = 4;
            velement_offset = offsetof(egVertex, blendweights);
            break;
        case IQM_COLOR        : // ubyte, 4
            velement_size = sizeof(char);
            velement_count = 4;
            velement_offset = offsetof(egVertex, color);
            break;
        default:
            break;
        }
        vcopyto = (void*)(vertices);
        vcopyto += velement_offset;
        vcopyfrom = buffer + varray->offset;
        for (int w = 0; w < head->num_vertexes; ++w) {
            //copy elements
            memcpy(vcopyto, vcopyfrom, velement_size * velement_count);
            //increment pointers
            vcopyfrom += (velement_size * velement_count);
            vcopyto += sizeof(egVertex);
        }
        ++varray;
    }
    varray = 0;

    egTriangle * triangles = (egTriangle*)(buffer + head->ofs_triangles);

    //create new mesh
    modelpattern.meshes = malloc(sizeof(egMeshPattern) * head->num_meshes);
    iqmmesh * mesh = (iqmmesh*)(buffer + head->ofs_meshes);
    for (int i = 0; i < head->num_meshes; ++i) {
        egMeshPattern * mp = modelpattern.meshes + i;
        //load vertex arrays
        mp->vert_count = mesh->num_vertexes;
        mp->vertices = vertices + mesh->first_vertex;
        //mp->vertices = malloc(sizeof(egVertex) * mp->vert_count);
        //add vertex arrays to mesh

        //load triangles
        mp->triangle_count = mesh->num_triangles;
        mp->triangles = triangles + mesh->first_triangle;
        //mp->triangles = malloc(sizeof(egTriangle) * mp->triangle_count);
        //add triangles to mesh
        ++mesh;
    }


    static char mname[16] = {0};
    strncpy(mname, iqmfilename, 15);

    for (int i = (int)(strchr(iqmfilename, '.') - iqmfilename); i < 16; ++i) {
        mname[i] = '\0';
    }
    egModelNew(modelpattern, mname);
error:
    //free(joints);
    free(buffer);
    free(vertices);
    eg_freeModelPattern(modelpattern);
    PHYSFS_close(fiqm);
    return ecode;
}
