#include <scene.h>

inline __m128 sr_comp_light(const register sr_scene_t * restrict scene, const register sr_obj_t * object,
                            const register __m128 object_point, const register __m128 object_normal,
                            register __m128 color) {
    color = _mm_add_ps(color, scene->ambient_light.sse);

    switch (object->type) {
        case SR_OBJ_TYPE_SPHERE: {
            for (register uint16_t i = 0; i < scene->light_sources_quantiy; i++) {
                register sr_obj_t * light_obj = scene->light_sources[i];
                register __m128 light_ray = _mm_sub_ps(object_point, light_obj->form.sphere.center.sse);

                sr_intersection_t intersection = { 0, 0 };
                sr_comp_objects_intersection(&intersection, scene->objects, light_ray,
                    light_obj->form.sphere.center.sse, scene->objects_quantiy, SR_FLOAT_EPS);

                if (intersection.object != NULL && intersection.root < 1 - SR_FLOAT_EPS) {
                    continue;
                }

                register float intensity = sr_vec_scalar_mul(light_ray, light_ray);
                intensity = -sr_vec_scalar_mul(object_normal, light_ray) / (intensity * intensity);
                intensity = sr_isru(intensity);

                if (intensity > 0) {
                    color = _mm_add_ps(color, _mm_mul_ps(light_obj->color.sse, _mm_set_ps1(intensity * (1 - object->reflection))));
                }
            }
        } break;
    }

    return _mm_mul_ps(color, object->color.sse);
}

inline void sr_scene_create(register sr_scene_t * restrict scene) {
    scene->objects_quantiy = 5;
    scene->light_sources_quantiy = 2;

    void * memory = _mm_malloc(scene->objects_quantiy * sizeof(sr_obj_t) + scene->light_sources_quantiy * sizeof(sr_obj_t **), 16);

    scene->objects = memory;
    memory = (sr_obj_t *)memory + scene->objects_quantiy;

    scene->objects[0] = (const sr_obj_t) {
        .color = { .red = 1, .green = 1, .blue = 1, .alpha = 1 },
        .type = SR_OBJ_TYPE_SPHERE,
        .reflection = 0,
        .form.sphere = {
            .center = { .x = 0, .y = 3, .z = 0 },
            .radius_2 = 1,
        },
    };

    scene->objects[1] = (const sr_obj_t) {
        .color = { .red = 1, .green = 0.2, .blue = 0.1, .alpha = 1 },
        .type = SR_OBJ_TYPE_SPHERE,
        .is_light_source = 1,
        .reflection = 0,
        .form.sphere = {
            .center = { .x = 0, .y = 1.5, .z = 0 },
            .radius_2 = 0.1,
        },
    };

    scene->objects[2] = (const sr_obj_t) {
        .color = { .red = 1, .green = 1, .blue = 1, .alpha = 1 },
        .type = SR_OBJ_TYPE_SPHERE,
        .reflection = 0.6,
        .form.sphere = {
            .center = { .x = 0, .y = 0, .z = 0 },
            .radius_2 = 1,
        },
    };

    scene->objects[3] = (const sr_obj_t) {
        .color = { .red = 0.2, .green = 1, .blue = 0.1, .alpha = 1 },
        .type = SR_OBJ_TYPE_SPHERE,
        .is_light_source = 1,
        .reflection = 0,
        .form.sphere = {
            .center = { .x = 0, .y = -1.5, .z = 0 },
            .radius_2 = 0.1,
        },
    };

    scene->objects[4] = (const sr_obj_t) {
        .color = { .red = 1, .green = 1, .blue = 1, .alpha = 1 },
        .type = SR_OBJ_TYPE_SPHERE,
        .reflection = 0,
        .form.sphere = {
            .center = { .x = 0, .y = -3, .z = 0 },
            .radius_2 = 1,
        },
    };

    scene->light_sources = memory;
    memory = (sr_obj_t **)memory + scene->light_sources_quantiy;

    scene->light_sources[0] = scene->objects + 1;
    scene->light_sources[1] = scene->objects + 3;

    scene->ambient_light = (const sr_color_t) { .red = 0.05, .green = 0.05, .blue = 0.05, .alpha = 1 };

    scene->camera_vector   = (const sr_vec_t) { .x = 0, .y =  0, .z = -1 };
    scene->camera_position = (const sr_vec_t) { .x = 0, .y =  0, .z =  0 };
    scene->width_vector    = (const sr_vec_t) { .x = 1, .y =  0, .z =  0 };
    scene->height_vector   = (const sr_vec_t) { .x = 0, .y = -1, .z =  0 };
}

inline void sr_scene_update(register sr_scene_t * restrict scene) {
    register float k = 1;

    for (register uint8_t i = 0; i < scene->objects_quantiy; k += 0.2, i++) {
        sr_trig_t trig = sr_trig(k * scene->time);
        scene->objects[i].form.sphere.center.x = trig.cos;
        scene->objects[i].form.sphere.center.z = trig.sin - 9;
    }

    scene->time += 0.02;
}

void sr_scene_destroy(const register sr_scene_t * restrict scene) {
    free(scene->objects);
}
