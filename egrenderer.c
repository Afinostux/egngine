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
#include "egrenderer.h"
#include <math.h>
#include <string.h>
#include "util/array.h"
egRenderer renderer = {0};


void egRendererRender(void)
{
    renderer.StartFrame();
    renderer.Set3D();
    renderer.RenderEntities();
    //renderer.RenderCollidables();
    if (renderer.RenderUI) {
        renderer.RenderUI();
    }

    renderer.FinishFrame();
}


void egRendererFrustum(float l, float r, float b, float t, float n, float f)
{
    renderer.projection.xx = 2 * n / (r - l);
    renderer.projection.xy = 0;
    renderer.projection.xz = 0;
    renderer.projection.xw = 0;
    renderer.projection.yx = 0;
    renderer.projection.yy = 2 * n / (t - b);
    renderer.projection.yz = 0;
    renderer.projection.yw = 0;
    renderer.projection.zx = (r + l) / (r - l);
    renderer.projection.zy = (t + b) / (t - b);
    renderer.projection.zz = -(f + n) / (f - n);
    renderer.projection.zw = -1;
    renderer.projection.wx = 0;
    renderer.projection.wy = 0;
    renderer.projection.wz = -2 * f * n / (f - n);
    renderer.projection.ww = 0;
}

void egRendererPerspective(float fovy, float aspect, float zNear, float zFar)
{
    float f  = (float)(1 / tan(fovy*M_PI_180/2));


    renderer.projection.xx	= f/aspect;
    renderer.projection.yx	= 0;
    renderer.projection.zx	= 0;
    renderer.projection.wx	= 0;

    renderer.projection.xy	= 0;
    renderer.projection.yy	= f;
    renderer.projection.zy	= 0;
    renderer.projection.wy	=0;

    renderer.projection.xz	= 0;
    renderer.projection.yz	= 0;
    renderer.projection.zz	=(zFar+zNear)/(zNear-zFar) ;
    renderer.projection.wz	=-1;

    renderer.projection.xw	= 0;
    renderer.projection.yw	= 0;
    renderer.projection.zw	= 2*(zFar*zNear)/(zNear-zFar);
    renderer.projection.ww	= 0;
}

void egRendererOrtho(float l, float r, float b, float t, float n, float f)
{
    renderer.projection.xx = 2 / (r - l);
    renderer.projection.xy = 0;
    renderer.projection.xz = 0;
    renderer.projection.xw = 0;

    renderer.projection.yx = 0;
    renderer.projection.yy = 2 / (t - b);
    renderer.projection.yz = 0;
    renderer.projection.yw = 0;

    renderer.projection.zx = 0;
    renderer.projection.zy = 0;
    renderer.projection.zz = -1 / (f - n);
    renderer.projection.zw = 0;

    renderer.projection.wx = -(r + l) / (r - l);
    renderer.projection.wy = -(t + b) / (t - b);
    renderer.projection.wz = -n / (f - n);
    renderer.projection.ww = 1;
}

void egRendererOrthoRatio(float w, float h, float scale, float n, float f)
{
    float ratio = w / h;
    h = scale * 0.5;
    w = ratio * scale * 0.5;
    //egRendererOrtho(-w, w, -h, h, n, f);
    glOrtho(-w, w, -h, h, n, f);
}

void egRendererLookAt( egV3 eye, egV3 lookat, egV3 up)
{

    egV3 zaxis = egV3Norm( egV3Sub(lookat, eye) );
    egV3 xaxis = egV3Norm( egV3Cross(up, zaxis) );
    egV3 yaxis = egV3Cross(zaxis, xaxis);

    memcpy(&renderer.modelview, &egMat4Id, sizeof(egMat4));

    renderer.modelview.xx = xaxis.x;
    renderer.modelview.xy = xaxis.y;
    renderer.modelview.xz = xaxis.z;

    renderer.modelview.yx = yaxis.x;
    renderer.modelview.yy = yaxis.y;
    renderer.modelview.yz = yaxis.z;

    renderer.modelview.zx = -zaxis.x;
    renderer.modelview.zy = -zaxis.y;
    renderer.modelview.zz = -zaxis.z;
}

egMap textureMap = {{0}};

int egRstrcmp(const void * a, const void * b) {
	return strcmp( (char*)a, (char*)b);
}

unsigned int egRendererGetTexid( const char * name )
{
    unsigned int texid, * texidp;
    size_t suffixstart;
    char * fname;

    if (textureMap.kcomp == 0) {
        textureMap = eg_map_new(2, sizeof(char) * 16, sizeof(unsigned int), egRstrcmp);
    }

    texidp = eg_map_get(&textureMap, name, 0);

    if (texidp == 0) {
        suffixstart = strlen(name);
        fname = malloc(suffixstart + 5);
        strcpy(fname, name);
        strcat(fname, ".png");
        //printf("filename %s from %s\n", fname, name);
        texid = renderer.LoadTexture(fname);
        free(fname);
        eg_map_insert(&textureMap, name, &texid);
        return texid;
    }
    return *texidp;
}

void egRendererSetTexture( const char * name )
{
    renderer.SetTexture(egRendererGetTexid(name));
}

void egRendererSetTexid( unsigned int texid )
{
    renderer.SetTexture( texid );
}
