#include "ray.h"
#include "hitable.h"

#ifdef _WIN32
	#define srand48(x) srand((int)(x))
	#define drand48() ((double)rand()/(RAND_MAX + 1.0))
#endif
vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(drand48(), drand48() ,drand48()) - vec3(1.0, 1.0, 1.0);
	} while (p.squared_length() >= 1.0);
	return p;
}

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertion : public material {
public:
    lambertion(const vec3& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
    vec3 albedo;
};

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

class metal : public material {
public:
    metal(const vec3& a, float f)
        : albedo(a)
        , fuzz(f) {
        if (f < 0.0f) {
            fuzz = 0;
        } else if (f > 1.0f) {
            fuzz = 1.0;
        } else {
            fuzz = f;
        }
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return dot(rec.normal, reflected) > 0.0f;
    }
    vec3 albedo;
    float fuzz;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    // vec3 r_out_perp = ni_over_nt * (uv - n * dt);
    // vec3 r_out_parallel = -sqrt(1.0f - dot(r_out_perp, r_out_perp)) * n;
    // float discriminate = 1.0f - ni_over_nt * ni_over_nt * (1.0f + dt * dt - 2 * dt * dt);
    float discriminate = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminate > 0.0f) {
        refracted = ni_over_nt * (uv - n * dt) - sqrt(discriminate) * n;
        return true;
    }
    return false;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
    r0 *= r0;
    return r0 + (1.0f - r0) * powf(1.0f - cosine, 5.0f);
}

class dielectric : public material {
public:
    dielectric(float ri)
        : ref_idx(ri) {
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        attenuation = vec3(1.0f, 1.0f, 1.0f);

        vec3 outward_normal;
        float ni_over_nt;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0.0f) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }

        vec3 refracted;
        float reflect_prob;
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            reflect_prob = schlick(cosine, ref_idx);
        } else {
            reflect_prob = 1.0f;
        }
        if (drand48() < reflect_prob) {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected);
        } else {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }
    float ref_idx;
};
