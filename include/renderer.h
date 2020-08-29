#pragma once

#include <scene.h>
#include <thread.h>

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} sr_pixel_t;

typedef struct {
    sr_pixel_t * pixels;
    uint32_t     pixels_size;
    uint16_t     height;
    uint16_t     width;
    sr_scene_t   scene;
} sr_renderer_t;

void sr_renderer_create(register sr_renderer_t * restrict renderer, register const uint16_t width,
                        register const uint16_t height);
void sr_renderer_draw(register sr_renderer_t * restrict renderer);
void sr_renderer_set_sizes(register sr_renderer_t * restrict renderer, register const uint16_t width,
                           register const uint16_t height);
void sr_renderer_destroy(register sr_renderer_t * restrict renderer);
