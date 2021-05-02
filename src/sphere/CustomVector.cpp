/**
 * Copyright (c) 2021, André Gaillard, Christopher Raffl, Jens Eirik Saethre,
 * David Spielmann. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * @file CustomVector.cpp
 * 
 * @brief implementation of custom vector and Color classes
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 30.04.2021
 */

#include <math.h>
#include <ostream>

#include "SphereTypes.h"
#include "CustomVector.h"

/********************************** Vector ***********************************/

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
sphere::Vector sphere::Vector::operator+(const Vector &a) const
{
    return Vector(x + a.x, y + a.y, z + a.z);
}

/**
 * @brief Component wise vector subtraction
 * 
 * @param a Vector 
 * @return result of component wise subtraction
 */
sphere::Vector sphere::Vector::operator-(const Vector &a) const
{
    return Vector(x - a.x, y-a.y, z-a.z);
}

/**
 * @brief adds a vector and assigns result to this vector
 * 
 * @param a vector to add to this vector
 * @return result of component wise addition
 */
sphere::Vector& sphere::Vector::operator+=(const Vector &a)
{
    *this = *this + a;
    return *this;
}

/**
 * @brief subtracts a vector from this vector and assigns result to it
 * 
 * @param a vector 
 * @return sphere::Vector& 
 */
sphere::Vector& sphere::Vector::operator-=(const Vector &a)
{
    *this = *this - a;
    return *this;
}

/**
 * @brief dot product overload for multiplication
 * 
 * @param a vector to compute dot product with
 * @return the dot product value
 */
sphere::ftype sphere::Vector::operator*(const Vector &a) const
{
    return x * a.x + y * a.y + z * a.z;
}

/**
 * @brief scalar multiplication overload for multiplication operator
 * 
 * @param a the scalar to be multiplied with the vector
 * @return the result vector, scaled by the argument
 */
sphere::Vector sphere::Vector::operator*(const ftype &a) const
{
    return Vector(x * a, y * a, z * a);
}

/**
 * @brief converts a Vector to a Color object
 * @return a color object
 */
sphere::Vector::operator sphere::Color() const
{
    return Color(
        static_cast<ColorVal>(x),
        static_cast<ColorVal>(y),
        static_cast<ColorVal>(z)
    );
}

/**
 * @brief Component wise absolut value
 * 
 * @return result of component wise absolut value
 */
sphere::Vector sphere::Vector::absVal() const
{
    return Vector(abs(x), abs(y), abs(z));
}

/**
 * @brief Component wise max
 * 
 * @param a Vector
 * @return Result of component wise maximum 
 */
sphere::Vector sphere::Vector::componentwiseMax(const Vector &a) const
{
    return Vector(std::max(x,a.x), std::max(y,a.y), std::max(z, a.z));
}

/**
 * @brief Component wise min
 * 
 * @param a Vector
 * @return Result of component wise minimum 
 */
sphere::Vector sphere::Vector::componentwiseMin(const Vector &a) const
{
    return Vector(std::min(x,a.x), std::min(y,a.y), std::min(z, a.z));
}

/**
 * @brief Length of a vector
 * 
 * @return vector length 
 */
sphere::VectorVal sphere::Vector::length() const
{
    return sqrt(x*x + y*y + z*z);
}

/**
 * @brief Distance between two vectors
 * 
 * @param a 
 * @return Distance
 */
sphere::VectorVal sphere::Vector::distance(const Vector &a) const
{
    return (*this - a).length();
}

/**
 * @brief Normalize Vector
 * 
 * @return normalized vector
 */
sphere::Vector sphere::Vector::normalize() const
{
    VectorVal l = length();
    return Vector(x/l, y/l, z/l);
}

/**
 * @brief Returns the largest vector component
 * 
 * @return the largest vector component
 */
sphere::VectorVal sphere::Vector::maxComponent() const
{
    return std::max(std::max(x,y),z);
}

/**
 * @brief Returns the smallest vector component
 * 
 * @return the smallest vector component
 */
sphere::VectorVal sphere::Vector::minComponent() const
{
    return std::min(std::min(x,y),z);
}

/**
 * @brief Rotates a vector by a given rotationMatrix
 * 
 * @param rotationMatrix defines the rotation, size 9
 * @return sphere::Vector rotated vector
 */
sphere::Vector sphere::Vector::rotate(ftype rotationMatrix[]) const
{
    VectorVal r_x = rotationMatrix[0] * x + rotationMatrix[1] * y + rotationMatrix[2] * z;
    VectorVal r_y = rotationMatrix[3] * x + rotationMatrix[4] * y + rotationMatrix[5] * z;
    VectorVal r_z = rotationMatrix[6] * x + rotationMatrix[7] * y + rotationMatrix[8] * z;
    return sphere::Vector(r_x, r_y, r_z);
}

/********************************** Color ***********************************/

/**
 * @brief constructs a color object that represents black (all zeros)
 */
sphere::Color::Color()
 : r(0.0), g(0.0), b(0.0)
{
    // nothing to do here
}

/**
 * @brief constructs a new color object with given rgb value
 * 
 * @param r the value of the red component, in [0,1]
 * @param g the value of the green component, in [0,1]
 * @param b the value of the blue component, in [0,1]
 */
sphere::Color::Color(ColorVal r, ColorVal g, ColorVal b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

/**
 * @brief simple addition of two color values
 * 
 * @param other the color to add
 * @return a new color value in [0,1]
 */
sphere::Color sphere::Color::operator+(const Color &other)
{
    return Color(
        std::min(this->r + other.r, 1.0f),
        std::min(this->g + other.g, 1.0f),
        std::min(this->b + other.b, 1.0f)
    );
}

/**
 * @brief add and assign operation of two colors
 * 
 * @param other the color to add
 * @return a new color value in [0,1]
 */
sphere::Color& sphere::Color::operator+=(const Color &other)
{
    *this = *this + other;
    return *this;
}

/**
 * @brief overloads the stream operator to print a Color object
 * 
 * @param out the output stream
 * @param col the color to be printed
 * @return reference to the stream that produced the output
 */
std::ostream& sphere::operator<<(std::ostream &out, Color const &col)
{
    out << "(" << col.r << "," << col.g << "," << col.b << ")";
    return out;
}

/**
 * @brief overloads the stream operator to print a Vector object
 * 
 * @param out the output stream
 * @param vec the vector to be printed
 * @return reference to the stream that produced the output
 */
std::ostream& sphere::operator<<(std::ostream &out, Vector const &vec)
{
    out << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
    return out;
}