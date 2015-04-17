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

#include "GL/glew.h"
#include <SDL2/SDL.h>
#include "util/egmath.h"
#include "model.h"

enum eg_renderer_e {
    EG_GL3 = 0,
    EG_CRASH,
    EG_COUNT
};

typedef struct egCamera {
    egV3 position;
    egV3 lookat;
    egV3 up;
    union {
        float scale;
        float angle;
    };
    int projection;
} egCamera;

typedef struct egRenderer {
    int w, h;
    SDL_Window * window;
    SDL_GLContext context;
    int r_type;
    void (*Set3D)(void);
    void (*FinishFrame)(void);
    void (*StartFrame)(void);
    unsigned int (*LoadTexture)(const char *);
    void (*SetTexture)(unsigned int);
    void (*RenderEntities)(void);
    void (*RenderCollidables)(void);
    void (*RenderUI)(void);

    egCamera camera;

    egMat4 projection;
    egMat4 modelview;
} egRenderer;

extern egRenderer renderer;

void egRendererRender(void);

void egRendererFrustum(float l, float r, float b, float t, float n, float f);
void egRendererPerspective(float angle, float aspect, float n, float f);
void egRendererOrtho(float l, float r, float b, float t, float n, float f);
void egRendererOrthoRatio(float w, float h, float scale, float n, float f);

unsigned int egRendererGetTexid( const char * name );
void egRendererSetTexture( const char * name );
void egRendererSetTexid( unsigned int texid );

void egRendererLookAt( egV3 eye, egV3 lookat, egV3 up);
