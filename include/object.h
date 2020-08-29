#pragma once

#include <math.h>

typedef enum {
    SR_OBJ_TYPE_SPHERE,
} SR_PACKED sr_obj_type_t;

typedef union {
    struct {
        sr_vec_t center;
        float radius_2;
    } sphere;
} sr_form_t;

typedef union {
    __m128 sse;
    struct {
        float red;
        float green;
        float blue;
        float alpha;
    };
} sr_color_t;

typedef struct {
    sr_color_t    color;
    sr_form_t     form;
    float         reflection;
    sr_obj_type_t type;
    uint8_t       is_light_source : 1;
} sr_obj_t;

typedef struct {
    const sr_obj_t * object;
    float root;
} sr_intersection_t;

__m128 sr_get_obj_normal(const register sr_obj_t * restrict object, register __m128 point);
void sr_comp_objects_intersection(register sr_intersection_t * restrict intersection, const register sr_obj_t * restrict objects,
                                  const register __m128 ray_vec, const register __m128 start_point,
                                  register uint32_t objects_quantity, const register float root_min);
