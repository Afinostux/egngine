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
#include "egmem.h"
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>

egMemPool memArrays = 0, memPools = 0;

void egMemDeInit(void)
{

    size_t id = egMemPoolFirst(memArrays);
    egMemArray a = (egMemArray)egMemPoolNext(memArrays, &id);
    while(a) {
        //printf("freeing id %u\n", a->id);
        egMemArrayFree(a);
        a = (egMemArray)egMemPoolNext(memArrays, &id);
    }

    id = egMemPoolFirst(memPools);
    egMemPool p = (egMemPool)egMemPoolNext(memPools, &id);
    while(p) {
        //printf("freeing id %u\n", p->id);
        egMemPoolFree(p);
        p = (egMemPool)egMemPoolNext(memPools, &id);
    }

    egMemPoolFree(memArrays);
    egMemPoolFree(memPools);
}

void egMemInit(void)
{
    egMemPool a, b;
    egMemPoolNew(&a, sizeof(egMemArrayData), 16);
    egMemPoolNew(&b, sizeof(egMemPoolData), 16);
    memArrays = a;
    memPools = b;
    //atexit(egMemDeInit); there/s a bug!
}

//new mem array for object_size sized objects, initially containing space for object_count of them
void	egMemArrayNew(egMemArray * m, size_t object_size, size_t object_count)
{
    egMemArray a;
    size_t id;
    if (memArrays == 0) {
        a = malloc(sizeof(egMemArrayData));
        id = -1;
    } else {
        egMemPoolAlloc(memArrays, (void*)&a, &id);
    }
    //printf("making new memarray, size %u, objsize %u\n", object_count, object_size);
    a->data = malloc(object_size * object_count);
    a->object_size = object_size;
    a->object_count = 0;
    a->buffer_size = object_count * object_size;
    a->id = id;
    *m = a;
}
void	egMemArrayFree(egMemArray m)
{
    free(m->data);
    m->object_count = 0;
    m->object_size = 0;
    m->buffer_size = 0;
    if (egMemArrayManaged(m)) {
        egMemPoolErase(memArrays, m->id);
    } else {
        free(m);
    }
}

void	egMemArrayClear(egMemArray m)
{
    m->object_count = 0;
}

//copy the value pointed to by data to the back of m, allocate space for it if needed
void	egMemArrayPush(egMemArray m, const void * data)
{
    void * target;
    egMemArrayAlloc(m, &target, 1);
    memcpy(target, data, m->object_size);
}

//pop the back of m as a stack, return 0 if empty, copies returned value to data on success. shrinks the array by one.
int		egMemArrayPop(egMemArray m, void * data)
{
    void * redirect;
    if (m->object_count == 0) {
        return 0;
    }
    egMemArrayBack(m, &redirect);
    memcpy(data, redirect, m->object_size);
    --(m->object_count);
    return 1;
}

//array style retrieval. return 0 on failure. Copy value to data on success
int		egMemArrayGet(egMemArray m, void * data, size_t index)
{
    int8_t * dp;
    egMemArrayGetP(m, (void*)&dp, index);
    if (dp == 0) {
        //no result, fail!
        return 0;
    } else {
        //copy that shizz
        memcpy(data, dp, m->object_size);
        return 1;
    }
}

//return the address of the element at index.
void	egMemArrayGetP(egMemArray m, void ** data, size_t index)
{
    if (index < m->object_count) {
        *data = &(m->data)[index * m->object_size];
    } else {
        *data = 0;
    }
}

//delete the contents of the array at index and shrink the array to remain contiguous
void	egMemArrayErase(egMemArray m, size_t index)
{
    int8_t *item_p, *mov_p;
    size_t objects_to_move;
    egMemArrayGetP(m, (void*)&item_p, index);
    egMemArrayGetP(m, (void*)&mov_p, index + 1);
    if (item_p != 0) {
        //we've hit the array, continue
        if (mov_p != 0) {
            //item is in the middle of the array
            //determine the number of bytes until the end of the array,
            //then grab the back end of the array and move it over top of the deleted value
            objects_to_move = (m->object_count - (index + 1)) * m->object_size;
            memmove(item_p, mov_p, objects_to_move);
        }
        //if the object is not at the middle of the array, we can get rid of it by just decrementing. Standards!
        --(m->object_count);
    }
}

//on success, data points to the first element in an array of count uninitialized objects. on failure, it points to 0.
void	egMemArrayAlloc(egMemArray m, void ** data, size_t count)
{
    size_t first_alloc = m->object_count * m->object_size;
    egMemArrayResize(m, m->object_count + count);
    *data = &(m->data)[first_alloc];
}

//return 1 if this array will be automatically cleaned up
int		egMemArrayManaged(egMemArray m)
{
    if (m->id == (size_t)-1) {
        return 0;
    }
    return 1;
}

size_t	egMemArrayCount(egMemArray m)
{
    return m->object_count;
}

int8_t*	egMemArrayPointer(egMemArray m)
{
    return m->data;
}


void	egMemArrayResize(egMemArray m, size_t object_count)
{
    size_t nalloc = m->buffer_size;
    //printf("resizing array of %u bytes to fit %u objects of size %u\n", nalloc, object_count, m->object_size);
    while((nalloc / m->object_size) < object_count) {
        nalloc *= 2;
        if (nalloc > 100000) {
            egMemArrayData * doop = 0;
            doop->id -= 1;
        }
        //printf("\tresize to %u...\n", nalloc);
    }
    if (nalloc > m->buffer_size) {
        m->data = (int8_t*)realloc(m->data, nalloc);
        m->buffer_size = nalloc;
    }
    m->object_count = object_count;
}

void	egMemArrayBack(egMemArray m, void ** data)
{
    *data = &(m->data)[((m->object_count - 1) * m->object_size)];
}


void	egMemPoolNew(egMemPool *p, size_t object_size, size_t object_count)
{
    egMemPool a;
    size_t id;
    if (memPools == 0) {
        a = malloc(sizeof(egMemArrayData));
        id = -1;
    } else {
        egMemPoolAlloc(memPools, (void*)&a, &id);
    }
    egMemArrayNew(&(a->data), object_size, object_count);
    egMemArrayNew(&(a->usage), sizeof(uint8_t), object_count);
    egMemArrayNew(&(a->recycle), sizeof(size_t), object_count);

    a->next_id = 0;
    a->id = id;
    *p = a;
}

void	egMemPoolFree(egMemPool p)
{
    egMemArrayFree(p->data);
    egMemArrayFree(p->usage);
    egMemArrayFree(p->recycle);
    if (egMemPoolManaged(p)) {
        egMemPoolErase(memPools, p->id);
    } else {
        free(p);
    }
}

void	egMemPoolClear(egMemPool p)
{
    egMemArrayClear(p->data);
    egMemArrayClear(p->usage);
    egMemArrayClear(p->recycle);
    p->next_id = 0;
}

void	egMemPoolAlloc(egMemPool p, void ** data, size_t * id)
{
    assert(id);
    *id = p->next_id;
    void * redirect;
    uint8_t * usage_p = 0;
    if (!egMemArrayPop(p->recycle, id)) {
        ++(p->next_id);
    }

    if (*id >= p->data->object_count) {
        egMemArrayResize(p->data, *id + 1);
        egMemArrayResize(p->usage, *id + 1);
        p->data->object_count = *id + 1;
        p->usage->object_count = *id + 1;
    }

    egMemArrayGetP(p->usage, (void*)&usage_p, *id);
    //assert(usage_p);

    *usage_p = (uint8_t)1;

    egMemArrayGetP(p->data, &redirect, *id);
    assert(redirect);
    *data = redirect;
}

void	egMemPoolGetP(egMemPool p, void ** data, size_t id)
{
    uint8_t * usage_p;
    void * redirect = 0;

    egMemArrayGetP(p->usage, (void*)&usage_p, id);
    if (usage_p != 0 && *usage_p) {
        //we're in the array, and the id is in use
        egMemArrayGetP(p->data, &redirect, id);
    }

    *data = redirect;
}

void	egMemPoolErase(egMemPool p, size_t id)
{
    uint8_t * usage_p;
    egMemArrayGetP(p->usage, (void*)&usage_p, id);
    if (usage_p != 0 && *usage_p) {
        egMemArrayPush(p->recycle, &id);
        *usage_p = 0;
    }
}

int		egMemPoolManaged(egMemPool p)
{
    if (p->id == (size_t)-1) {
        return 0;
    }
    return 1;
}

size_t	egMemPoolCount(egMemPool p)
{
    //the total will be the number of unique ids that have been allocated minus the number that are available for reuse
    return p->next_id - egMemArrayCount(p->recycle);
}

size_t	egMemPoolFirst(egMemPool p)
{
    //return the first index in use
    size_t index = 0;
    uint8_t * usage_p;
    egMemArrayGetP(p->usage, (void*)&usage_p, index);
    //printf("looking for first index at %u\n", index);
    while(usage_p != 0 && *usage_p == 0) {
        ++index;
        //printf("\t...%u...\n", index);
        egMemArrayGetP(p->usage, (void*)&usage_p, index);
    }
    //printf("Settled on %u\n", index);
    return index;
}

//returns the data at id, or null when at the end. increments id to the next object.
int8_t*	egMemPoolNext(egMemPool p, size_t * id)
{
    assert(id);
    //printf("start ID %u\n", *id);

    if (*id >= p->next_id) {
        return 0;
    }

    void * redirect;
    uint8_t * usage_p;

    egMemPoolGetP(p, &redirect, *id);

    ++(*id);
    egMemArrayGetP(p->usage, (void*)&usage_p, *id);
    //printf("checking next id ID %u\n", *id);

    while(usage_p != 0 && *usage_p == 0 && *id < p->next_id) {
        ++(*id);
        //printf("checking next id ID %u\n", *id);
        egMemArrayGetP(p->usage, (void*)&usage_p, *id);
    }
    //printf("Settled on %u\n", *id);
    return redirect;
}
