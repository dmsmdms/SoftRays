#include <object.h>

SR_INLINE __m128 sr_get_obj_normal(const register sr_obj_t * restrict object, register __m128 point) {
    switch (object->type) {
        case SR_OBJ_TYPE_SPHERE: {
            point = _mm_sub_ps(point, object->form.sphere.center.sse);
            point = sr_vec_normalize(point, sr_vec_length(point));
        } break;
    }

    return point;
}

void sr_comp_objects_intersection(register sr_intersection_t * restrict intersection, const register sr_obj_t * restrict objects,
                                  const register __m128 ray_vec, const register __m128 start_point,
                                  register uint32_t objects_quantity, const register float root_min) {
    register float ray_vec_2 = sr_vec_scalar_mul(ray_vec, ray_vec);

    for (register float closest_root = __FLT_MAX__; objects_quantity--; objects++) {
        switch (objects->type) {
            case SR_OBJ_TYPE_SPHERE: {
                register __m128 OC_vec = _mm_sub_ps(objects->form.sphere.center.sse, start_point);
                register float B = sr_vec_scalar_mul(OC_vec, ray_vec);
                register float D = B * B - ray_vec_2 * (sr_vec_scalar_mul(OC_vec, OC_vec) - objects->form.sphere.radius_2);

                if (D >= 0) {
                    B = (B - sr_sqrt(D)) / ray_vec_2;

                    if (B > root_min && B < closest_root) {
                        intersection->root = closest_root = B;
                        intersection->object = objects;
                    }
                }
            } continue;
        }
    }
}
