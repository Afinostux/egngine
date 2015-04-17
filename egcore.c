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
#include "egcore.h"
#include "util/array.h"
#include <SDL2/SDL.h>
#include "model.h"
#include "egrenderer.h"
#include "util/egmath.h"
#include "egcollision.h"
#include "egmem.h"

short EG_RUNNING = 1;
unsigned int framedelay = 16;
egMap * buttons;
egMap buttonContexts;

size_t nextbutton = 0;

typedef struct egButtonMapping {
    int button;
    int modetrigger;
    void * data;
    void (*key)(void*);
} egButtonMapping;

void egCoreSetFrameDelay(unsigned int delay)
{
    framedelay = delay;
}

void egControlModeCreate(const char name[16])
{
    egMap m = eg_map_new(0, sizeof(int), sizeof(egButtonMapping), iCmp);
    buttons = eg_map_insert(&buttonContexts, name, &m);
    //buttons = eg_vec_at(&buttonContexts.values,ofs);
}

void egControlModeSet(const char name[16])
{
    buttons = eg_map_get(&buttonContexts, name, 0);
}

size_t egButtonPressed(int button, void (*key)(void*), void * data)
{
    egButtonMapping m;
    m.button = button;
    m.modetrigger = 0;
    m.data = data;
    m.key = key;

    eg_map_insert(buttons, &nextbutton, &m);
    return nextbutton++;
}

size_t egButtonReleased(int button, void (*key)(void*), void * data)
{
    egButtonMapping m;
    m.button = button;
    m.modetrigger = 1;
    m.data = data;
    m.key = key;

    eg_map_insert(buttons, &nextbutton, &m);
    return nextbutton++;
}

int egCSstrcmp(const void * a, const void * b){
	return strcmp( (const char*)a, (const char*)b);
}

void egCoreStart(void)
{
    egMemInit();
    eg_initmodels();
    egCollidersInit();
    buttonContexts = eg_map_new(1, sizeof(char) * 16, sizeof(egMap), egCSstrcmp);
}

void egCoreTick(void)
{
    SDL_Event e;
    egButtonMapping * bmap;
    while (SDL_PollEvent(&e)) {
        //process events
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
                EG_RUNNING = 0;
            }
        }
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            //correct category
            for (int i = 0; i < buttons->keys.element_count; ++i) {
                //check if there's a control for it
                bmap = (egButtonMapping*)(buttons->values.elements) + i;
                if (bmap->button == e.key.keysym.sym) {
                    //filter for keydown state
                    if (e.type == SDL_KEYDOWN) {
                        if (bmap->modetrigger == 0) {
                            bmap->key(bmap->data);
                        }
                    } else {
                        if (bmap->modetrigger == 1) {
                            bmap->key(bmap->data);
                        }
                    }
                }
            }
        }
    }

    egCollidersTick();
    egRendererRender();

    SDL_Delay(framedelay);
}

void egCoreEnd(void)
{
    for (int i = 0; i < buttonContexts.values.element_count; ++i) {
        eg_map_free((egMap*)(buttonContexts.values.elements) + i);
    }
    eg_map_free(&buttonContexts);
    eg_shutdownmodels();
    egMemDeInit();
}
