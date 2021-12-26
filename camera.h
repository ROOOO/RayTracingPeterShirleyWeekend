#pragma once

#include <math.h>
#include "ray.h"

class camera {
public:
    camera() {
	    lower_left_corner = vec3(-2.0, -1.0, -1.0);
	    horizontal = vec3(4.0, 0.0, 0.0);
	    vertical = vec3(0.0, 2.0, 0.0);
	    origin = vec3(0.0, 0.0, 0.0);
    }

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
		float theta = vfov * M_PI / 180.0f;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		vec3 w = unit_vector(lookfrom - lookat);
		vec3 u = unit_vector(cross(vup, w));
		vec3 v = cross(w, u);
	    origin = lookfrom;
		lower_left_corner = origin - half_width * u - half_height * v - w;
	    horizontal = 2.0f * half_width * u;
	    vertical = 2.0f * half_height * v;
    }

    ray get_ray(float u, float v) {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 origin;
};
