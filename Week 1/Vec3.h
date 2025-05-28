#include <iostream>
#include <cmath>
using namespace std;

class Vec3{
    float v[3];
    public:
    Vec3() {
        v[0] = 0; v[1] =0; v[2] =0;
    }
    Vec3(float x, float y, float z){
        v[0] = x; v[1] = y; v[2] = z;
    }
    Vec3 operator+ (const Vec3 other) const {
        return Vec3(v[0]+ other.v[0],v[1]+ other.v[1],v[2]+ other.v[2]);
    }
    Vec3 operator- (const Vec3 other) const {
        return Vec3(v[0]- other.v[0],v[1]- other.v[1],v[2]- other.v[2]);
    }
    Vec3 operator* (const Vec3 other) const {
        return Vec3(v[0]* other.v[0],v[1]* other.v[1],v[2]* other.v[2]);
    }
    Vec3 operator* (const float a) const {
        return Vec3(v[0]* a,v[1]*a,v[2]*a);
    }
    Vec3 operator/ (const Vec3 other) const {
        return Vec3(v[0]/ other.v[0],v[1]/ other.v[1],v[2]/ other.v[2]);
    }
    Vec3 operator- () const {
        return Vec3 (-v[0], -v[1] , -v[2]);
    }
    float operator[] (int i) const {
        return v[i];
    }
    float length() const {
        return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    }
    Vec3 unit() const {
        float l = length();
        return Vec3 (v[0]/l,v[1]/l, v[2]/l);
    }
    float dot (const Vec3& other ) const {
        return (v[0]*other.v[0]+v[1]*other.v[1]+v[2]*other.v[2]);
    }
    Vec3 cross (const Vec3& other ) const {
        return Vec3 (v[1]*other.v[2] - v[2]*other.v[1],v[2]*other.v[0] - v[0]*other.v[2],v[0]*other.v[1] - v[1]*other.v[0]);
    }
    Vec3 reflect (Vec3& normal) const {
        return (*this - normal.unit()*(2*(*this).dot(normal.unit()))).unit();
    }
    Vec3 refract (Vec3& normal, float refractive_index) const {
        Vec3 i = this->unit();
        Vec3 n = normal.unit();
        float cosi = max(-1.0f,min(1.0f,i.dot(n)));
        float eta = refractive_index;
        if (cosi > 0)
        {
            n = -n;
            eta = 1/eta;
            cosi*=-1;
        }
        float k = (1/eta)*(1/eta)*(1- cosi*cosi);
        if (k>1) return reflect(n);
        float eta_r = 1/eta;
        return (i*eta_r + n*(eta_r*cosi - sqrt(k))).unit();
    }
};