#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {

    point3 org;
    vec3 dir;

    public:
    ray () {}

    ray (const point3& origin, const vec3& direction) : org(origin), dir(direction) {}
    const point3& origin() const  { return org; }
    const vec3& direction() const { return dir; }

    point3 at(double t) const {
        return org + t*dir;
    }
};

#endif 