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

// maximum number of objects per shape type 
constexpr itype MAX_OBJECTS = 16;

// forward declarations
class ShapeWrapper; class BoxWrapper; class ConeWrapper; class OctaWrapper;
class PlaneWrapper; class SphereWrapper; class TorusWrapper;

/**
 * @brief enum class defining the different shapes we can render. Each
 * shape has exactly one of the types in this enum.
 */
enum class ShapeType {PLANE, BOX, SPHERE, TORUS, OCTAHEDRON, CONE, UNKNOWN};

/**
 * @brief simple class that stores four distances for the vectorized 
 * distance functions
 */
class Distances
{
public: 
    // constructor
    Distances(ftype dd0, ftype dd1, ftype dd2, ftype dd3) 
        : d0(dd0), d1(dd1), d2(dd2), d3(dd3) {}

    // public member fields
    ftype d0;
    ftype d1;
    ftype d2;
    ftype d3;
};

/******************************* Shape Classes *******************************/

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
    static Distances vectDistFunc(TorusWrapper const *wTorus, Vector const &ray, itype idx);

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

/******************************* Shape Wrappers ******************************/

/**
 * @brief wrapper class for Shape for vectorization
 */
class ShapeWrapper
{
public:
    // constructor & destructor
    ShapeWrapper();
    virtual ~ShapeWrapper();

    // function to add a shape
    void addShape(Shape *shp);

    // public member fields (arrays for aligned access)
    ftype *xPos; //!< x-axis positions of all shapes
    ftype *yPos; //!< y-axis positions of all shapes
    ftype *zPos; //!< z-axis positions of all shapes
    ftype *rotMatrix; //!< the rotation matrices of all shapes 
    itype numElems; //!< number of elements of this type in scene
};

/**
 * @brief wrapper class for Plane for vectorization
 */
class PlaneWrapper : public ShapeWrapper
{
public:
    // constructor & destructor
    PlaneWrapper();
    ~PlaneWrapper();

    // function to add a plane
    void addPlane(Plane *plane);

    // additional public member fields
    ftype *xNor; //!< normal vector's x-coordinates
    ftype *yNor; //!< normal vector's y-coordinates
    ftype *zNor; //!< normal vector's z-coordinates
    ftype *displacement; //!< displacement values
    std::vector<Plane*> planes; //!< container of all plane objects
};

/**
 * @brief wrapper class for Box for vectorization
 */
class BoxWrapper : public ShapeWrapper
{
public:
    // constructor & destructor
    BoxWrapper();
    ~BoxWrapper();

    // function to add a box
    void addBox(Box *box);

    // additional public member fields
    ftype *xExt; //!< normal vector's x-coordinates
    ftype *yExt; //!< normal vector's y-coordinates
    ftype *zExt; //!< normal vector's z-coordinates
    std::vector<Box*> boxes; //!< container with all box objects
};

/**
 * @brief wrapper class for Sphere for vectorization
 */
class SphereWrapper : public ShapeWrapper
{
public:
    // constructor & destructor
    SphereWrapper();
    ~SphereWrapper();

    // function to add a sphere
    void addSphere(Sphere *sph);

    // additional public member fields
    ftype *radiuses; //!< sphere radiuses
    std::vector<Sphere*> spheres; //!< container with all sphere objects
};

/**
 * @brief wrapper class for Torus for vectorization
 */
class TorusWrapper : public ShapeWrapper
{
public:
    // constructor & destructor
    TorusWrapper();
    ~TorusWrapper();

    // function to add a torus
    void addTorus(Torus *torus);

    // additional public member fields
    ftype *r1s; //!< inner torus radiuses
    ftype *r2s; //!< outer torus radiuses
    std::vector<Torus*> tori; //!< container with all torus objects
};

/**
 * @brief wrapper class for Octahedron for vectorization
 */
class OctaWrapper : public ShapeWrapper
{
public:
    // constructor & destructor
    OctaWrapper();
    ~OctaWrapper();

    // function to add a Octahedron
    void addOcta(Octahedron *octa);

    // additional public member fields
    ftype *s; //!< all s values
    std::vector<Octahedron*> octas; //!< container with all octahedron objects
};

/**
 * @brief wrapper class for Cone for vectorization
 */
class ConeWrapper : public ShapeWrapper
{
public:
    // constructor & destructor
    ConeWrapper();
    ~ConeWrapper();

    // function to add a cone
    void addCone(Cone *cone);

    // additional public member fields
    ftype *xForm; //!< x-coordinates of form vectors
    ftype *yForm; //!< y-coordinates of form vectors
    ftype *zForm; //!< z-coordinates of form vectors
    ftype *xK1;   //!< x-coordinates of K1 param
    ftype *yK1;   //!< y-coordinates of K1 param
    ftype *xK2;   //!< x-coordinates of K2 param
    ftype *yK2;   //!< y-coordinates of K2 param
    ftype *k2DotInv; //!< all inverse of k2's dot product
    std::vector<Cone*> cones; //!< container with all cone objects
};

} // namespace sphere

#endif // SPHERE_SHAPES_H