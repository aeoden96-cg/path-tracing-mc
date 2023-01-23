//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "onb.h"
#include "pdf.h"

class sphere: public hitable  {
    public:
        sphere() {}
        sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m)  {};
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        virtual float  pdf_value(const vec3& o, const vec3& v) const;
        virtual vec3 random(const vec3& o) const;
        vec3 center;
        float radius;
        material *mat_ptr;
};

float sphere::pdf_value(const vec3& o, const vec3& v) const {
    hit_record rec;
    if (this->hit(ray(o.to_glm(), v.to_glm()), 0.001, FLT_MAX, rec)) {
        float cos_theta_max = sqrt(1 - radius*radius/(center-o).squared_length());
        float solid_angle = 2*M_PI*(1-cos_theta_max);
        return  1 / solid_angle;
    }
    else
        return 0;
}

vec3 sphere::random(const vec3& o) const {
     vec3 direction = center - o;
     float distance_squared = direction.squared_length();
     onb uvw;
     uvw.build_from_w(direction);
     return uvw.local(random_to_sphere(radius, distance_squared));
}


bool sphere::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center.to_glm() - vec3(radius, radius, radius).to_glm(), center.to_glm()+ vec3(radius, radius, radius).to_glm());
    return true;
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = toVec3(r.origin()) - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, toVec3(r.direction()));
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(b*b-a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            get_sphere_uv(((rec.p-center.to_glm())/radius), rec.u, rec.v);
            rec.normal = (rec.p - center.to_glm()) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(b*b-a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            get_sphere_uv(((rec.p-center.to_glm())/radius), rec.u, rec.v);
            rec.normal = (rec.p - center.to_glm()) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}


#endif

