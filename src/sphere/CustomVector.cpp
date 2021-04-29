
#include "CustomVector.h"
#include "SphereTypes.h"



sphere::Vector::Vector(VectorVal x, VectorVal y, VectorVal z) : x(x), y(y), z(z) {}

sphere::Vector sphere::Vector::operator+(const Vector &a) {
    return Vector(x + a.x, x + a.y, z + a.z);
}

sphere::Vector sphere::Vector::operator-(const Vector &a) {
    return Vector(x - a.x, y-a.y, z-a.z);
}

sphere::Vector sphere::Vector::absVal() {
    return Vector(abs(x), abs(y), abs(z));
}

sphere::Vector sphere::Vector::compMax(const Vector &a) {
    return Vector(std::max(x,a.x), std::max(y,a.y), std::max(z, a.z));
}

sphere::Vector sphere::Vector::compMin(const Vector &a) {
    return Vector(std::min(x,a.x), std::min(y,a.y), std::min(z, a.z));

}

sphere::VectorVal sphere::Vector::length() {
    return sqrt(x*x + y*y + z*z);
}

sphere::VectorVal sphere::Vector::distance(const Vector &a) {
    return (*this - a).length();
}

sphere::Vector sphere::Vector::normalize() {
    VectorVal l = length();
    return Vector(x/l, y/l, z/l);
}