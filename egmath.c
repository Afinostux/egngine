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

#include "util/egmath.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

const egMat2 egMat2Id = {
    1, 0,
    0, 1
};

const egMat3 egMat3Id = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
};

const egMat4 egMat4Id = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

const egV3 egV3Zero = {0, 0, 0};
const egQuat egQuatId = {0, 0, 0, 1};

//scalar functions

int iCmp(const void * a, const void * b)
{
    return *(int*)a - *(int*)b;
}

float egLerp( float a, float b, float t )
{
    return a + t * ( b - a );
}


float egRSqrt( float x )
{
    //from http://fabiensanglard.net/doom3_documentation/37725-293747_293747.pdf
    long i;
    float y, r;
    y = x * 0.5f;
    i = *( long * )( &x );
    i = 0x5f3759df - ( i >> 1 );
    r = *( float * )( &i );
    r = r * ( 1.5f - r * r * y );
    return r;
}

float egSinZH( float a )
{
    //from http://fabiensanglard.net/doom3_documentation/37725-293747_293747.pdf
    float s, t;
    s = a * a;
    t = -2.39e-08f;
    t *= s;
    t += 2.7526e-06f;
    t *= s;
    t += -1.98409e-04f;
    t *= s;
    t += 8.3333315e-03f;
    t *= s;
    t += -1.666666664e-01f;
    t *= s;
    t += 1.0f;
    t *= a;
    return t;
}

float egAtanP( float y, float x )
{
    //from http://fabiensanglard.net/doom3_documentation/37725-293747_293747.pdf
    float a, d, s, t;
    if ( y > x ) {
        a = -x / y;
        d = M_PI / 2;
    } else {
        a = y / x;
        d = 0.0f;
    }
    s = a * a;
    t = 0.0028662257f;
    t *= s;
    t += -0.0161657367f;
    t *= s;
    t += 0.0429096138f;
    t *= s;
    t += -0.0752896400f;
    t *= s;
    t += 0.1065626393f;
    t *= s;
    t += -0.1420889944f;
    t *= s;
    t += 0.1999355085f;
    t *= s;
    t += -0.3333314528f;
    t *= s;
    t += 1.0f;
    t *= a;
    t += d;
    return t;
}

float egSin( float a )
{
    short even = ( short )( a * M_2_PI )%2, negate = ( short )( a * M_1_PI )%2;
    a = (even == 0)?fmodf(a, M_PI * 0.5):1 - fmodf(a, M_PI * 0.5);
    return (negate)?egSinZH(a) * -1:egSinZH(a);
}

float egRandRange(float min, float max)
{
    const int RAND_MOD = RAND_MAX/10;
    int start;
    float scale, output;

    start = rand() % RAND_MOD;
    scale = (float)start / RAND_MOD;
    output = (scale * (max - min)) + min;
    return output;
}

//2d vector functions

egV2 egV2N(float x, float y)
{
    egV2 v = {x, y};
    return v;
}

egV2 egV2Add( egV2 a, egV2 b )
{
    egV2 v = {
        a.x + b.x,
        a.y + b.y
    };
    return v;
}

egV2 egV2Mul( egV2 a, float b )
{
    egV2 v = {
        a.x * b,
        a.y * b
    };
    return v;
}

egV2 egV2Sub( egV2 a, egV2 b )
{
    egV2 v = {
        a.x - b.x,
        a.y - b.y
    };
    return v;
}

egV2 egV2Lerp( egV2 a, egV2 b, float t )
{
    egV2 v = {
        a.x + t*( b.x - a.x ),
        a.y + t*( b.y - a.y )
    };
    return v;
}

egV2 egV2Norm( egV2 a )
{
    float im = 1.f/sqrtf( a.x * a.x + a.y * a.y );
    egV2 v = {
        a.x * im,
        a.y * im
    };
    return v;
}

float egV2Cross( egV2 a, egV2 b )
{
    return a.x * b.y - a.y * b.x;
}

int egV2Equal( egV2 a, egV2 b )
{
    return a.x == b.x && a.y == b.y;
}

float egV2Len( egV2 a )
{
    return sqrtf( a.x * a.x + a.y * a.y );
}

float egV2LenSq( egV2 a )
{
    return a.x * a.x + a.y * a.y;
}

float egV2Dot( egV2 a, egV2 b )
{
    return a.x * b.x + a.y * b.y;
}

float egV2Dist( egV2 a, egV2 b )
{
    float x = b.x - a.x, y = b.y - a.y;
    return sqrtf( x * x + y * y );
}

float egV2DistSq( egV2 a, egV2 b )
{
    float x = b.x - a.x, y = b.y - a.y;
    return x * x + y * y;
}

egV2 egV2Rotate( egV2 a, float r )
{
    float
    s = sinf( r),
    c = cosf( r);
    egV2 v = {
        ( a.x * c ) + ( a.y * s ),
        ( a.x * s ) + ( a.y * c )
    };
    return v;
}

void egV2Print( egV2 v )
{
    printf( "vec2 ( " );
    float * vp = ( float* )&v;
    for ( int i = 0; i < 2; ++i ) {
        printf( "\t%f ", *( vp + i ));
    }
    printf( " )\n" );
}

//3d vector functions

egV3	egV3N(float x, float y, float z)
{
    egV3 v = {
        x, y, z
    };
    return v;
}

egV3 egV3Add( egV3 a, egV3 b )
{
    egV3 v = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
    return v;
}

egV3 egV3Mul( egV3 a, float b )
{
    egV3 v = {
        a.x * b,
        a.y * b,
        a.z * b
    };
    return v;
}

egV3 egV3Sub( egV3 a, egV3 b )
{
    egV3 v = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
    return v;
}

egV3 egV3Lerp( egV3 a, egV3 b, float t )
{
    egV3 v = {
        a.x + t*( b.x - a.x ),
        a.y + t*( b.y - a.y ),
        a.z + t*( b.z - a.z )
    };
    return v;
}

egV3 egV3Norm( egV3 a )
{
    float im = 1.f/sqrtf( a.x * a.x + a.y * a.y + a.z * a.z );
    egV3 v = {
        a.x * im,
        a.y * im,
        a.z * im
    };
    return v;
}

egV3 egV3Cross( egV3 a, egV3 b )
{
    egV3 v = {
        ( a.y * b.z - a.z * b.y ),
        ( a.z * b.x - a.x * b.z ),
        ( a.x * b.y - a.y * b.x )
    };
    return v;
}

int egV3Equal( egV3 a, egV3 b )
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

float egV3Len( egV3 a )
{
    return sqrtf( a.x * a.x + a.y * a.y + a.z * a.z );
}

float egV3LenSq( egV3 a )
{
    return ( a.x * a.x + a.y * a.y + a.z * a.z );
}

float egV3Dot( egV3 a, egV3 b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float egV3Dist( egV3 a, egV3 b )
{
    float x = b.x - a.x, y = b.y - a.y, z = b.z - a.z;
    return sqrtf( x * x + y * y + z * z );
}

float egV3DistSq( egV3 a, egV3 b )
{
    float x = b.x - a.x, y = b.y - a.y, z = b.z - a.z;
    return x * x + y * y + z * z;
}

void egV3Print( egV3 v )
{
    printf( "vec3 ( " );
    float * vp = ( float* )&v;
    for ( int i = 0; i < 3; ++i ) {
        printf( "\t%f ", *( vp + i ));
    }
    printf( " )\n" );
}

//4d vector functions

egV4 egV4Add( egV4 a, egV4 b )
{
    egV4 v = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
    return v;
}

egV4 egV4Mul( egV4 a, float b )
{
    egV4 v = {
        a.x * b,
        a.y * b,
        a.z * b,
        a.w * b
    };
    return v;
}

egV4 egV4Sub( egV4 a, egV4 b )
{
    egV4 v = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
    return v;
}

egV4 egV4Lerp( egV4 a, egV4 b, float t )
{
    egV4 v = {
        a.x + t*( b.x - a.x ),
        a.y + t*( b.y - a.y ),
        a.z + t*( b.z - a.z ),
        a.w + t*( b.w - a.w )
    };
    return v;
}

egV4 egV4Norm( egV4 a )
{
    float im = 1.f/sqrtf( a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w );
    egV4 v = {
        a.x * im,
        a.y * im,
        a.z * im,
        a.w * im
    };
    return v;
}

int egV4Equal( egV4 a, egV4 b )
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

float egV4Len( egV4 a )
{
    return sqrtf( a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w );
}

float egV4LenSq( egV4 a )
{
    return a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
}

float egV4Dot( egV4 a, egV4 b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float egV4Dist( egV4 a, egV4 b )
{
    float x = b.x - a.x, y = b.y - a.y, z = b.z - a.z, w = b.w - a.w;
    return sqrtf( x * x + y * y + z * z + w * w );
}

float egV4DistSq( egV4 a, egV4 b )
{
    float x = b.x - a.x, y = b.y - a.y, z = b.z - a.z, w = b.w - a.w;
    return x * x + y * y + z * z + w * w;
}

void egV4Print( egV4 v )
{
    printf( "vec4 ( " );
    float * vp = ( float* )&v;
    for ( int i = 0; i < 4; ++i ) {
        printf( "\t%f ", *( vp + i ));
    }
    printf( " )\n" );
}

//quaternion functions

egQuat  egQuatN(float x, float y, float z, float w)
{
    egQuat q = {x, y, z, w};
    return q;
}

egQuat egQuatAdd( egQuat a, egQuat b )
{
    egQuat q = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
    return q;
}

egQuat egQuatMul( egQuat a, float b )
{
    egQuat q = {
        a.x * b,
        a.y * b,
        a.z * b,
        a.w * b
    };
    return q;
}

egQuat egQuatSub( egQuat a, egQuat b )
{
    egQuat q = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
    return q;
}

egQuat egQuatXQ( egQuat a, egQuat b )
{
    egQuat q = {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
    return q;
}

egQuat egQuatNorm( egQuat a )
{
    float im = 1.f/sqrtf( a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w );
    egQuat q = {
        a.x * im,
        a.y * im,
        a.z * im,
        a.w * im
    };
    return q;
}

egV3 egQuatToEuler( egQuat a )
{
    float
    sx = a.x * a.x,
    sy = a.y * a.y,
    sz = a.z * a.z,
    sw = a.w * a.w;

    egV3 v = {
        asinf( -2.0f * ( a.x * a.z - a.y * a.w ) ),
        atan2f( 2.0f * ( a.y * a.z + a.x * a.w ), ( -sx - sy + sz + sw ) ),
        atan2f( 2.0f * ( a.x * a.y + a.z * a.w ), ( sx - sy - sz + sw ) )
    };

    return v;
}

egQuat egQuatFromAngleAxis( float angle, egV3 axis )
{
    axis = egV3Norm( axis );

    float
    s = sinf( angle / 2.0f ),
    c = cosf( angle / 2.0f );

    egQuat q = {
        axis.x * s,
        axis.y * s,
        axis.z * s,
        c
    };

    return q;
}

egQuat egQuatFromX( float x )
{
    egV3 axis = {1.f,0,0};
    return egQuatFromAngleAxis( x, axis );
}

egQuat egQuatFromY( float y )
{
    egV3 axis = {0,1.f,0};
    return egQuatFromAngleAxis( y, axis );
}

egQuat egQuatFromZ( float z )
{
    egV3 axis = {0,0,1.f};
    return egQuatFromAngleAxis( z, axis );
}

egQuat  egQuatFromLocalX(const egQuat q, float x)
{
    egV3 localx = {
        1 - 2*q.y*q.y - 2*q.z*q.z,	2*q.x*q.y - 2*q.z*q.w,		2*q.x*q.z + 2*q.y*q.w
    };
    return egQuatFromAngleAxis(x, localx);
}

egQuat  egQuatFromLocalY(const egQuat q, float y)
{
    egV3 localy = {
        2*q.x*q.y + 2*q.z*q.w,		1 - 2*q.x*q.x - 2*q.z*q.z,	2*q.y*q.z - 2*q.x*q.w
    };
    return egQuatFromAngleAxis(y, localy);
}

egQuat  egQuatFromLocalZ(const egQuat q, float z)
{
    egV3 localz = {
        2*q.x*q.z - 2*q.y*q.w,		2*q.y*q.z + 2*q.x*q.w,		1 - 2*q.x*q.x - 2*q.y*q.y
    };
    return egQuatFromAngleAxis(z, localz);
}

float 	egQuatDot( egQuat a, egQuat b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

#define SLERP_THRESHOLD 0.99f
egQuat egQuatSlerp( egQuat a, egQuat b, float t )
{
    //fuck all that complicated shit, I'm going to take a shot at makin' slerp my way
    float
    dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w,
    angle,
    isine;

    //make sure we're taking the shortest path
    if ( dot < 0.f ) {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
        dot = -dot;
    }

    //just lerp if the distance is super short
    if ( dot < SLERP_THRESHOLD ) {
        //slerp it
        angle = acosf( dot );
        isine = 1.f/sqrtf( 1.0f - ( dot * dot ));
        t = sin( t * angle ) * isine;
    }

    egQuat q = {
        a.x + t*( b.x - a.x ),
        a.y + t*( b.y - a.y ),
        a.z + t*( b.z - a.z ),
        a.w + t*( b.w - a.w )
    };
    //just gonna reuse isine since it's done
    isine = 1.f/sqrtf( q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w );
    q.x *= isine;
    q.y *= isine;
    q.z *= isine;
    q.w *= isine;
    return q;
}
#undef SLERP_THRESHOLD

void egSlerpOptimized( const egQuat *from, const egQuat *to, float t, egQuat *result )
{
    float cosom, absCosom, sinom, sinSqr, omega, scale0, scale1;
    cosom = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;
    absCosom = fabs( cosom );
    if ( ( 1.0f - absCosom ) > 1e-6f ) {
        sinSqr = 1.0f - absCosom * absCosom;
        sinom = egRSqrt( sinSqr );
        omega = egAtanP( sinSqr * sinom, absCosom );
        scale0 = egSinZH( ( 1.0f - t ) * omega ) * sinom;
        scale1 = egSinZH( t * omega ) * sinom;
    } else {
        scale0 = 1.0f - t;
        scale1 = t;
    }
    scale1 = ( cosom >= 0.0f ) ? scale1 : -scale1;
    result->x = scale0 * from->x + scale1 * to->x;
    result->y = scale0 * from->y + scale1 * to->y;
    result->z = scale0 * from->z + scale1 * to->z;
    result->w = scale0 * from->w + scale1 * to->w;
}

egQuat egQuatNlerp( egQuat a, egQuat b, float t )
{
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w, im = 1-t;
    if ( dot < 0.f ) {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
    }
    egQuat q = {
        t * b.x + im * a.x,
        t * b.y + im * a.y,
        t * b.z + im * a.z,
        t * b.w + im * a.w
    };
    im = 1.f/sqrtf( q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w );
    q.x *= im;
    q.y *= im;
    q.z *= im;
    q.w *= im;
    return q;
}

egMat4 egQuatMat4( egQuat a )
{
    egMat4 m = {
        1 - 2*a.y*a.y - 2*a.z*a.z,	2*a.x*a.y - 2*a.z*a.w,		2*a.x*a.z + 2*a.y*a.w,		0,
        2*a.x*a.y + 2*a.z*a.w,		1 - 2*a.x*a.x - 2*a.z*a.z,	2*a.y*a.z - 2*a.x*a.w,		0,
        2*a.x*a.z - 2*a.y*a.w,		2*a.y*a.z + 2*a.x*a.w,		1 - 2*a.x*a.x - 2*a.y*a.y,	0,
        0,							0,							0,							1
    };
    return m;
}

void egQuatPrint( egQuat q )
{
    printf( "quaternion ( " );
    float * qp = ( float* )&q;
    for ( int i = 0; i < 4; ++i ) {
        printf( "\t%f ", *( qp + i ));
    }
    printf( " )\n" );
}

//mat2 matrix functions

egMat2 egMat2Add( egMat2 a, egMat2 b )
{
    egMat2 m = {
        a.xx + b.xx,
        a.xy + b.xy,
        a.yx + b.yx,
        a.yy + b.yy
    };
    return m;
}

egMat2 egMat2Mul( egMat2 a, egMat2 b )
{
    egMat2 m = {
        a.xx * b.xx + a.xy * b.yx,//xx
        a.xx * b.xy + a.xy * b.yy,//xy
        a.yx * b.xx + a.yy * b.yx,//yx
        a.yx * b.xy + a.yy * b.yy //yy
    };
    return m;
}

egMat2 egMat2Sub( egMat2 a, egMat2 b )
{
    egMat2 m = {
        a.xx - b.xx,
        a.xy - b.xy,
        a.yx - b.yx,
        a.yy - b.yy
    };
    return m;
}

void egMat2Print( egMat2 m )
{
    printf( "mat2\n" );
    float * mp = ( float* )&m;
    for ( int i = 0; i < 2; ++i ) {
        for ( int j = 0; j < 2; ++j ) {
            printf( "\t%f ", *( mp + j + 2*i ));
        }
        printf( "\n" );
    }
}

//mat3 matrix functions

egMat3 egMat3Add( egMat3 a, egMat3 b )
{
    egMat3 m = {
        a.xx + b.xx,
        a.xy + b.xy,
        a.xz + b.xz,
        a.yx + b.yx,
        a.yy + b.yy,
        a.yz + b.yz,
        a.zx + b.zx,
        a.zy + b.zy,
        a.zz + b.zz
    };
    return m;
}

egMat3 egMat3Mul( egMat3 a, egMat3 b )
{
    egMat3 m = {
        a.xx * b.xx + a.xy * b.yx + a.xz * b.zx,//xx
        a.xx * b.xy + a.xy * b.yy + a.xz * b.zy,//xy
        a.xx * b.xz + a.xy * b.yz + a.xz * b.zz,//xz

        a.yx * b.xx + a.yy * b.yx + a.yz * b.zx,//yx
        a.yx * b.xy + a.yy * b.yy + a.yz * b.zy,//yy
        a.yx * b.xz + a.yy * b.yz + a.yz * b.zz,//yz

        a.zx * b.xx + a.zy * b.yx + a.zz * b.zx,//zx
        a.zx * b.xy + a.zy * b.yy + a.zz * b.zy,//zy
        a.zx * b.xz + a.zy * b.yz + a.zz * b.zz //zz
    };
    return m;
}

egMat3 egMat3Sub( egMat3 a, egMat3 b )
{
    egMat3 m = {
        a.xx - b.xx,
        a.xy - b.xy,
        a.xz - b.xz,
        a.yx - b.yx,
        a.yy - b.yy,
        a.yz - b.yz,
        a.zx - b.zx,
        a.zy - b.zy,
        a.zz - b.zz
    };
    return m;
}

void egMat3Print( egMat3 m )
{
    printf( "mat3\n" );
    float * mp = ( float* )&m;
    for ( int i = 0; i < 3; ++i ) {
        for ( int j = 0; j < 3; ++j ) {
            printf( "\t%f ", *( mp + j + 3*i ));
        }
        printf( "\n" );
    }
}

//mat4 matrix functions

egMat4 egMat4Add( egMat4 a, egMat4 b )
{
    egMat4 m = {
        a.xx + b.xx,
        a.xy + b.xy,
        a.xz + b.xz,
        a.xw + b.xw,
        a.yx + b.yx,
        a.yy + b.yy,
        a.yz + b.yz,
        a.yw + b.yw,
        a.zx + b.zx,
        a.zy + b.zy,
        a.zz + b.zz,
        a.zw + b.zw,
        a.wx + b.wx,
        a.wy + b.wy,
        a.wz + b.wz,
        a.ww + b.ww
    };
    return m;
}

egMat4 egMat4Mul( egMat4 a, egMat4 b )
{
    egMat4 m = {
        a.xx * b.xx + a.xy * b.yx + a.xz * b.zx + a.xw * b.wx,//xx
        a.xx * b.xy + a.xy * b.yy + a.xz * b.zy + a.xw * b.wy,//xy
        a.xx * b.xz + a.xy * b.yz + a.xz * b.zz + a.xw * b.wz,//xz
        a.xx * b.xw + a.xy * b.yw + a.xz * b.zw + a.xw * b.ww,//xw
        a.yx * b.xx + a.yy * b.yx + a.yz * b.zx + a.yw * b.wx,//yx
        a.yx * b.xy + a.yy * b.yy + a.yz * b.zy + a.yw * b.wy,//yy
        a.yx * b.xz + a.yy * b.yz + a.yz * b.zz + a.yw * b.wz,//yz
        a.yx * b.xw + a.yy * b.yw + a.yz * b.zw + a.yw * b.ww,//yw
        a.zx * b.xx + a.zy * b.yx + a.zz * b.zx + a.zw * b.wx,//zx
        a.zx * b.xy + a.zy * b.yy + a.zz * b.zy + a.zw * b.wy,//zy
        a.zx * b.xz + a.zy * b.yz + a.zz * b.zz + a.zw * b.wz,//zz
        a.zx * b.xw + a.zy * b.yw + a.zz * b.zw + a.zw * b.ww,//zw
        a.wx * b.xx + a.wy * b.yx + a.wz * b.zx + a.ww * b.wx,//wx
        a.wx * b.xy + a.wy * b.yy + a.wz * b.zy + a.ww * b.wy,//wy
        a.wx * b.xz + a.wy * b.yz + a.wz * b.zz + a.ww * b.wz,//wz
        a.wx * b.xw + a.wy * b.yw + a.wz * b.zw + a.ww * b.ww //ww
    };
    return m;
}

egMat4 egMat4Sub( egMat4 a, egMat4 b )
{
    egMat4 m = {
        a.xx - b.xx,
        a.xy - b.xy,
        a.xz - b.xz,
        a.xw - b.xw,
        a.yx - b.yx,
        a.yy - b.yy,
        a.yz - b.yz,
        a.yw - b.yw,
        a.zx - b.zx,
        a.zy - b.zy,
        a.zz - b.zz,
        a.zw - b.zw,
        a.wx - b.wx,
        a.wy - b.wy,
        a.wz - b.wz,
        a.ww - b.ww
    };
    return m;
}

void egMat4TransVec3( egMat4 a, egV3 v, egV3 * out )
{
    out->x = ( a.xx * v.x ) + ( a.xy * v.y ) + ( a.xz * v.z ) + a.xw;
    out->y = ( a.yx * v.x ) + ( a.yy * v.y ) + ( a.yz * v.z ) + a.yw;
    out->z = ( a.zx * v.x ) + ( a.zy * v.y ) + ( a.zz * v.z ) + a.zw;
}

void egMat4TransVec3A( egMat4 a, egV3 * v, egV3 * out, size_t count )
{
    egV3 * va, *outa;
    for ( size_t i = 0; i < count; ++i ) {
        va = v + i;
        outa = out + i;
        outa->x = ( a.xx * va->x ) + ( a.xy * va->y ) + ( a.xz * va->z ) + a.xw;
        outa->y = ( a.yx * va->x ) + ( a.yy * va->y ) + ( a.yz * va->z ) + a.yw;
        outa->z = ( a.zx * va->x ) + ( a.zy * va->y ) + ( a.zz * va->z ) + a.zw;
    }
}

egMat4	egMat4Move( egMat4 a, egV3 v )
{
    a.xw += v.x;
    a.yw += v.y;
    a.zw += v.z;
    return a;
}

egMat4	egMat4Scale( egMat4 a, egV3 v )
{
    egMat4 sm;
    memcpy( &sm, &egMat4Id, sizeof( egMat4 ));
    sm.xx = v.x;
    sm.yy = v.y;
    sm.zz = v.z;
    return egMat4Mul( a, sm );
}

void egMat4Transpose(egMat4 * a)
{
    float sf;

    //xy
    sf = a->xy;
    a->xy = a->yx;
    a->yx = sf;

    //xz
    sf = a->xz;
    a->xz = a->zx;
    a->zx = sf;

    //xw
    sf = a->xw;
    a->xw = a->wx;
    a->wx = sf;

    //yz
    sf = a->yz;
    a->yz = a->zy;
    a->zy = sf;

    //yw
    sf = a->yw;
    a->yw = a->wy;
    a->wy = sf;

    //zw
    sf = a->zw;
    a->zw = a->wz;
    a->wz = sf;
}


void egMat4Print( egMat4 m )
{
    printf( "mat4\n" );
    float * mp = ( float* )&m;
    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
            printf( "\t%f ", *( mp + j + 4*i ));
        }
        printf( "\n" );
    }
}
