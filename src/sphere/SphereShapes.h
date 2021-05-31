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
 * @file SphereShapes.h
 * 
 * @brief declaration of classes that define the shapes that need to be rendered.
 * There is one class, i.e. Shape, that all other classes declared within this
 * file derive from. Each derived class e.g. provides its own distance function.
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 26.04.2021
 */

#ifndef SPHERE_SHAPES_H
#define SPHERE_SHAPES_H

#include <string>
#include <iosfwd>
#include <json.hpp>

#include "SphereTypes.h"
#include "CustomVector.h"

using json = nlohmann::json;

namespace sphere {

/**
 * @brief enum class defining the different shapes we can render. Each
 * shape has exactly one of the types in this enum.
 */
enum class ShapeType {PLANE, BOX, SPHERE, TORUS, OCTAHEDRON, CONE, UNKNOWN};


/**
 * @brief abstract class that provides the template for the different shapes
 * to be rendered.
 */
class Shape
{
public:
    // constructors and destructor
    Shape(json const &params);
    ~Shape();

    // transate and rotate function
    Vector translate_rotate(Vector *pos);

    // distance function
    virtual ftype distanceFunction(Vector pos) = 0;
    virtual ftype distanceFunctionSquared(Vector pos) = 0;

    // static functions
    static ShapeType getShapeType(std::string shapeName);

    // public member variables shared by all shapes
    Vector position;  //!< the shape's position
    Vector rotation;  //!< the shape's rotation
    ftype reflection; //!< the shape's reflection coefficient
    ftype shininess;  //!< the shape's shininess value
    Color color;      //!< the shape's color (RGB)
    ShapeType type;   //!< the type of the shape
    std::string name; //!< the shape's name
    bool isRotated;   //!< indicates whether rotation is non-zero
    ftype inverseRotation[9]; //!< inverse rotation matrix
};

/**
 * @brief represents a plane object to be rendered
 */
class Plane : public Shape 
{
public:
    // constructor
    Plane(json const &plane);

    // distance function
    ftype distanceFunction(Vector pos);
    ftype distanceFunctionSquared(Vector pos);

    // additional public member fields
    Vector normal;
    ftype displacement;
};

/**
 * @brief represents a box object to be rendered
 */
class Box : public Shape
{
public:
    // constructor
    Box(json const &box);

    // distance function
    ftype distanceFunction(Vector pos);
    ftype distanceFunctionSquared(Vector pos);

    // additional public member fields
    Vector extents;
};

/**
 * @brief represents a sphere object to be rendered
 */
class Sphere : public Shape 
{
public:
    // constructor
    Sphere(json const &params);

    // distance function
    ftype distanceFunction(Vector pos);
    ftype distanceFunctionSquared(Vector pos);

    // additional public member fields
    ftype radius;

};

/**
 * @brief represents a torus object to be rendered
 */
class Torus : public Shape 
{
public:
    // constructor
    Torus(json const &params);

    // distance function
    ftype distanceFunction(Vector pos);
    ftype distanceFunctionSquared(Vector pos);

    // additional public member fields
    ftype r1;
    ftype r2;
};

/**
 * @brief represents an octahedron object to be rendered
 */
class Octahedron : public Shape 
{
public:
    // constructor
    Octahedron(json const &params);

    // distance function
    ftype distanceFunction(Vector pos);
    ftype distanceFunctionSquared(Vector pos);

    // additional public member fields
    ftype s;
};

/**
 * @brief represents a cone object to be rendered
 */
class Cone : public Shape 
{
public:
    // constructor
    Cone(json const &params);

    // distance function
    ftype distanceFunction(Vector pos);
    ftype distanceFunctionSquared(Vector pos);

    // additional public member fields
    Vector form;
    Vector2 k1;
    Vector2 k2;
    ftype k2_dot_inv;
};

// overload stream operator for ShapeType
std::ostream& operator<<(std::ostream &out, ShapeType const &type);

/**
 * @brief struct storing four distances to be used for AVX versions of 
 * the distance function
 */
typedef struct Distances {
    ftype d0; 
    ftype d1;
    ftype d2;
    ftype d3;
} Distances;

/**
 * @brief struct storing four Shapes we want to compute distance functions on
 * 
 */
typedef struct Spheres {
    Sphere *s0;
    Sphere *s1;
    Sphere *s2;
    Sphere *s3;
} Spheres;

typedef struct Tori {
    Torus *s0;
    Torus *s1;
    Torus *s2;
    Torus *s3;
} Tori;

typedef struct Cones {
    Cone *s0;
    Cone *s1;
    Cone *s2;
    Cone *s3;
} Cones;

typedef struct Planes {
    Plane *s0;
    Plane *s1;
    Plane *s2;
    Plane *s3;
} Planes;

typedef struct Octahedrons {
    Octahedron *s0;
    Octahedron *s1;
    Octahedron *s2;
    Octahedron *s3;
} Octahedrons;

} // namespace sphere




#endif // SPHERE_SHAPES_H