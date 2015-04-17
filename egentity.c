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
#include "egentity.h"
#include "util/egmath.h"

#include "model.h"
#include "egrenderer.h"



egMemPool entityPool = 0;


unsigned int egEntNew(egV3 position, egQuat rotation, char model[16], char texture[16])
{
    if (entityPool == 0) {

        egMemPoolNew(&entityPool, sizeof(egEntity), 16);
    }

    //printf("new entity %s %s\n", model, texture);

    size_t id;
    egEntity * e;
    //printf("initializing entity\n");
    egMemPoolAlloc(entityPool, (void*)&e, &id);

    e->model = egModelGet(model);
    e->texid = egRendererGetTexid(texture);
    e->position = position;
    e->rotation = rotation;


    return id;
}

void egEntErase(unsigned int id)
{
    egMemPoolErase(entityPool, id);
}

egEntity * egEntGet(unsigned int id)
{
    egEntity * ent = 0;
    egMemPoolGetP(entityPool, (void*)&ent, id);
    return ent;
}

egMemPool egEntPool(void)
{
    return entityPool;
}
