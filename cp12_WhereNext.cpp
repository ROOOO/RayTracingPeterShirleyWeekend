#include <iostream>
#include <cfloat>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

vec3 color(const ray& r, hitable* world, int depth = 0) {
	hit_record rec;
	if (world->hit(r, 0.0001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 100 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		return vec3(0, 0, 0);
	} 
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

hitable* random_scene() {
	int n = 500;
	hitable** list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, new lambertion(vec3(0.5f, 0.5f, 0.5f)));
	int i = 1;
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			float choose_mat = drand48();
			vec3 center(a + 0.9f * drand48(), 0.2f, b + 0.9f * drand48());
			if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
				if (choose_mat < 0.8f) {
					list[i++] = new sphere(center, 0.2f, new lambertion(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48())));
				} else if (choose_mat < 0.95f) {
					list[i++] = new sphere(center, 0.2f, new metal(vec3(0.5 * (1.0f + drand48()), 0.5 * (1.0f + drand48()), 0.5f * (1.0f + drand48())), 0.5f * drand48()));
				} else {
					list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0.0f, 1.0f, 0.0f), 1.0f, new dielectric(1.5f));
	list[i++] = new sphere(vec3(-4.0f, 1.0f, 0.0f), 1.0f, new lambertion(vec3(0.4f, 0.2f, 0.1f)));
	list[i++] = new sphere(vec3(4.0f, 1.0f, 0.0f), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
	return new hitable_list(list, i);
}

int main() {
	int nx = 360;
	int ny = 360;
	int ns = 100;
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	hitable* world = random_scene();
	vec3 lookfrom(4.0f, 3.0f, 3.0f);
	vec3 lookat(0.0f, 0.0f, -1.0f);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 2.0f;
	camera cam(lookfrom, lookat, vec3(0.0f, 1.0f, 0.0f), 90.0f, float(nx) / float(ny));
	for (int j = ny - 1; j >= 0; --j) {
		for (int i = 0; i < nx; ++i) {
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; ++s) {
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
}
