#include <renderer.h>

#ifndef SR_RECURSION_DEPTH
    #define SR_RECURSION_DEPTH 1
#endif

#ifndef SR_THREAD_QUANTITY
    #define SR_THREAD_QUANTITY 4
#endif

inline void sr_renderer_create(register sr_renderer_t * restrict renderer, register const uint16_t width,
                               register const uint16_t height) {
    renderer->width = width;
    renderer->height = height;
    renderer->pixels_size = width * height * sizeof(sr_pixel_t);

    sr_scene_create(&renderer->scene);
}

inline void sr_renderer_set_sizes(register sr_renderer_t * restrict renderer, register const uint16_t width,
                                  register const uint16_t height) {
    renderer->width = width;
    renderer->height = height;
    renderer->pixels_size = width * height * sizeof(sr_pixel_t);
}

SR_INLINE sr_pixel_t sr_color_normalize(register __m128 pixel_color) {
    union {
        __m64 sse;
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
    } result_color;

    result_color.sse = _mm_packs_pu16(_mm_cvtps_pi16(_mm_mul_ps(pixel_color, _mm_set_ps1(255))), _mm_setzero_si64());

    return (sr_pixel_t) {
        .red = result_color.red,
        .green = result_color.green,
        .blue = result_color.blue,
        .alpha = result_color.alpha,
    };
}

__m128 sr_trace_ray(register sr_renderer_t * restrict renderer, register __m128 ray_vec,
                    register __m128 start_point, register float min_root,
                    register uint8_t recursion_depth) {
    sr_intersection_t intersection = { 0, 0 };
    sr_comp_objects_intersection(&intersection, renderer->scene.objects,
        ray_vec, start_point, renderer->scene.objects_quantiy, min_root);

    if (intersection.object != NULL) {
        if (intersection.object->is_light_source) {
            return intersection.object->color.sse;
        }

        register __m128 object_normal;
        start_point = _mm_add_ps(_mm_mul_ps(ray_vec, _mm_set_ps1(intersection.root)), start_point);
        object_normal = sr_get_obj_normal(intersection.object, start_point);
        ray_vec = sr_vec_normalize(ray_vec, sr_vec_length(ray_vec));
        ray_vec = _mm_add_ps(object_normal, ray_vec);

        register __m128 color = _mm_setzero_ps();

        if (intersection.object->reflection > SR_FLOAT_EPS && recursion_depth < SR_RECURSION_DEPTH) {
            color = sr_trace_ray(renderer, ray_vec, start_point, SR_FLOAT_EPS, recursion_depth + 1);
            color = _mm_mul_ps(color, _mm_set_ps1(intersection.object->reflection));
        }

        return sr_comp_light(&renderer->scene, intersection.object, start_point, object_normal, color);
    } else {
        return _mm_setzero_ps();
    }
}

inline void sr_renderer_draw(register sr_renderer_t * restrict renderer) {
    register sr_pixel_t * restrict pixels = renderer->pixels;

    for (register uint16_t y = 0; y < renderer->height; y++) {
        register __m128 y_bias_vec = _mm_set_ps1((float) y / renderer->height - 0.5);

        y_bias_vec = _mm_mul_ps(y_bias_vec, renderer->scene.height_vector.sse);
        y_bias_vec = _mm_add_ps(y_bias_vec, renderer->scene.camera_vector.sse);

        for (register uint16_t x = 0; x < renderer->width; x++) {
            register __m128 ray_vec = _mm_set_ps1((float) x / renderer->width - 0.5);

            ray_vec = _mm_mul_ps(ray_vec, renderer->scene.width_vector.sse);
            ray_vec = _mm_add_ps(ray_vec, y_bias_vec);

            *pixels++ = sr_color_normalize(sr_trace_ray(renderer, ray_vec, renderer->scene.camera_position.sse, 1, 0));
        }
    }

    sr_scene_update(&renderer->scene);
}

inline void sr_renderer_destroy(register sr_renderer_t * restrict renderer) {
    sr_scene_destroy(&renderer->scene);
}
