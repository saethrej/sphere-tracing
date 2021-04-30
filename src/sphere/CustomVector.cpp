
#include "CustomVector.h"
#include "SphereTypes.h"
#include "cmath"

/**
 * @brief Construct a new sphere::Vector::Vector object
 * 
 * @param x x Coordinate
 * @param y y Coordinate
 * @param z z Coordinate
 */
sphere::Vector::Vector(VectorVal x, VectorVal y, VectorVal z) : x(x), y(y), z(z) {}


/**
 * @brief Component wise vector addition
 * 
 * @param a Vector
 * @return result of component wise addition
 */
sphere::Vector sphere::Vector::operator+(const Vector &a) {
    return Vector(x + a.x, x + a.y, z + a.z);
}

/**
 * @brief Component wise vector subtraction
 * 
 * @param a Vector 
 * @return result of component wise subtraction
 */
sphere::Vector sphere::Vector::operator-(const Vector &a) {
    return Vector(x - a.x, y-a.y, z-a.z);
}

sphere::ftype sphere::Vector::operator*(const Vector &a) 
{
    return x * a.x + y * a.y + z * a.z;
}

sphere::Vector sphere::Vector::operator*(const ftype &a) 
{
    return Vector(x * a, y * a, z * a);
}


/**
 * @brief Component wise absolut value
 * 
 * @return result of component wise absolut value
 */
sphere::Vector sphere::Vector::absVal() {
    return Vector(abs(x), abs(y), abs(z));
}

/**
 * @brief Component wise max
 * 
 * @param a Vector
 * @return Result of component wise maximum 
 */
sphere::Vector sphere::Vector::componentwiseMax(const Vector &a) {
    return Vector(std::max(x,a.x), std::max(y,a.y), std::max(z, a.z));
}

/**
 * @brief Component wise min
 * 
 * @param a Vector
 * @return Result of component wise minimum 
 */
sphere::Vector sphere::Vector::componentwiseMin(const Vector &a) {
    return Vector(std::min(x,a.x), std::min(y,a.y), std::min(z, a.z));

}

/**
 * @brief Length of a vector
 * 
 * @return vector length 
 */
sphere::VectorVal sphere::Vector::length() {
    return sqrt(x*x + y*y + z*z);
}

/**
 * @brief Distance between two vectors
 * 
 * @param a 
 * @return Distance
 */
sphere::VectorVal sphere::Vector::distance(const Vector &a) {
    return (*this - a).length();
}

/**
 * @brief Normalize Vector
 * 
 * @return normalized vector
 */
sphere::Vector sphere::Vector::normalize() {
    VectorVal l = length();
    return Vector(x/l, y/l, z/l);
}

/**
 * @brief Returns the largest vector component
 * 
 * @return the largest vector component
 */
sphere::VectorVal sphere::Vector::maxComponent(){
    return std::max(std::max(x,y),z);
}

/**
 * @brief Returns the smallest vector component
 * 
 * @return the smallest vector component
 */
sphere::VectorVal sphere::Vector::minComponent(){
    return std::min(std::min(x,y),z);
}

/**
 * @brief Rotates a vector by a given rotationMatrix
 * 
 * @param rotationMatrix defines the rotation, size 9
 * @return sphere::Vector rotated vector
 */
sphere::Vector sphere::Vector::rotate(ftype rotationMatrix[])
{
    VectorVal r_x = rotationMatrix[0] * x + rotationMatrix[1] * y + rotationMatrix[2] * z;
    VectorVal r_y = rotationMatrix[3] * x + rotationMatrix[4] * y + rotationMatrix[5] * z;
    VectorVal r_z = rotationMatrix[6] * x + rotationMatrix[7] * y + rotationMatrix[8] * z;
    return sphere::Vector(r_x, r_y, r_z);
}