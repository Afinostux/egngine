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
#include "util/array.h"
#include "util/sort.h"
#include "util/endian.h"
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL_endian.h>


// -------      sorting functions
int eg_insertion_sort(void *data[], size_t n, int (*comp)(void*, void*))
{
    long i, b;
    void* temp;
    for (i = 1; i < n; ++i) {
        b = i - 1;
        temp = data[i];
        //printf("sorting %d:\n",*(int*)temp);
        while (b >= 0 && comp(data[b], temp) < 0) {
            data[b+1] = data[b];
            //printf("%u\t%d\n", b , *(int*)temp);
            --b;
        }
        data[b+1] = temp;
    }
    return 0;
}

// -------       array functions


//hidden helper
int eg_vec_insert_helper(egVec * v)
{
    if (v->element_count == v->alloc_size) {
        //if we can't double the buffer size, try adding one or fail
        if (!eg_vec_resize(v, (v->alloc_size)?v->alloc_size * 2:2))
            return eg_vec_resize(v, (v->alloc_size)?v->alloc_size + 1:2);
    }
    return 1;
}

egVec eg_vec_new(size_t alloc_size, size_t element_size)
{
    egVec v;
    v.elements = calloc(alloc_size, element_size);

    v.alloc_size = alloc_size;
    v.element_size = element_size;
    v.element_count = 0;

    return v;
}

//vector accessor
void * eg_vec_at(egVec* v, size_t element)
{
    if (element >= v->element_count || v->elements == 0) {
        return NULL;
    }
    char * d = v->elements;
    return &d[element * v->element_size];
}

int eg_vec_pop(egVec* v, void * element)
{
    if (v->element_count == 0 || v->elements == 0) {
        //element = 0;
        return 0;
    }
    char * d = v->elements;

    memcpy(element, &d[(--(v->element_count) * v->element_size)], v->element_size);
    return 1;
}

//vector insertion
int eg_vec_push(egVec* v, void * element)
{
    if (!eg_vec_insert_helper(v)) {
        return 0;
    }
    ++v->element_count;
    memcpy(eg_vec_at(v,v->element_count - 1),element, v->element_size);
    return v->element_count - 1;
}

int eg_vec_insert(egVec* v, void * element, size_t position)
{
    if (!eg_vec_insert_helper(v)) {
        return 0;
    }
    //check to make sure we're hitting the array
    void* insertpoint = eg_vec_at(v,position);
    ++v->element_count;//end-1 hack
    void* movepoint = eg_vec_at(v,position + 1);

    if (insertpoint == 0) {
        //trying to add outside the array, this should act as push

        //++v->element_count;
        memcpy(eg_vec_at(v,v->element_count - 1),element, v->element_size);

    } else {
        //sanity check in case of fucked up shit
        assert(movepoint > insertpoint);
        //clear a space
        memmove(movepoint, insertpoint, v->element_size * ((v->element_count - 1) - position));
        //++v->element_count;
        memcpy(insertpoint, element, v->element_size);
    }

    return 1;
}

//vector maintainance
int eg_vec_shrinkwrap(egVec* v)
{
    if (v->alloc_size == v->element_count) {
        return 1;
    }

    eg_vec_resize(v, v->element_count);
    return (v->elements)?1:0;
}
int eg_vec_resize(egVec* v, size_t new_allocsize)
{
    if (new_allocsize < v->element_count) {
        v->element_count = new_allocsize;
    }
    //printf("resizing to %u elements; %u bytes\n",new_allocsize, new_allocsize * v->element_size);
    v->alloc_size = new_allocsize;
    v->elements = realloc(v->elements, v->alloc_size * v->element_size);
    return (v->elements)?1:0;
}

int eg_vec_erase(egVec* v, size_t position)
{
    void * p = eg_vec_at(v, position);
    void * movep = eg_vec_at(v, position + 1);
    if (p) {
        //erase a thing!
        if (movep) {
            //run it over with the end of the array!
            memmove(p,movep,v->element_size * (v->element_count - (position + 1)));

        }
        --v->element_count;
    }
    //nothing to erase now!
    return position - 1;
}

int eg_vec_clear(egVec* v)
{
    v->element_count = 0;
    return 1;
}

int eg_vec_free(egVec* v)
{
    int e = eg_vec_resize(v,0);
    v->elements = 0;
    return e;
}

// --------- map functions


//map constructor
egMap eg_map_new(size_t alloc_size, size_t key_size, size_t value_size, int (*kcomp)(const void * a, const void * b))
{
    egMap m;
    m.kcomp = kcomp;
    m.keys = eg_vec_new(alloc_size, key_size);
    m.values = eg_vec_new(alloc_size, value_size);
    return m;
}

//map accessors
int eg_map_at(egMap * m, size_t element, const void ** key, void ** value)
{
    int rc = 0;
    if (key) {
        *key = eg_vec_at(&(m->keys),element);
        rc |= (intptr_t)*key;
    }

    if (value) {
        *value = eg_vec_at(&(m->values),element);
        rc |= (intptr_t)*value;
    }

    return rc?1:0;
}

void * eg_map_get(egMap * m, const void * key, size_t * el)
{
    size_t left = 0, right = m->keys.element_count, center;
    void * data;
    while (left < right) {
        center = (left + right) / 2;
        //printf("%u - %u - %u\n",left, center, right);
        data = eg_vec_at(&(m->keys),center);
        //printf("%s vs %s\n", (char*)key, (char*)data);
        assert(data);
        int comp = m->kcomp(key,data);

        if (comp == 0) {
            if (el)
                *el = center;
            return eg_vec_at(&(m->values),center);
        } else if (comp > 0) {
            left = center + 1;
        } else {
            right = center;
        }
    }
    if (el)
        *el = -1;
    return 0;
}

//map insertion
void * eg_map_insert(egMap * m, const void * key, void * value)
{
    size_t index;
    //printf("%s inserted\n", (char*)key);
    //get to a point where we're no longer greater than that index
    for (index = 0; index < m->keys.element_count && m->kcomp(key, eg_vec_at(&(m->keys),index)) > 0; ++index) {
    }
    //insert to that index
    eg_vec_insert(&(m->keys), key, index);
    eg_vec_insert(&(m->values), value, index);
    return eg_vec_at(&(m->values), index);
}

//map maintainance
int eg_map_erase(egMap * m, const void * key)
{
    size_t element;
    if (!eg_map_get(m,key,&element))
        return 0;
    eg_map_erase_at(m,element);
    return 1;
}

int eg_map_erase_at(egMap * m, size_t element)
{
    if (element >= m->keys.element_count)
        return 0;
    eg_vec_erase(&(m->keys), element);
    eg_vec_erase(&(m->values), element);
    return 1;
}

void eg_map_clear(egMap * m)
{
    eg_vec_clear(&(m->keys));
    eg_vec_clear(&(m->values));
}
void eg_map_free(egMap * m)
{
    eg_vec_free(&(m->keys));
    eg_vec_free(&(m->values));
}


