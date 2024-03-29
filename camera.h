#pragma once

#include <math.h>
#include "ray.h"

#ifdef _WIN32
	#define srand48(x) srand((int)(x))
	#define drand48() ((double)rand()/(RAND_MAX + 1.0))
#endif
vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0f * vec3(drand48(), drand48(), 0.0f) - vec3(1.0f, 1.0f, 0.0f);
	} while (dot(p, p) >= 1.0f);
	return p;
}

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
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
	    origin = lookfrom;
		lower_left_corner = origin - half_width * u - half_height * v - w;
	    horizontal = 2.0f * half_width * u;
	    vertical = 2.0f * half_height * v;
    }

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
		lens_radius = aperture / 2;
		float theta = vfov * M_PI / 180.0f;
		float half_height = tan(theta / 2) * focus_dist;
		float half_width = aspect * half_height;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
	    origin = lookfrom;
		lower_left_corner = origin - half_width * u - half_height * v - focus_dist * w;
	    horizontal = 2.0f * half_width * u;
	    vertical = 2.0f * half_height * v;
    }

    ray get_ray(float s, float t) {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 origin;
	vec3 u, v, w;
	float lens_radius{};
};
