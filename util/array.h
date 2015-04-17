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

//vector handle
typedef struct egVec egVec;
typedef struct egMap egMap;
struct egVec {
    void * elements;
    size_t element_size;
    size_t element_count;
    size_t alloc_size;
};

struct egMap {
    egVec keys;
    egVec values;
    int (*kcomp)(const void * a, const void * b);
};

//vector constructor
egVec eg_vec_new(size_t element_count, size_t element_size);

//vector accessor
void * eg_vec_at(egVec* v, size_t element);
int eg_vec_pop(egVec* v, void * element);

//vector insertion
int eg_vec_push(egVec* v, void * element );
int eg_vec_insert(egVec* v, void * element, size_t position);

//vector maintainance
int eg_vec_shrinkwrap(egVec* v);
int eg_vec_resize(egVec* v, size_t new_allocsize);
int eg_vec_erase(egVec* v, size_t position);
int eg_vec_clear(egVec* v);
int eg_vec_free(egVec* v);

//map constructor
egMap eg_map_new(size_t alloc_size, size_t key_size, size_t value_size, int (*kcomp)(const void * a, const void * b));

//map accessors
int eg_map_at(egMap * m, size_t element, const void ** key, void ** value);
void * eg_map_get(egMap * m, const void * key, size_t * el);

//map insertion
void * eg_map_insert(egMap * m, const void * key, void * value);

//map maintainance
int eg_map_erase(egMap * m, const void * key);
int eg_map_erase_at(egMap * m, size_t element);
void eg_map_clear(egMap * m);
void eg_map_free(egMap * m);

