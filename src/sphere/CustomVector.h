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
 * @file customVector.h
 * 
 * @brief implements a custom vector
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 25.04.2021
 * 
 */

#ifndef CUSTOM_VECTOR_H
#define CUSTOM_VECTOR_H

#include <math.h>
#include <ostream>
#include <algorithm>
#include <tuple>

#include "SphereTypes.h"
#include "SphereBenchmark.h"

namespace sphere {

/**
 * @brief type that stores a color in RGB format, where each color is
 * a floating point value in [0,1] indicating the intensity of each 
 * component.
 */
class Color {

public:
    // public member fields
    ColorVal r; //!< the color's red component
    ColorVal g; //!< the color's green component
    ColorVal b; //!< the color's blue component
    uint8_t _pad[PAD_COLOR]; //!< auxiliary padding for cache performance

    /****************************** Constructors *****************************/

    /**
     * @brief Construct a black color object
     */
    Color() : r(0.0), g(0.0), b(0.0) {}
    
    /**
     * @brief constructs a color object that represents black (all zeros)
     */
    Color(ColorVal r, ColorVal g, ColorVal b) : r(r), g(g), b(b) {}

    /************************** Operator Overloading *************************/

    /**
     * @brief simple addition of two color values
     * 
     * @param other the color to add
     * @return a new color value in [0,1]
     */
    Color operator+(const Color &other)
    {
        COUNT_OPS(3);
        return Color(
            std::min(this->r + other.r, 1.0f),
            std::min(this->g + other.g, 1.0f),
            std::min(this->b + other.b, 1.0f)
        );
    }

    /**
     * @brief add and assign operation of two colors
     * @param other the color to add
     * @return a new color value in [0,1]
     */
    Color& operator+=(const Color &other)
    {
        COUNT_OPS(0); // already accounted for by operator+
        *this = *this + other;
        return *this;
    }

    /**
     * @brief mutliply color with a scalar value
     * @param other scalar to multiply the color with
     * @return a new color value in [0,1]
     */
    Color operator*(const ftype &other)
    {
        return Color(
            std::clamp(this->r * other, 0.0, 1.0),
            std::clamp(this->g * other, 0.0, 1.0),
            std::clamp(this->b * other, 0.0, 1.0)
        );
    }

    /**
     * @brief compares two colors and returns true if they are equal
     * 
     * @param other the other color to compare with
     * @return true if r,g,b values are the same for both colors
     * @return false else
     */
    bool operator==(const Color &other) const
    {
        return (r == other.r && g == other.g && b == other.b);
    }
};

/**
 * @brief type that stores a 3d vector of integers
 */
class Vector {

public:
    VectorVal x; // the vector's x-coordinate
    VectorVal y; // the vector's y-coordinate
    VectorVal z; // the vector's z-coordinate
    uint8_t _pad[PAD_VECTOR]; // auxiliary padding for cache performance

    /****************************** Constructors *****************************/

    /**
     * @brief Constructs a new zero vector 
     */
    Vector() : x(0), y(0), z(0) {};

    /**
     * @brief Construct a new sphere::Vector::Vector object
     * 
     * @param x x Coordinate
     * @param y y Coordinate
     * @param z z Coordinate
     */
    Vector(VectorVal x, VectorVal y, VectorVal z) :  x(x), y(y), z(z) {}

    /************************* Operator Overloading **************************/

    /**
     * @brief Component wise vector addition
     * 
     * @param a Vector
     * @return result of component wise addition
     */
    Vector operator+(const Vector &a) const
    {
        COUNT_OPS(3);
        return Vector(x + a.x, y + a.y, z + a.z);
    }

    /**
     * @brief Scalar addition to vector
     * 
     * @param a float
     * @return result of scalar addition 
     */
    Vector operator+(const ftype &a) const 
    {
        COUNT_OPS(3);
        return Vector(x + a, y + a, z + a);
    }

    /**
     * @brief Component wise vector subtraction
     * 
     * @param a Vector 
     * @return result of component wise subtraction
     */
    Vector operator-(const Vector &a) const
    {
        COUNT_OPS(3);
        return Vector(x - a.x, y-a.y, z-a.z);
    }

    /**
     * @brief Scalar subtraction from vector
     * 
     * @param a float
     * @return result of scalar subtraction
     */
    Vector operator-(const ftype &a) const
    {
        COUNT_OPS(3);
        return Vector(x - a, y - a, z - a);
    }

    /**
     * @brief dot product overload for multiplication
     * 
     * @param a vector to compute dot product with
     * @return the dot product value
     */
    ftype operator*(const Vector &a) const
    {
        COUNT_OPS(5);
        return x * a.x + y * a.y + z * a.z;
    }

    /**
     * @brief scalar multiplication overload for multiplication operator
     * 
     * @param a the scalar to be multiplied with the vector
     * @return the result vector, scaled by the argument
     */
    Vector operator*(const ftype &a) const
    {
        COUNT_OPS(3);
        return Vector(x * a, y * a, z * a);
    }

    /**
     * @brief adds a vector and assigns result to this vector
     * 
     * @param a vector to add to this vector
     * @return result of component wise addition
     */
    Vector& operator+=(const Vector &a)
    {
        COUNT_OPS(0); // ops are counted in + operator
        *this = *this + a;
        return *this;
    }

    /**
     * @brief subtracts a vector from this vector and assigns result to it
     * 
     * @param a vector to subtract
     * @return reference to the result vector (this object)
     */
    Vector& operator-=(const Vector &a)
    {
        COUNT_OPS(0); // ops are counted in - operator
        *this = *this - a;
        return *this;
    }

    /************************* Conversion Functions **************************/

    /**
     * @brief converts a Vector to a Color object
     * @return a color object
     * 
     * @todo does casting count as a flop?
     */
    operator Color() const 
    {
        COUNT_OPS(0);
        return Color(
            static_cast<ColorVal>(x),
            static_cast<ColorVal>(y),
            static_cast<ColorVal>(z)
        );
    }

    /************************** Auxiliary Functions **************************/
    
    /**
     * @brief Component wise absolut value
     * 
     * @return result of component wise absolut value
     */
    Vector absVal() const
    {
        COUNT_OPS(3);
        return Vector(std::fabs(x), std::fabs(y), std::fabs(z));
    }

    /**
     * @brief Component wise max
     * 
     * @param a Vector
     * @return Result of component wise maximum 
     */
    Vector componentwiseMax(const Vector &a) const
    {
        COUNT_OPS(0);
        return Vector(std::max(x,a.x), std::max(y,a.y), std::max(z, a.z));
    }

    /**
     * @brief Component wise min
     * 
     * @param a Vector
     * @return Result of component wise minimum 
     */
    Vector componentwiseMin(const Vector &a) const
    {
        COUNT_OPS(0);
        return Vector(std::min(x,a.x), std::min(y,a.y), std::min(z, a.z));
    }

    /**
     * @brief Length of a vector
     * @return vector length 
     */
    VectorVal length() const
    {
        COUNT_OPS(25); // assumes 20 flops for sqrt
        return sqrt(x*x + y*y + z*z);
    }

    /**
     * @brief computes the distance between two vectors
     * @param a the other vector
     * @return the distance
     */
    VectorVal distance(const Vector &a) const
    {
        COUNT_OPS(0); // already accounted in operator- and length()
        return (*this - a).length();
    }

    /**
     * @brief returns a normalized version of this vector
     * @return the normalized vector
     */    
    Vector normalize() const
    {
        COUNT_OPS(3); // length() already accounted for in function call
        VectorVal l = length();
        return Vector(x/l, y/l, z/l);        
    }

    /**
     * @brief Returns the largest vector component
     * @return the largest vector component
     */
    VectorVal maxComponent() const
    {
        COUNT_OPS(0);
        return std::max(std::max(x,y),z);        
    }

    /**
     * @brief Returns the smallest vector component
     * @return the smallest vector component
     */    
    VectorVal minComponent() const
    {
        COUNT_OPS(0);
        return std::min(std::min(x,y),z);        
    }

    /**
     * @brief Rotates a vector by a given rotationMatrix
     * @param rotationMatrix defines the rotation, size 9
     * @return sphere::Vector rotated vector
     */
    Vector rotate(ftype rotationMatrix[]) const
    {
        COUNT_OPS(15);
        VectorVal r_x = rotationMatrix[0] * x + rotationMatrix[1] * y + rotationMatrix[2] * z;
        VectorVal r_y = rotationMatrix[3] * x + rotationMatrix[4] * y + rotationMatrix[5] * z;
        VectorVal r_z = rotationMatrix[6] * x + rotationMatrix[7] * y + rotationMatrix[8] * z;
        return sphere::Vector(r_x, r_y, r_z);        
    }

    /**
     * @brief calculates in which axis the vector has its biggest expansion 
     * and returns false for this and true for the other two axes
     * 
     * @return std::tuple<bool, bool, bool> false for the axis with the biggest expansion,
     * true for the other two
     */
    std::tuple<bool, bool, bool> shadowAxes() const
    {
        if (std::fabs(x) > std::fabs(y) && std::fabs(x) > std::fabs(z)){
            return std::make_tuple(false, true, true);
        }
        if (std::fabs(y) > std::fabs(z)){
            return std::make_tuple(true, false, true);
        } else {
            return std::make_tuple(true, true, false);
        }
    }
};

/**
 * @brief class for a two-dimensional vector
 */
class Vector2 {

public:
    VectorVal x; //!< the vector's x-coordinate
    VectorVal y; //!< the vector's y-coordinate
    uint8_t _pad[PAD_2DVECT]; //!< auxiliary padding for cache performance

    /****************************** Constructors *****************************/
    
    /**
     * @brief creates a 2d zero vector
     */
    Vector2() : x(0), y(0) {}

    /**
     * @brief creates a new 2d vector
     * 
     * @param x the vector's x component
     * @param y the vector's y component
     */
    Vector2(VectorVal x, VectorVal y) : x(x), y(y) {}

    /************************* Operator Overloading **************************/

    /**
     * @brief overloads the + operator for vector addition
     * @param a the vector to add
     * @return the result vector 
     */
    Vector2 operator+(const Vector2 &a) const
    {
        COUNT_OPS(2);
        return Vector2(x + a.x, y + a.y);
    }

    /**
     * @brief overloads the - operator for vector subtraction
     * @param a the vector to subtract
     * @return the result vector
     */
    Vector2 operator-(const Vector2 &a) const
    {
        COUNT_OPS(2);
        return Vector2(x - a.x, y - a.y);
    }

    /**
     * @brief overloads the * operator to the dot product
     * @param a the vector to compute the dot product with
     * @return the dot product
     */
    ftype operator*(const Vector2 &a) const
    {
        COUNT_OPS(3);
        return x * a.x + y * a.y;
    }

    /**
     * @brief overloads the * operator to a scalar multiplication
     * @param a the scalar to compute the product with
     * @return the result vector
     */
    Vector2 operator*(const ftype &a) const
    {
        COUNT_OPS(2);
        return Vector2(a * x, a * y);
    }

    /************************** Auxiliary Functions **************************/
    
    /**
     * @brief computes the absolute value of the vector components
     * @return the vector with the absolute values
     */
    Vector2 absVal() const
    {
        COUNT_OPS(2);
        return Vector2(std::fabs(x), std::fabs(y));
    }

    /**
     * @brief computes the length of a 2d vector
     * @return the vector's length
     */
    ftype length() const 
    {
        COUNT_OPS(23); // assume 20 flops for sqrt
        return std::sqrt(x*x + y*y);
    }

    /**
     * @brief returns the normalized vector
     * @return the normalized vector
     */
    Vector2 normalize() const
    {
        COUNT_OPS(2); // length already accounted for in length()
        ftype l = length();
        return Vector2(x/l, y/l);
    }

    /**
     * @brief returns the maximum component of the vector
     * @return the max component
     */
    VectorVal maxComponent() const
    {
        COUNT_OPS(0);
        return std::max(x, y);
    }

    /**
     * @brief returns the minimum component of the vector
     * @return the min component
     */
    VectorVal minComponent() const
    {
        COUNT_OPS(0);
        return std::min(x, y);
    }
};

/****************************** Stream Overloading ***************************/

/**
 * @brief overloads the stream operator to print a Color object
 * 
 * @param out the output stream
 * @param col the color to be printed
 * @return reference to the stream that produced the output
 */
inline std::ostream& operator<<(std::ostream &out, Color const &col)
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
inline std::ostream& operator<<(std::ostream &out, Vector const &vec)
{
    out << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
    return out;
}

/**
 * @brief overloads the stream operator to print a Vector2 object
 * 
 * @param out the output stream
 * @param vec the vector to be printed
 * @return reference to the stream that produced the output
 */
inline std::ostream& operator<<(std::ostream &out, Vector2 const &vec)
{
    out << "(" << vec.x << "," << vec.y << ")";
    return out;
}

} // namespace sphere

#endif //CUSTOM_VECTOR_H
