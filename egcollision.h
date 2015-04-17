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
#include <stdint.h>
#include "util/egmath.h"
#include "egmem.h"


typedef struct egCollider {
    egV2 position;
    float width, height;
    uint32_t type, id, oid;
    void * userdata;
    uint16_t (*collision)(struct egCollider *, struct egCollider *);
    uint16_t active;
} egCollider;

void egCollidersInit(void);

uint32_t egColliderCount();
egMemPool egColliderPool();

uint32_t egColliderNew(float x, float y, float w, float h, uint32_t type, uint16_t (*collision)(egCollider *, egCollider *), void * userdata, uint32_t oid);
egCollider * egColliderGet(uint32_t id);
void egColliderErase(uint32_t id);

void egColliderDeactivate(uint32_t id);
void egColliderActivate(uint32_t id);

void egCollidersTick(void);
