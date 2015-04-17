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
#include "egcollision.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>

egMemPool colliders = 0;

void egCollidersDeInit(void)
{
    //
}

uint32_t egColliderCount()
{
    return egMemPoolCount(colliders);
}

egMemPool egColliderPool()
{
    return colliders;
}

void egCollidersInit(void)
{
    atexit(egCollidersDeInit);
    egMemPoolNew(&colliders, sizeof(egCollider), 16);
}

uint32_t egColliderNew(float x, float y, float w, float h, uint32_t type, uint16_t (*collision)(egCollider *, egCollider *), void * userdata, uint32_t oid)
{
    size_t id;
    egCollider * c;
    egMemPoolAlloc(colliders, (void*)&c, &id);


    c->position = egV2N(x, y);
    c->width = w * 0.5;
    c->height = h * 0.5;
    c->type = type;
    c->id = id;
    c->userdata = userdata;
    c->collision = collision;
    c->oid = oid;
    c->active = 1;

    //printf("new collider created: %u\n next new id:%u\n", id, nextNewColliderID);

    return id;
}

egCollider * egColliderGet(uint32_t id)
{
    egCollider * redirect;
    egMemPoolGetP(colliders, (void*)&redirect, id);
    return redirect;
}

void egColliderErase(uint32_t id)
{
    egMemPoolErase(colliders, id);
}

void egColliderDeactivate(uint32_t id)
{
    egColliderGet(id)->active = 0;
}

void egColliderActivate(uint32_t id)
{
    egColliderGet(id)->active = 1;
}

void egCollidersTick(void)
{

    size_t id1 = egMemPoolFirst(colliders), id2;
    egCollider *cur = (egCollider*)egMemPoolNext(colliders, &id1);

    while (cur) {
        id2 = id1;
        egCollider *cmp = (egCollider*)egMemPoolNext(colliders, &id2);
        while(cmp) {
            if (cur->active && cmp->active &&
                    fabs(cur->position.x - cmp->position.x) < (cur->width + cmp->width) &&
                    fabs(cur->position.y - cmp->position.y) < (cur->height + cmp->height)) {
                if (cur->collision) {
                    cur->collision(cur, cmp);
                }

                if (cmp->collision) {
                    cmp->collision(cmp, cur);
                }
            }
            cmp = (egCollider*)egMemPoolNext(colliders, &id2);
        }
        cur = (egCollider*)egMemPoolNext(colliders, &id1);
    }
}
