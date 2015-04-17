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
#include "eggl3renderer.h"
#include "egmem.h"
#include "egentity.h"
#include "GL/glew.h"
#include <SDL2/SDL.h>
#include "SOIL.h"
#include <physfs.h>
#include "egcollision.h"

void egGL3Set3D(void);
void egGL3FinishFrame(void);
void egGL3StartFrame(void);
void egGL3SetTexture(unsigned int);
unsigned int egGL3LoadTexture(const char *);
void egGL3RenderEntities(void);
void egGL3RenderColliders(void);

void egDestroyRendererGL3(void)
{
    SDL_GL_DeleteContext(renderer.context);
    SDL_DestroyWindow(renderer.window);
    SDL_Quit();
}

int egSetRendererGL3			( int w, int h )
{
    if ( renderer.window ) {
        //repeat call to resize window
        renderer.w = w;
        renderer.h = h;
    } else {
        //first time setup
        renderer.Set3D = &egGL3Set3D;
        renderer.FinishFrame = &egGL3FinishFrame;
        renderer.SetTexture = &egGL3SetTexture;
        renderer.LoadTexture = egGL3LoadTexture;
        renderer.RenderEntities = &egGL3RenderEntities;
        renderer.StartFrame = &egGL3StartFrame;
        renderer.RenderCollidables = &egGL3RenderColliders;
        renderer.w = w;
        renderer.h = h;
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );

        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

        renderer.window = SDL_CreateWindow( "HAM STRIFE HAM STRIFE HAM STRIFE",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            renderer.w,
                                            renderer.h,
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

        //glViewport(0, 0, renderer.w, renderer.h);

        const char *error = SDL_GetError( );
        if ( *error != '\0' ) {
            printf( "SDL error: %s + line: %i\n", error, __LINE__ );
            SDL_ClearError( );
        }

        renderer.context = SDL_GL_CreateContext( renderer.window );

        error = SDL_GetError( );
        if ( *error != '\0' ) {
            printf( "SDL error: %s + line: %i\n", error, __LINE__ );
            SDL_ClearError( );
        }

        //glEnable(GL_FOG);
        //glFogi(GL_FOG_MODE, GL_LINEAR);
        //glFogf(GL_FOG_END, 43.f);

        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        //glFogf(GL_FOG_DENSITY, 0.01);


        SDL_GL_SetSwapInterval(1);

        //glDisable(GL_ALPHA_TEST);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);

        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        //glDisableClientState(GL_COLOR_ARRAY);

        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        atexit(egDestroyRendererGL3);
        return 1;
    }
    return 0;
}


void egGL3Set3D(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDisable(GL_BLEND);
    //glEnableClientState (GL_NORMAL_ARRAY);

    //glEnable(GL_TEXTURE_2D);

}

void egGL3FinishFrame(void)
{
    SDL_GL_SwapWindow(renderer.window);
}

void egGL3StartFrame(void)
{
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (renderer.camera.projection) {
    case 0:
        //ortho
        egRendererOrthoRatio(renderer.w, renderer.h, renderer.camera.scale, 1.f, 100.f);

        break;
    default:
        //egRendererPerspective(renderer.camera.angle, renderer.w/renderer.h, 1.f, 100.f);
        //egRendererPerspective(renderer.camera.angle, renderer.w/renderer.h, 1.f, 100.f);
        gluPerspective(renderer.camera.angle, ((float)renderer.w)/renderer.h, 1.f, 100.f);

        break;
    }
    //egMat4Transpose(&renderer.projection);
    //glLoadMatrixf((float*)&renderer.projection);
    //glFrustum(-3, 3, -3, 3, 20.0f, 100.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(	renderer.camera.position.x,
                renderer.camera.position.y,
                renderer.camera.position.z,
                renderer.camera.lookat.x,
                renderer.camera.lookat.y,
                renderer.camera.lookat.z,
                renderer.camera.up.x,
                renderer.camera.up.y,
                renderer.camera.up.z);


    //glTranslatef(0.f, 0.f, -20.f);
    //egV3 lookvector = egV3Norm(egV3Sub(renderer.camera.lookat, renderer.camera.position) );
    //egV2 lookatflat = {lookvector.x, lookvector.y};
    //lookatflat = egV2Norm(lookatflat);
//
//	//glTranslatef(-renderer.camera.position.x, -renderer.camera.position.y, -renderer.camera.position.z);
//
//	//float angle = atan2f( lookatflat.y - 1, lookatflat.x);
//	//glRotatef(angle, 0, 0, 1);
//	//angle = atan2f(lookvector.z, sqrtf(lookvector.x * lookvector.x + lookvector.y * lookvector.y));
    //glRotatef(angle, 1, 0, 0);



    //egRendererLookAt(renderer.camera.lookat, renderer.camera.position, renderer.camera.up);
    //glLoadIdentity();

    //glLoadIdentity();
    //glTranslatef(0.f, 0.f, -20.f);
}

void egGL3SetTexture(unsigned int texid)
{
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texid);
}

unsigned int egGL3LoadTexture(const char * filename)
{
    if (!PHYSFS_exists(filename))
        return 0;
    PHYSFS_File * f = PHYSFS_openRead(filename);
    unsigned int result, fsize = PHYSFS_fileLength(f);
    unsigned char * data = malloc(sizeof(char) * fsize);

    PHYSFS_read(f, data, sizeof(char), fsize);

    result = SOIL_load_OGL_texture_from_memory(data, fsize, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    PHYSFS_close(f);
    free(data);
    return result;//SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
}

void egGL3RenderEntities(void)
{
    egMemPool entpool = egEntPool();
    egEntity * tentity;
    egModel * tmodel;
    egMesh * tmesh;
    egTriangle * ttri;
    egVertex * tvert;
    size_t eid = egMemPoolFirst(entpool);
    tentity = (egEntity*)egMemPoolNext(entpool, &eid);

    egMat4 tmat;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    while(tentity) {
        //glGetFloatv(GL_MODELVIEW, (float*)&renderer.projection);
        glPushMatrix();
        //printf("%u\n", i);
        //printf("%u %u\n", (size_t)tentity->model, tentity->texid);
        tmodel = tentity->model;
        tmat = egQuatMat4(tentity->rotation);
        renderer.SetTexture(tentity->texid);

        glTranslatef(tentity->position.x, tentity->position.y, tentity->position.z);
        //egMat4Transpose(&tmat);
        glMultMatrixf((float*)&tmat);



        for (int j = 0; j < tmodel->meshes; ++j) {
            tmesh = egMeshGet(tmodel->ofs_mesh) + j;
            ttri = egTriGet(tmesh->ofs_tri);
            tvert = egVertGet(tmesh->ofs_vert);

            glVertexPointer(3, GL_FLOAT, sizeof(egVertex), (void*)&(tvert->position));
            glTexCoordPointer(2, GL_FLOAT, sizeof(egVertex), (void*)&(tvert->texcoord));
            glDrawElements(GL_TRIANGLES, tmesh->tris * 3, GL_UNSIGNED_INT, (void*)ttri);
        }
        //glLoadMatrixf((float*)&renderer.projection);
        glPopMatrix();
        tentity = (egEntity*)egMemPoolNext(entpool, &eid);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void egGL3RenderColliders(void)
{
    egMemPool colliders = egColliderPool();
    //glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glColor4f(0.0f, 1.0f, 0.0f, 0.3f);

    static egV3 verts[4] = {{0}};
    static uint8_t indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    float top, bottom, left, right;

    size_t id = egMemPoolFirst(colliders);
    egCollider * c = (egCollider*)egMemPoolNext(colliders, &id);

    glEnableClientState(GL_VERTEX_ARRAY);
    while (c) {
        top     = c->position.y + c->height;
        bottom  = c->position.y - c->height;

        left    = c->position.x + c->width;
        right   = c->position.x - c->width;

        verts[0].z = top;
        verts[0].y = left;

        verts[1].z = top;
        verts[1].y = right;

        verts[2].z = bottom;
        verts[2].y = right;

        verts[3].z = bottom;
        verts[3].y = left;
        glVertexPointer(3, GL_FLOAT, 0, verts);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
        c = (egCollider*)egMemPoolNext(colliders, &id);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisable(GL_BLEND);
}
