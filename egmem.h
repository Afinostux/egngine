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
#include <string.h>

void egMemInit(void);
void egMemDeInit(void);

typedef struct egMemArrayData {
    int8_t * data;
    size_t object_size;
    size_t object_count;
    size_t buffer_size;
    size_t id;
} egMemArrayData;

typedef egMemArrayData * egMemArray;

//new mem array for object_size sized objects, initially containing space for object_count of them
void	egMemArrayNew(egMemArray *m, size_t object_size, size_t object_count);
void	egMemArrayFree(egMemArray m);
void	egMemArrayClear(egMemArray m);

//copy the value pointed to by data to the back of m, allocate space for it if needed
void	egMemArrayPush(egMemArray m, const void * data);

//pop the back of m as a stack, return 0 if empty, copies returned value to data on success. shrinks the array by one.
int		egMemArrayPop(egMemArray m, void * data);

//array style retrieval. return 0 on failure. Copy value to data on success
int		egMemArrayGet(egMemArray m, void * data, size_t index);

//return the address of the element at index.
void	egMemArrayGetP(egMemArray m, void ** data, size_t index);

//delete the contents of the array at index and shrink the array to remain contiguous
void	egMemArrayErase(egMemArray m, size_t index);

//on success, data points to the first element in an array of count uninitialized objects. on failure, it points to 0.
void	egMemArrayAlloc(egMemArray m, void ** data, size_t count);

//return 1 if this array will be automatically cleaned up
int		egMemArrayManaged(egMemArray m);

size_t	egMemArrayCount(egMemArray m);

int8_t*	egMemArrayPointer(egMemArray m);

void	egMemArrayResize(egMemArray m, size_t object_count);

void	egMemArrayBack(egMemArray m, void ** data);


typedef struct egMemPoolData {
    egMemArray data, usage, recycle;
    size_t next_id, id;
} egMemPoolData;

typedef egMemPoolData * egMemPool;

void	egMemPoolNew(egMemPool *p, size_t object_size, size_t object_count);
void	egMemPoolFree(egMemPool p);
void	egMemPoolClear(egMemPool p);

void	egMemPoolAlloc(egMemPool p, void ** data, size_t * id);

int32_t	egMemPoolGet(egMemPool p, void * data, size_t id);
void	egMemPoolGetP(egMemPool p, void ** data, size_t id);

void	egMemPoolErase(egMemPool p, size_t id);

int		egMemPoolManaged(egMemPool p);

size_t	egMemPoolCount(egMemPool p);

size_t	egMemPoolFirst(egMemPool p);

//returns the data at id, or null when at the end. increments id to the next object.
int8_t*	egMemPoolNext(egMemPool p, size_t * id);


