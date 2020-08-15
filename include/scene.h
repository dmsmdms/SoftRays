#pragma once

#include <object.h>
#include <malloc.h>

typedef struct {
    sr_vec_t camera_position;
    sr_vec_t camera_vector;
    sr_vec_t height_vector;
    sr_vec_t width_vector;
    sr_obj_t * objects;
    sr_obj_t ** light_sources;
    sr_color_t  ambient_light;
    uint32_t objects_quantiy;
    uint16_t light_sources_quantiy;
    float    time;
} sr_scene_t;

void sr_scene_create(register sr_scene_t * restrict scene);
void sr_scene_update(register sr_scene_t * restrict scene);
void sr_scene_destroy(const register sr_scene_t * restrict scene);

__m128 sr_comp_light(const register sr_scene_t * restrict scene, const register sr_obj_t * object,
                     const register __m128 object_point, const register __m128 object_normal,
                     register __m128 color);
