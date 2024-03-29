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

#include "hittable.h"
#include "onb.h"
#include "pdf.h"

class sphere: public hittable  {
    public:
        sphere() {}
        sphere(glm::vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m)  {};
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        virtual float  pdf_value(const glm::vec3 &o, const vec3& v) const;
        virtual glm::vec3 random(const glm::vec3 &o) const;
        glm::vec3 center;
        float radius;
        material *mat_ptr;
};

float sphere::pdf_value(const glm::vec3 &o, const vec3& v) const {
    auto _o = o;
    auto _v = v.to_glm();

    hit_record rec;
    if (this->hit(ray(_o,_v), 0.001, FLT_MAX, rec)) {
        float cos_theta_max = sqrt(1 - radius*radius/glm::dot(center - _o,center - _o));
        float solid_angle = 2*M_PI*(1-cos_theta_max);
        return  1 / solid_angle;
    }
    else
        return 0;
}

glm::vec3 sphere::random(const glm::vec3 &o) const {
     vec3 direction = toVec3(center - o);
     float distance_squared = direction.squared_length();
     onb uvw;
     uvw.build_from_w(direction.to_glm());
     return uvw.local(random_to_sphere(radius, distance_squared).to_glm());
}


bool sphere::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center- vec3(radius, radius, radius).to_glm(), center+ vec3(radius, radius, radius).to_glm());
    return true;
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = toVec3(r.origin()) - toVec3(center);
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, toVec3(r.direction()));
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(b*b-a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            get_sphere_uv(((rec.p-center)/radius), rec.u, rec.v);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(b*b-a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            get_sphere_uv(((rec.p-center)/radius), rec.u, rec.v);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}


#endif

