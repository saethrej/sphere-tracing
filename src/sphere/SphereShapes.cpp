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
#include <algorithm>
#include <math.h>
#include <cmath>

#include "SphereTypes.h"
#include "SphereShapes.h"
#include "CustomVector.h"

using json = nlohmann::json;

/**
 * @brief overloads the stream operator for shape type
 * 
 * @param out the stream that the type should be output in
 * @param type the type of the shape
 * @return reference to the stream where the output was written
 */
std::ostream& sphere::operator<<(std::ostream &out, ShapeType const &type)
{
    if (type == sphere::ShapeType::BOX) {
        out << "Box";
    } else if (type == sphere::ShapeType::CONE) {
        out << "Cone";
    } else if (type == sphere::ShapeType::OCTAHEDRON) {
        out << "Octahedron";
    } else if (type == sphere::ShapeType::PLANE) {
        out << "Plane";
    } else if (type == sphere::ShapeType::SPHERE) {
        out << "Sphere";
    } else if (type == sphere::ShapeType::TORUS) {
        out << "Torus";
    } else {
        out << "Unknown Shape";
    }
    return out;
}

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
        if (this->rotation.x != 0 || this->rotation.y != 0 || this->rotation.z != 0) {
            this->isRotated = true;
        } else {
            this->isRotated = false;
        }

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

    // we create the inverse rotation matrix
    ftype r = M_PI/180.0;
    ftype phi = rotation.x * r;
    ftype theta = rotation.y * r;
    ftype xi = rotation.z * r;
    inverseRotation[0] = cos(theta) * cos(xi);
    inverseRotation[1] = cos(theta) * sin(xi);
    inverseRotation[2] = -sin(theta); // x axis
    inverseRotation[3] = -cos(phi) * sin(xi) + sin(phi) * sin(theta) * cos(xi);
    inverseRotation[4] = cos(phi) * cos(xi) + sin(phi) * sin(theta) * sin(xi);
    inverseRotation[5] = sin(phi) * cos(theta); // y axis
    inverseRotation[6] = sin(phi) * sin (xi) + cos(phi) * sin(theta) * cos(xi);
    inverseRotation[7] = - sin(phi) * cos(xi) + cos(phi) * sin(theta) * sin(xi);
    inverseRotation[8] = cos(phi) * cos(theta); // z axis
}

/**
 * @brief deletes a shape element (nothing to do)
 */
sphere::Shape::~Shape()
{
    // nothing to do here
}

/**
 * @brief Translates and rotates a point such that shape is at origin and in normal position
 * 
 * @param pos vector to point that should be translatet and rotated
 * @return sphere::Vector translated and rotated vector
 */
sphere::Vector sphere::Shape::translate_rotate(Vector *pos)
{
    Vector translated = *pos - position;
    return translated.rotate(this->inverseRotation);
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
        this->normal = Vector(nor.value("x", 0.0), nor.value("y", 0.0), nor.value("z", 0.0)).normalize();
        this->name = "plane";
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
sphere::ftype sphere::Plane::distanceFunction(Vector pointPos)
{
    // rotate only if there is a non-zero rotation
    if (!isRotated){
        // only compute distance with translation
        COUNT_OPS(2)
        return std::abs((pointPos - this->position) * this->normal - this->displacement);
    } else {
        // translate, rotate and compute distance
        Vector tr_point = Shape::translate_rotate(&pointPos);
        COUNT_OPS(2)
        return std::abs(tr_point * this->normal - this->displacement);
    }
}

/**
 * @brief computes squared distance from a point to this plane
 * @param pointPos the position of the point
 * @returns the distance between the point and this plane
 */
sphere::ftype sphere::Plane::distanceFunctionSquared(Vector pointPos)
{
    // rotate only if there is a non-zero rotation
    if (!isRotated){
        // only compute distance with translation
        COUNT_OPS(2)
        ftype ret_val = (pointPos - this->position) * this->normal - this->displacement;
        return ret_val * ret_val;
    } else {
        // translate, rotate and compute distance
        Vector tr_point = Shape::translate_rotate(&pointPos);
        COUNT_OPS(2)
        ftype ret_val = tr_point * this->normal - this->displacement;
        return ret_val * ret_val;
    }
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
        this->name = "box";
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
sphere::ftype sphere::Box::distanceFunction(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    // this is only done if required, i.e. if the object itself is rotated
    Vector tr_point;
    if (!isRotated) {
        tr_point = pointPos - this->position;
    } else {
        tr_point = Shape::translate_rotate(&pointPos);
    }
    // compute the distance in this new space
    Vector q = tr_point.absVal() - extents;
    ftype ret_val = 0;
    if(q.x >= 0.0){
        COUNT_OPS(2)
        ret_val += q.x * q.x;
    }
    if(q.y >= 0.0){
        COUNT_OPS(2)
        ret_val += q.y * q.y;
    }
    if(q.z >= 0.0){
        COUNT_OPS(2)
        ret_val += q.z * q.z;
    }
    COUNT_OPS(30)
    return sqrt(ret_val);
}

/**
 * @brief computes squared distance from a point to this box
 * @param pointPos position of the point of interest
 * @returns the distance between point and box
 */
sphere::ftype sphere::Box::distanceFunctionSquared(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    // this is only done if required, i.e. if the object itself is rotated
    Vector tr_point;
    if (!isRotated) {
        tr_point = pointPos - this->position;
    } else {
        tr_point = Shape::translate_rotate(&pointPos);
    }
    // compute the distance in this new space
    Vector q = tr_point.absVal() - extents;
    ftype ret_val = 0;
    if(q.x >= 0.0){
        ret_val += q.x * q.x;
    }
    if(q.y >= 0.0){
        ret_val += q.y * q.y;
    }
    if(q.z >= 0.0){
        ret_val += q.z * q.z;
    }
    return ret_val;
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
        this->name = "sphere";
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
sphere::ftype sphere::Sphere::distanceFunction(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    Vector tr_point = pointPos - position;
    COUNT_OPS(1)
    // calculate distance in this coordinate system
    return tr_point.length() - radius;
}

/**
 * @brief computes squared distance from a point to this sphere
 * @param pointPos position of the point of interest
 * @returns the distance between point and sphere
 */
sphere::ftype sphere::Sphere::distanceFunctionSquared(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    Vector tr_point = pointPos - position;
    // calculate distance in this coordinate system
    ftype ret_val = tr_point.length() - radius;
    return ret_val * std::fabs(ret_val);
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
        this->name = "torus";
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
sphere::ftype sphere::Torus::distanceFunction(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    // this is only done if required, i.e. if the object itself is rotated
    Vector tr_point;
    if (!isRotated) {
        tr_point =  pointPos - this->position;
    } else {
        tr_point = Shape::translate_rotate(&pointPos);
    }
    // calculate distance in this coordinate system
    COUNT_OPS(38)
    Vect2D q = {sqrt(tr_point.x*tr_point.x + tr_point.z*tr_point.z) - this->r1, tr_point.y};
    return sqrt(q.x * q.x + q.y * q.y) - this->r2;
}

/**
 * @brief computes squared distance from a point to this torus
 * @param pointPos position of the point of interest
 * @returns the distance between point and torus
 */
sphere::ftype sphere::Torus::distanceFunctionSquared(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    // this is only done if required, i.e. if the object itself is rotated
    Vector tr_point;
    if (!isRotated) {
        tr_point =  pointPos - this->position;
    } else {
        tr_point = Shape::translate_rotate(&pointPos);
    }
    // calculate distance in this coordinate system
    
    Vect2D q = {sqrt(tr_point.x*tr_point.x + tr_point.z*tr_point.z) - this->r1, tr_point.y};
    ftype ret_val = sqrt(q.x * q.x + q.y * q.y) - this->r2;
    return ret_val * std::fabs(ret_val);
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
        this->name = "octahedron";
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
sphere::ftype sphere::Octahedron::distanceFunction(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    Vector tr_point;
    if (!isRotated) {
        tr_point = pointPos - this->position;
    } else {
        tr_point = Shape::translate_rotate(&pointPos);
    }
    // calculate distance in this coordinate system
    Vector abs_tr_point = tr_point.absVal();
    ftype m = abs_tr_point.x + abs_tr_point.y + abs_tr_point.z - s;
    Vector r = abs_tr_point * 3.0 - m;
    COUNT_OPS(5)
    Vector q;
    if (r.x < 0){
        q = abs_tr_point;
    }
    else if(r.y < 0) {
        q = Vector(abs_tr_point.y, abs_tr_point.z, abs_tr_point.x);
    }
    else if(r.z < 0) {
        q = Vector(abs_tr_point.z, abs_tr_point.x, abs_tr_point.y);
    }
    else {
        COUNT_OPS(1)
        return m*0.57735027;
    }
    ftype y_s = q.y - s;
    ftype to_clamp = 0.5*(q.z - y_s);
    ftype k = to_clamp < 0.0 ? 0.0 : (to_clamp > s ? s : to_clamp);
    COUNT_OPS(5)
    return Vector(q.x, y_s + k, q.z - k).length();
}

/**
 * @brief computes squared distance from a point to this octahedron
 * @param pointPos position of the point of interest
 * @returns the distance between point and octahedron
 */
sphere::ftype sphere::Octahedron::distanceFunctionSquared(Vector pointPos)
{
    // translate and rotate point such that object is at origin and in normal position
    Vector tr_point;
    if (!isRotated) {
        tr_point = pointPos - this->position;
    } else {
        tr_point = Shape::translate_rotate(&pointPos);
    }
    // calculate distance in this coordinate system
    Vector abs_tr_point = tr_point.absVal();
    ftype m = abs_tr_point.x + abs_tr_point.y + abs_tr_point.z - s;
    Vector r = abs_tr_point * 3.0 - m;
    Vector q;
    if (r.x < 0){
        q = abs_tr_point;
    }
    else if(r.y < 0) {
        q = Vector(abs_tr_point.y, abs_tr_point.z, abs_tr_point.x);
    }
    else if(r.z < 0) {
        q = Vector(abs_tr_point.z, abs_tr_point.x, abs_tr_point.y);
    }
    else {
        return m*m*0.3333333334;
    }
    ftype y_s = q.y - s;
    ftype to_clamp = 0.5*(q.z - y_s);
    ftype k = to_clamp < 0.0 ? 0.0 : (to_clamp > s ? s : to_clamp);
    return q.x * q.x + (y_s + k)*(y_s + k) + (q.z - k)*(q.z - k);
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
        this->k1 = {form.y, form.z};
        this->k2 = {form.y - form.x, 2.0*form.z};
        this->k2_dot_inv = 1/(k2*k2);
        this->name = "cone";
    }
    catch (const json::exception &e) {
        // print information on exception and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief returns the signed distance to a cone
 * @param pointPos the position of the point 
 * @return the signed distance
 */
sphere::ftype sphere::Cone::distanceFunction(Vector pointPos)
{
    // extract the relevant values from the form param of the cone
    VectorVal h = this->form.z, r1 = this->form.x, r2 = this->form.y;

    // translate and rotate point such that the object is at the origin
    Vector rotP;
    if (!isRotated){
        rotP = pointPos - this->position;
    } else {
        rotP = Shape::translate_rotate(&pointPos);
    }
    // calculate the distance in this coordinate system
    Vector2 q = Vector2(Vector2(rotP.x, rotP.z).length(), rotP.y);

    Vector2 ca = Vector2(
        q.x - std::min(q.x, q.y < 0 ? r1 : r2),
        std::fabs(q.y) - h
    );
    Vector2 cb = q - this->k1 + this->k2 * std::clamp((this->k2 * (this->k1 - q)) * this->k2_dot_inv, 0.0, 1.0);
    ftype s = cb.x < 0.0 && ca.y < 0.0 ? -1.0 : 1.0;
    COUNT_OPS(34)
    return s * std::sqrt(std::min(ca * ca, cb * cb));
}

/**
 * @brief returns the squared signed distance to a cone
 * @param pointPos the position of the point 
 * @return the signed distance
 */
sphere::ftype sphere::Cone::distanceFunctionSquared(Vector pointPos)
{
    // extract the relevant values from the form param of the cone
    VectorVal h = this->form.z, r1 = this->form.x, r2 = this->form.y;

    // translate and rotate point such that the object is at the origin
    Vector rotP;
    if (!isRotated){
        rotP = pointPos - this->position;
    } else {
        rotP = Shape::translate_rotate(&pointPos);
    }
    // calculate the distance in this coordinate system
    Vector2 q = Vector2(Vector2(rotP.x, rotP.z).length(), rotP.y);

    Vector2 ca = Vector2(
        q.x - std::min(q.x, q.y < 0 ? r1 : r2),
        std::fabs(q.y) - h
    );
    Vector2 cb = q - this->k1 + this->k2 * std::clamp((this->k2 * (this->k1 - q)) *this->k2_dot_inv, 0.0, 1.0);
    ftype s = cb.x < 0.0 && ca.y < 0.0 ? -1.0 : 1.0;
    return s * std::min(ca * ca, cb * cb);
}