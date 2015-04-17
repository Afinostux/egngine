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
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_1_PI
#define M_1_PI 0.31830988618379067153776752674503
#endif

#ifndef M_2_PI
#define M_2_PI 0.63661977236758134307553505349006
#endif

#ifndef M_180_PI
#define M_180_PI 57.295779513082320876798154814105
#endif

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576923690768489
#endif

//all of these should always be passed by reference
//math vector types
typedef struct egV2 egV2;
typedef struct egV3 egV3;
typedef struct egV4 egV4;
typedef struct egV4 egQuat;

//math matrix types
typedef struct egMat2 egMat2;
typedef struct egMat3 egMat3;
typedef struct egMat4 egMat4;

struct egV2 {
    float x, y;
};

struct egV3 {
    float x, y, z;
};

struct egV4 {
    float x, y, z, w;
};

struct egMat2 {
    float xx, xy;
    float yx, yy;
};

struct egMat3 {
    float xx, xy, xz;
    float yx, yy, yz;
    float zx, zy, zz;
};

struct egMat4 {
    float xx, xy, xz, xw;
    float yx, yy, yz, yw;
    float zx, zy, zz, zw;
    float wx, wy, wz, ww;
};

extern const egMat2 egMat2Id;
extern const egMat3 egMat3Id;
extern const egMat4 egMat4Id;

extern const egV3 egV3Zero;
extern const egQuat egQuatId;

//scalar functions

int iCmp(const void * a, const void * b);

float 	egLerp(float a, float b, float t);
float 	egRSqrt( float x );
float 	egSinZH( float a );
float 	egSin( float a );
float 	egAtanP( float y, float x );

float   egRandRange(float min, float max);

//2d vector functions

egV2    egV2N(float x, float y);

egV2 	egV2Add(egV2 a, egV2 b);
egV2 	egV2Mul(egV2 a, float b);
egV2 	egV2Sub(egV2 a, egV2 b);

egV2 	egV2Lerp(egV2 a, egV2 b, float t);
egV2 	egV2Norm(egV2 a);
float 	egV2Cross(egV2 a, egV2 b);
int 	egV2Equal(egV2 a, egV2 b);
float 	egV2Len(egV2 a);
float 	egV2LenSq(egV2 a);
float 	egV2Dot(egV2 a, egV2 b);
float 	egV2Dist(egV2 a, egV2 b);
float 	egV2DistSq(egV2 a, egV2 b);

egV2 	egV2Rotate(egV2 a, float r);

void 	egV2Print(egV2 v);

//3d vector functions

egV3	egV3N(float x, float y, float z);

egV3 	egV3Add(egV3 a, egV3 b);
egV3 	egV3Mul(egV3 a, float b);
egV3 	egV3Sub(egV3 a, egV3 b);

egV3 	egV3Lerp(egV3 a, egV3 b, float t);
egV3 	egV3Norm(egV3 a);
egV3 	egV3Cross(egV3 a, egV3 b);
int 	egV3Equal(egV3 a, egV3 b);
float 	egV3Len(egV3 a);
float 	egV3LenSq(egV3 a);
float 	egV3Dot(egV3 a, egV3 b);
float 	egV3Dist(egV3 a, egV3 b);
float 	egV3DistSq(egV3 a, egV3 b);

void 	egV3Print(egV3 v);

//4d vector functions

egV4 	egV4Add(egV4 a, egV4 b);
egV4 	egV4Mul(egV4 a, float b);
egV4 	egV4Sub(egV4 a, egV4 b);

egV4 	egV4Lerp(egV4 a, egV4 b, float t);
egV4 	egV4Norm(egV4 a);
int 	egV4Equal(egV4 a, egV4 b);
float 	egV4Len(egV4 a);
float 	egV4LenSq(egV4 a);
float 	egV4Dot(egV4 a, egV4 b);
float 	egV4Dist(egV4 a, egV4 b);
float 	egV4DistSq(egV4 a, egV4 b);

void 	egV4Print(egV4 v);

//quaternion functions

egQuat  egQuatN(float x, float y, float z, float w);

egQuat 	egQuatAdd(egQuat a, egQuat b);
egQuat 	egQuatMul(egQuat a, float b);
egQuat 	egQuatSub(egQuat a, egQuat b);
void 	egQuatPrint(egQuat q);

egQuat 	egQuatXQ(egQuat a, egQuat b);
egQuat 	egQuatNorm(egQuat a);
egV3 	egQuatToEuler(egQuat a);

egQuat 	egQuatFromAngleAxis(float angle, egV3 axis);
egQuat 	egQuatFromX(float x);
egQuat 	egQuatFromY(float y);
egQuat 	egQuatFromZ(float z);

egQuat  egQuatFromLocalX(const egQuat q, float x);
egQuat  egQuatFromLocalY(const egQuat q, float y);
egQuat  egQuatFromLocalZ(const egQuat q, float z);

float 	egQuatDot(egQuat a, egQuat b);
egQuat 	egQuatSlerp(egQuat a, egQuat b, float t);
void 	egSlerpOptimized( const egQuat *from, const egQuat *to, float t, egQuat *result );
egQuat 	egQuatNlerp(egQuat a, egQuat b, float t);

egMat4 	egQuatMat4(egQuat a);

//mat2 matrix functions

egMat2 	egMat2Add(egMat2 a, egMat2 b);
egMat2 	egMat2Mul(egMat2 a, egMat2 b);
egMat2 	egMat2Sub(egMat2 a, egMat2 b);
void 	egMat2Print(egMat2 m);

//mat3 matrix functions

egMat3 	egMat3Add(egMat3 a, egMat3 b);
egMat3 	egMat3Mul(egMat3 a, egMat3 b);
egMat3 	egMat3Sub(egMat3 a, egMat3 b);
void 	egMat3Print(egMat3 m);

//mat4 matrix functions

egMat4 	egMat4Add(egMat4 a, egMat4 b);
egMat4 	egMat4Mul(egMat4 a, egMat4 b);
egMat4 	egMat4Sub(egMat4 a, egMat4 b);

egMat4	egMat4Move(egMat4 a, egV3 v);
egMat4	egMat4Scale(egMat4 a, egV3 v);

void 	egMat4TransVec3(egMat4 a, egV3 v, egV3 * out);
void 	egMat4TransVec3A(egMat4 a, egV3 * v, egV3 * out, size_t count);

void 	egMat4Print(egMat4 m);

void	egMat4Transpose(egMat4 * a);
