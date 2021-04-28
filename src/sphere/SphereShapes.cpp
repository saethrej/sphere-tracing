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
 * @file SphereShapes.cpp
 * 
 * @brief definition of classes that define the shapes that need to be rendered.
 * This mostly concerns the constructor and the distance functions.
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 26.04.2021
 */

#include <string>
#include <fstream>
#include <iostream>

#include "SphereTypes.h"
#include "SphereShapes.h"

using json = nlohmann::json;

/********************************** Shape ************************************/

/**
 * @brief constructor for a shape, should only be called from within derived
 * classes
 * @param params the parameters for this shape in json format
 * 
 * @throws SphereException if either the file could not be found or if it 
 * contains any syntax error
 */
sphere::Shape::Shape(json const &params) 
{
    // try to read the data from the json object
    try {
        // get non vector data first 
        this->reflection = params.value("reflection", 0.0);
        this->shininess = params.value("shininess", 0.0);

        // obtain data on position and rotation
        json pos = params["position"];
        json rot = params["rotation"];
        this->position = {pos.value("x", 0.0), pos.value("y", 0.0), pos.value("z", 0.0)};
        this->rotation = {rot.value("x", 0.0), rot.value("y", 0.0), rot.value("z", 0.0)};

        // obtain data on color
        json col = params["color"];
        this->color.r = static_cast<float>(col.value("x", 0.0));
        this->color.g = static_cast<float>(col.value("y", 0.0));
        this->color.b = static_cast<float>(col.value("z", 0.0));
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief deletes a shape element (nothing to do)
 */
sphere::Shape::~Shape()
{
    // nothing to do here
}

/**
 * @brief returns the shape type from the string representation in the json
 * scene description file
 * @param shapeName string representation of shape type
 * @returns the ShapeType associated with this name. UNKNOWN if the string 
 * that is provided does not match any of the supported types
 */
sphere::ShapeType sphere::Shape::getShapeType(std::string shapeName)
{
    if (shapeName.compare("plane") == 0) {
        return ShapeType::PLANE;
    } else if (shapeName.compare("box") == 0) {
        return ShapeType::BOX;
    } else if (shapeName.compare("cone") == 0) {
        return ShapeType::CONE;
    } else if (shapeName.compare("octahedron") == 0) {
        return ShapeType::OCTAHEDRON;
    } else if (shapeName.compare("sphere") == 0) {
        return ShapeType::SPHERE;
    } else if (shapeName.compare("torus") == 0) {
        return ShapeType::TORUS;
    } else {
        return ShapeType::UNKNOWN;
    }
}

/********************************** Plane ************************************/

/**
 * @brief constructor for a Plane shape
 * @param plane json object containing this plane's information
 */
sphere::Plane::Plane(json const &plane)
 : Shape(plane)
{
    // try to load plane-specific information
    try {
        json params = plane["params"];
        this->displacement = params.value("displacement", 0.0);
        json nor = params["normal"];
        this->normal = {nor.value("x", 0.0), nor.value("y", 0.0), nor.value("z", 0.0)}; 
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }    
}

/**
 * @brief computes distance from a point to this plane
 * @param pointPos the position of the point
 * @returns the distance between the point and this plane
 */
sphere::ftype sphere::Plane::distanceFunction(Vector *pointPos)
{
    return 0.0;
}

/*********************************** Box *************************************/

/**
 * @brief constructs a box object
 * @param box the json shape description of the box
 */
sphere::Box::Box(json const &box)
 : Shape(box)
{
    // try to load box-specific information
    try {
        json ext = box["params"]["extents"];
        this->extents = {ext.value("x", 0.0), ext.value("y", 0.0), ext.value("z", 0.0)};
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief computes distance from a point to this box
 * @param pointPos position of the point of interest
 * @returns the distance between point and box
 */
sphere::ftype sphere::Box::distanceFunction(Vector *pointPos)
{
    return 0.0;
}

/********************************** Sphere ************************************/

/**
 * @brief creates a sphere object
 * @param sph the json shape description of the sphere
 */
sphere::Sphere::Sphere(json const &sph)
 : Shape(sph)
{
    // try to load sphere-specific information
    try {
        this->radius = sph["params"].value("radius", 0.0);
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief computes distance from a point to this sphere
 * @param pointPos position of the point of interest
 * @returns the distance between point and sphere
 */
sphere::ftype sphere::Sphere::distanceFunction(Vector *pointPos)
{
    return 0.0;
}

/********************************** Torus ************************************/

/**
 * @brief creates a torus object
 * @param torus the json shape description of the torus
 */
sphere::Torus::Torus(json const &torus)
 : Shape(torus)
{
    // try to load torus-specific information
    try {
        this->r1 = torus["params"].value("r1", 0.0);
        this->r2 = torus["params"].value("r2", 0.0);
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief computes distance from a point to this torus
 * @param pointPos position of the point of interest
 * @returns the distance between point and torus
 */
sphere::ftype sphere::Torus::distanceFunction(Vector *pointPos)
{
    return 0.0;
}

/******************************* Octahedron **********************************/

/**
 * @brief creates a new octahedron object
 * @param octa the json shape description of the octahedron
 */
sphere::Octahedron::Octahedron(json const &octa)
 : Shape(octa)
{
    // try to load octahedron-specific information
    try {
        this->s = octa["params"].value("s", 0.0);
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief computes distance from a point to this octahedron
 * @param pointPos position of the point of interest
 * @returns the distance between point and octahedron
 */
sphere::ftype sphere::Octahedron::distanceFunction(Vector *pointPos)
{
    return 0.0;
}

/********************************* Cone **************************************/

/**
 * @brief creates a new cone object
 * @param cone the json shape description of the cone
 */
sphere::Cone::Cone(json const &cone)
 : Shape(cone)
{
    try {
        json fm = cone["params"];
        this->form = {fm[0], fm[1], fm[2]};
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief computes distance from a point to this cone
 * @param pointPos position of the point of interest
 * @returns the distance between point and cone
 */
sphere::ftype sphere::Cone::distanceFunction(Vector *pointPos)
{
    return 0.0;
}