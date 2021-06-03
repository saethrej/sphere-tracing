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
#include <immintrin.h>

#include "SphereTypes.h"
#include "SphereShapes.h"
#include "CustomVector.h"
#include "immintrin.h"


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

void sphere::Plane::vectDistFunc(PlaneWrapper const *planeWrap, Vector const &pos, itype idx, ftype *dstPtr) {
    __m256d rotx, roty,rotz;
    __m256d rotx2, roty2, rotz2;
    __m256d rotx3, roty3, rotz3;
    __m256d xCoords,yCoords,zCoords;
    __m256d xTrPoint, yTrPoint, zTrPoint;
    __m256d xRotPoint, yRotPoint, zRotPoint;

    __m256d xPos = _mm256_broadcast_sd(&pos.x);
    __m256d yPos = _mm256_broadcast_sd(&pos.y);
    __m256d zPos = _mm256_broadcast_sd(&pos.z);
    __m256d displacement = _mm256_load_pd(planeWrap->displacement + idx);
    __m256d abs_mask = _mm256_set1_pd(-0.0);

    xCoords = _mm256_load_pd(planeWrap->xPos + idx);
    yCoords = _mm256_load_pd(planeWrap->yPos + idx);
    zCoords = _mm256_load_pd(planeWrap->zPos + idx);

    xTrPoint = _mm256_sub_pd(xPos, xCoords);
    yTrPoint = _mm256_sub_pd(yPos, yCoords);
    zTrPoint = _mm256_sub_pd(zPos, zCoords);

    rotx = _mm256_load_pd(planeWrap->rotMatrix + idx);
    roty = _mm256_load_pd(planeWrap->rotMatrix + idx + MAX_OBJECTS);
    rotz = _mm256_load_pd(planeWrap->rotMatrix + idx + MAX_OBJECTS + MAX_OBJECTS);

    xRotPoint = _mm256_fmadd_pd(rotz, zTrPoint, _mm256_fmadd_pd(roty, yTrPoint, _mm256_mul_pd(rotx, xTrPoint)));

    rotx2 = _mm256_load_pd(planeWrap->rotMatrix + idx + 3* MAX_OBJECTS);
    roty2 = _mm256_load_pd(planeWrap->rotMatrix + idx + 4*MAX_OBJECTS);
    rotz2 = _mm256_load_pd(planeWrap->rotMatrix + idx + 5*MAX_OBJECTS);

    yRotPoint = _mm256_fmadd_pd(rotz2, zTrPoint, _mm256_fmadd_pd(roty2, yTrPoint, _mm256_mul_pd(rotx2, xTrPoint)));

    rotx3 = _mm256_load_pd(planeWrap->rotMatrix + idx + 6*MAX_OBJECTS);
    roty3 = _mm256_load_pd(planeWrap->rotMatrix + idx + 7*MAX_OBJECTS);
    rotz3 = _mm256_load_pd(planeWrap->rotMatrix + idx+ 8*MAX_OBJECTS);

    zRotPoint = _mm256_fmadd_pd(rotz3, zTrPoint, _mm256_fmadd_pd(roty3, yTrPoint, _mm256_mul_pd(rotx3, xTrPoint)));

    __m256d xNormal = _mm256_load_pd(planeWrap->xNor + idx);
    __m256d yNormal = _mm256_load_pd(planeWrap->yNor + idx);
    __m256d zNormal = _mm256_load_pd(planeWrap->zNor + idx);

    __m256d xScal = _mm256_mul_pd(xRotPoint, xNormal);
    __m256d yScal = _mm256_fmadd_pd(yRotPoint, yNormal, xScal);
    __m256d scalProd = _mm256_fmadd_pd(zRotPoint, zNormal, yScal);

    _mm256_store_pd(dstPtr, _mm256_andnot_pd(abs_mask, _mm256_sub_pd(scalProd, displacement)));
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
        return std::abs((pointPos - this->position) * this->normal - this->displacement);
    } else {
        // translate, rotate and compute distance
        Vector tr_point = Shape::translate_rotate(&pointPos);
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
        ftype ret_val = (pointPos - this->position) * this->normal - this->displacement;
        return ret_val * ret_val;
    } else {
        // translate, rotate and compute distance
        Vector tr_point = Shape::translate_rotate(&pointPos);
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

void sphere::Box::vectDistFunc(BoxWrapper const *boxWrap, Vector const &pos, itype idx, ftype *dstPtr)
{


    __m256d rotx, roty,rotz;
    __m256d rotx2, roty2, rotz2;
    __m256d rotx3, roty3, rotz3;
    __m256d xCoords,yCoords,zCoords;
    __m256d xTrPoint, yTrPoint, zTrPoint;
    __m256d xRotPoint, yRotPoint, zRotPoint;
    __m256d xext, yext, zext;
    __m256d zero = _mm256_setzero_pd();

    __m256d xPos = _mm256_broadcast_sd(&pos.x);
    __m256d yPos = _mm256_broadcast_sd(&pos.y);
    __m256d zPos = _mm256_broadcast_sd(&pos.z);

    xCoords = _mm256_load_pd(boxWrap->xPos + idx);
    yCoords = _mm256_load_pd(boxWrap->yPos + idx);
    zCoords = _mm256_load_pd(boxWrap->zPos + idx);

    xTrPoint = _mm256_sub_pd(xPos, xCoords);
    yTrPoint = _mm256_sub_pd(yPos, yCoords);
    zTrPoint = _mm256_sub_pd(zPos, zCoords);

    rotx = _mm256_load_pd(boxWrap->rotMatrix + idx);
    roty = _mm256_load_pd(boxWrap->rotMatrix + idx + MAX_OBJECTS);
    rotz = _mm256_load_pd(boxWrap->rotMatrix + idx + MAX_OBJECTS + MAX_OBJECTS);


    xRotPoint = _mm256_fmadd_pd(rotz, zTrPoint, _mm256_fmadd_pd(roty, yTrPoint, _mm256_mul_pd(rotx, xTrPoint)));

    rotx2 = _mm256_load_pd(boxWrap->rotMatrix + idx + 3* MAX_OBJECTS);
    roty2 = _mm256_load_pd(boxWrap->rotMatrix + idx + 4*MAX_OBJECTS);
    rotz2 = _mm256_load_pd(boxWrap->rotMatrix + idx + 5*MAX_OBJECTS);

    yRotPoint = _mm256_fmadd_pd(rotz2, zTrPoint, _mm256_fmadd_pd(roty2, yTrPoint, _mm256_mul_pd(rotx2, xTrPoint)));

    rotx3 = _mm256_load_pd(boxWrap->rotMatrix + idx + 6*MAX_OBJECTS);
    roty3 = _mm256_load_pd(boxWrap->rotMatrix + idx + 7*MAX_OBJECTS);
    rotz3 = _mm256_load_pd(boxWrap->rotMatrix + idx+ 8*MAX_OBJECTS);

    zRotPoint = _mm256_fmadd_pd(rotz3, zTrPoint, _mm256_fmadd_pd(roty3, yTrPoint, _mm256_mul_pd(rotx3, xTrPoint)));


    // calculate the absolute value
    __m256d abs_mask = _mm256_set1_pd(-0.0);
    __m256d  abs_x = _mm256_andnot_pd(abs_mask, xRotPoint);
    __m256d  abs_y = _mm256_andnot_pd(abs_mask, yRotPoint);
    __m256d  abs_z = _mm256_andnot_pd(abs_mask, zRotPoint);



    xext = _mm256_load_pd(boxWrap->xExt + idx);
    yext = _mm256_load_pd(boxWrap->yExt + idx);
    zext = _mm256_load_pd(boxWrap->zExt + idx);

    

    // abs - extent

    __m256d xq = _mm256_sub_pd(abs_x, xext);
    __m256d yq = _mm256_sub_pd(abs_y, yext);
    __m256d zq = _mm256_sub_pd(abs_z, zext);


    __m256d xMask = _mm256_cmp_pd(xq, zero, _CMP_GE_OQ);
    __m256d yMask = _mm256_cmp_pd(yq, zero, _CMP_GE_OQ);
    __m256d zMask = _mm256_cmp_pd(zq, zero, _CMP_GE_OQ);

    __m256d xtmp = _mm256_mul_pd(xq, xq);
    __m256d res0 = _mm256_and_pd(xtmp, xMask);
    __m256d tmp1 = _mm256_add_pd(res0, zero);
    __m256d ytmp = _mm256_mul_pd(yq, yq);
    __m256d res1 = _mm256_and_pd(ytmp, yMask);
    __m256d tmp2 = _mm256_add_pd(res1, zero);
    __m256d ztmp = _mm256_mul_pd(zq, zq);
    __m256d res2 = _mm256_and_pd(ztmp, zMask);
    __m256d tmp3 = _mm256_add_pd(res2, zero);

    __m256d squaredRes = _mm256_add_pd(_mm256_add_pd(tmp1,tmp2), tmp3);

    __m256d res = _mm256_sqrt_pd(squaredRes);

    _mm256_store_pd(dstPtr, res);


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
        ret_val += q.x * q.x;
    }
    if(q.y >= 0.0){
        ret_val += q.y * q.y;
    }
    if(q.z >= 0.0){
        ret_val += q.z * q.z;
    }
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

void sphere::Sphere::vectDistFunc(SphereWrapper const *sphereWrap, Vector const &pos, itype idx, ftype *dstPtr) {
    __m256d xCoords, yCoords, zCoords;
    __m256d xRay, yRay, zRay;
    __m256d xTrPoint, yTrPoint, zTrPoint;
    __m256d vRadius;
    __m256d xSquaredCoord, ySquaredCoord, zSquaredCoord;
    __m256d vectorLength;

    xCoords = _mm256_load_pd(sphereWrap->xPos + idx);
    yCoords = _mm256_load_pd(sphereWrap->yPos + idx);
    zCoords = _mm256_load_pd(sphereWrap->zPos + idx);
    
    
    xRay = _mm256_broadcast_sd(&pos.x);
    yRay = _mm256_broadcast_sd(&pos.y);
    zRay = _mm256_broadcast_sd(&pos.z);

    vRadius = _mm256_load_pd(sphereWrap->radiuses + idx);

    xTrPoint = _mm256_sub_pd(xRay, xCoords);
    yTrPoint = _mm256_sub_pd(yRay, yCoords);
    zTrPoint = _mm256_sub_pd(zRay, zCoords);
    xSquaredCoord = _mm256_mul_pd(xTrPoint, xTrPoint);
    ySquaredCoord = _mm256_mul_pd(yTrPoint, yTrPoint);
    zSquaredCoord = _mm256_mul_pd(zTrPoint, zTrPoint);

    vectorLength = _mm256_sqrt_pd(_mm256_add_pd(xSquaredCoord, _mm256_add_pd(ySquaredCoord, zSquaredCoord)));

    __m256d res = _mm256_sub_pd(vectorLength, vRadius);

    _mm256_store_pd(dstPtr, res);
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
    //std::cout << "trpoint "  << tr_point.x;
    // calculate distance in this coordinate system
    Vector2 q(sqrt(tr_point.x*tr_point.x + tr_point.z*tr_point.z) - this->r1, tr_point.y);
    return q.length() - this->r2;
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
    
    Vector2 q(sqrt(tr_point.x*tr_point.x + tr_point.z*tr_point.z) - this->r1, tr_point.y);
    ftype ret_val = q.length() - this->r2;
    return ret_val * std::fabs(ret_val);
}
/**
 * @brief computes distance from a point to this torus
 * @param pointPos position of the point of interest
 * @returns the distance between point and torus
 */
void sphere::Torus::vectDistFunc(TorusWrapper const *wTorus , Vector const &ray, itype idx, ftype *dstPtr)
{
    __m256d xTrPoint, yTrPoint, zTrPoint;
    __m256d xRay, yRay, zRay;
    __m256d xCoords, yCoords, zCoords;
    __m256d xValsSquared,yValsSquared, zValsSquared;
    __m256d vectorLength, vectorLengthR1, vectorLengthR1Squared;
    __m256d r1, r2;
    __m256d q, res, absVal, retVal;

    // load components of the pos vector into registers
    __m256d xPos = _mm256_broadcast_sd(&ray.x);
    __m256d yPos = _mm256_broadcast_sd(&ray.y);
    __m256d zPos = _mm256_broadcast_sd(&ray.z);

    //load the torus position
    __m256d torusPosX = _mm256_load_pd(wTorus->xPos + idx);
    __m256d torusPosY = _mm256_load_pd(wTorus->yPos + idx);
    __m256d torusPosZ = _mm256_load_pd(wTorus->zPos + idx);

    //translate the vector
    xPos = _mm256_sub_pd(xPos, torusPosX);
    yPos = _mm256_sub_pd(yPos, torusPosY);
    zPos = _mm256_sub_pd(zPos, torusPosZ);

    //load rotMatrix
    __m256d rotCol00 = _mm256_load_pd(wTorus->rotMatrix +idx);
    __m256d rotCol01 = _mm256_load_pd(wTorus->rotMatrix + idx + MAX_OBJECTS);
    __m256d rotCol02 = _mm256_load_pd(wTorus->rotMatrix + idx + 2*MAX_OBJECTS);

    //compute rotation
    __m256d xRotPoint = _mm256_fmadd_pd(zPos, rotCol02, _mm256_fmadd_pd(yPos, rotCol01, _mm256_mul_pd(xPos, rotCol00))); 

    //load rotMax
    __m256d rotCol10 = _mm256_load_pd(wTorus->rotMatrix + idx + 3*MAX_OBJECTS);
    __m256d rotCol11 = _mm256_load_pd(wTorus->rotMatrix + idx + 4*MAX_OBJECTS);
    __m256d rotCol12 = _mm256_load_pd(wTorus->rotMatrix + idx + 5*MAX_OBJECTS);

    //compute rotation
     __m256d yRotPoint = _mm256_fmadd_pd(zPos, rotCol12, _mm256_fmadd_pd(yPos, rotCol11, _mm256_mul_pd(xPos, rotCol10))); 
    
    //load rotMax
    __m256d rotCol20 = _mm256_load_pd(wTorus->rotMatrix +idx + 6*MAX_OBJECTS);
    __m256d rotCol21 = _mm256_load_pd(wTorus->rotMatrix + idx +7*MAX_OBJECTS);
    __m256d rotCol22 = _mm256_load_pd(wTorus->rotMatrix + idx + 8*MAX_OBJECTS);

    //load rotMax
    __m256d zRotPoint = _mm256_fmadd_pd(zPos, rotCol22, _mm256_fmadd_pd(yPos, rotCol21, _mm256_mul_pd(xPos, rotCol20))); 

    //load torus parameters
    r1 = _mm256_load_pd(wTorus->r1s + idx);
    r2 = _mm256_load_pd(wTorus->r2s + idx);

    xValsSquared = _mm256_mul_pd(xRotPoint, xRotPoint);

    vectorLength = _mm256_sqrt_pd(_mm256_fmadd_pd(zRotPoint, zRotPoint, xValsSquared));
    vectorLengthR1 = _mm256_sub_pd(vectorLength, r1);
    vectorLengthR1Squared = _mm256_mul_pd(vectorLengthR1, vectorLengthR1);

    q = _mm256_sqrt_pd(_mm256_fmadd_pd(yRotPoint, yRotPoint,vectorLengthR1Squared));
    res = _mm256_sub_pd(q, r2);

    //return dist 
    _mm256_store_pd(dstPtr, res);
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
        return m*0.57735027;
    }
    ftype y_s = q.y - s;
    ftype to_clamp = 0.5*(q.z - y_s);
    ftype k = to_clamp < 0.0 ? 0.0 : (to_clamp > s ? s : to_clamp);
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

/**
 * @brief returns the distances of the pos vector to four boxes
 * 
 * @param octas octahedrons pointer to four octahedrons
 * @param pos the position to compute distance from
 * @return sphere::Distances the four distances of each octahedron
 */
void sphere::Octahedron::vectDistFunc(OctaWrapper const *wOcta, Vector const &ray, itype idx, ftype *destPtr)
{
    __m256d zero = _mm256_setzero_pd();
    __m128i rotOffsets = _mm_set_epi32(0, 3, 6, 0);
    __m256d colMask = _mm256_set_pd(-0.0, -0.0, -0.0, 0.0);

    // load components of the pos vector into registers
    __m256d xPos = _mm256_broadcast_sd(&ray.x);
    __m256d yPos = _mm256_broadcast_sd(&ray.y);
    __m256d zPos = _mm256_broadcast_sd(&ray.z);

    // load the octa positions
    __m256d octaPosX = _mm256_load_pd(wOcta->xPos + idx);
    __m256d octaPosY = _mm256_load_pd(wOcta->yPos + idx);
    __m256d octaPosZ = _mm256_load_pd(wOcta->zPos + idx);

    // translate the vector
    xPos = _mm256_sub_pd(xPos, octaPosX);
    yPos = _mm256_sub_pd(yPos, octaPosY);
    zPos = _mm256_sub_pd(zPos, octaPosZ);

     //load rotMatrix
    __m256d rotCol00 = _mm256_load_pd(wOcta->rotMatrix +idx);
    __m256d rotCol01 = _mm256_load_pd(wOcta->rotMatrix + idx + MAX_OBJECTS);
    __m256d rotCol02 = _mm256_load_pd(wOcta->rotMatrix + idx + 2*MAX_OBJECTS);

    //compute rotation
    __m256d xRotPoint = _mm256_fmadd_pd(zPos, rotCol02, _mm256_fmadd_pd(yPos, rotCol01, _mm256_mul_pd(xPos, rotCol00))); 

    //load rotMax
    __m256d rotCol10 = _mm256_load_pd(wOcta->rotMatrix + idx + 3*MAX_OBJECTS);
    __m256d rotCol11 = _mm256_load_pd(wOcta->rotMatrix + idx + 4*MAX_OBJECTS);
    __m256d rotCol12 = _mm256_load_pd(wOcta->rotMatrix + idx + 5*MAX_OBJECTS);

    //compute rotation
     __m256d yRotPoint = _mm256_fmadd_pd(zPos, rotCol12, _mm256_fmadd_pd(yPos, rotCol11, _mm256_mul_pd(xPos, rotCol10))); 
    
    //load rotMax
    __m256d rotCol20 = _mm256_load_pd(wOcta->rotMatrix +idx + 6*MAX_OBJECTS);
    __m256d rotCol21 = _mm256_load_pd(wOcta->rotMatrix + idx +7*MAX_OBJECTS);
    __m256d rotCol22 = _mm256_load_pd(wOcta->rotMatrix + idx + 8*MAX_OBJECTS);

    //load rotMax
    __m256d zRotPoint = _mm256_fmadd_pd(zPos, rotCol22, _mm256_fmadd_pd(yPos, rotCol21, _mm256_mul_pd(xPos, rotCol20))); 

    // load s value
    __m256d s_vect = _mm256_load_pd(wOcta->s + idx);

    //compute abs value
    __m256d abs_mask = _mm256_set1_pd(-0.0);
    __m256d x_abs = _mm256_andnot_pd(abs_mask, xRotPoint);
    __m256d y_abs = _mm256_andnot_pd(abs_mask, yRotPoint);
    __m256d z_abs = _mm256_andnot_pd(abs_mask, zRotPoint);

    //compute m
    __m256d m_0 = _mm256_add_pd(_mm256_add_pd(x_abs, y_abs), z_abs);
    __m256d m = _mm256_sub_pd(m_0, s_vect);

    // compute r vectors
    __m256d three = _mm256_set1_pd(3.0);
    __m256d r_x = _mm256_fmsub_pd(x_abs, three, m);
    __m256d r_y = _mm256_fmsub_pd(y_abs, three, m);
    __m256d r_z = _mm256_fmsub_pd(z_abs, three, m);

    // compute masks
    __m256d x_mask = _mm256_cmp_pd(r_x, zero, _CMP_LT_OQ);
    __m256d y_mask = _mm256_cmp_pd(r_y, zero, _CMP_LT_OQ);
    __m256d z_mask = _mm256_cmp_pd(r_z, zero, _CMP_LT_OQ);
    __m256d sqrt0_3 = _mm256_set1_pd(0.57735027);
    __m256d m_ret = _mm256_mul_pd(m, sqrt0_3); 
    __m256d m_mask = _mm256_or_pd(_mm256_or_pd(x_mask, y_mask), z_mask);

    // create new vectors depending on masks
    __m256d new_y_0 = _mm256_blendv_pd(zero, x_abs, z_mask);
    __m256d new_y_1 = _mm256_blendv_pd(new_y_0, z_abs, y_mask);
    __m256d new_y = _mm256_blendv_pd(new_y_1, y_abs, x_mask);

    __m256d new_x_0 = _mm256_blendv_pd(zero, z_abs, z_mask);
    __m256d new_x_1 = _mm256_blendv_pd(new_x_0, y_abs, y_mask);
    __m256d new_x = _mm256_blendv_pd(new_x_1, x_abs, x_mask);

    __m256d new_z_0 = _mm256_blendv_pd(zero, y_abs, z_mask);
    __m256d new_z_1 = _mm256_blendv_pd(new_z_0, x_abs, y_mask);
    __m256d new_z = _mm256_blendv_pd(new_z_1, z_abs, x_mask);

    // compute y_s
    __m256d y_s = _mm256_sub_pd(new_y, s_vect);

    // clamp
    __m256d zero_point_five = _mm256_set1_pd(0.5);
    __m256d clamp = _mm256_mul_pd(zero_point_five, _mm256_sub_pd(new_z, y_s));
    __m256d sub_mask = _mm256_cmp_pd(clamp, zero, _CMP_LT_OQ);
    __m256d top_mask = _mm256_cmp_pd(s_vect, clamp, _CMP_LT_OQ);
    __m256d k_0 = _mm256_blendv_pd(clamp, zero, sub_mask);
    __m256d k = _mm256_blendv_pd(k_0, s_vect, top_mask);

    // compute square root
    __m256d x_squared = _mm256_mul_pd(new_x, new_x);
    __m256d y_squared_0 = _mm256_add_pd(y_s, k);
    __m256d y_squared = _mm256_mul_pd(y_squared_0, y_squared_0);
    __m256d z_squared_0 = _mm256_sub_pd(new_z, k);
    __m256d x_y_squared = _mm256_add_pd(x_squared, y_squared);
    __m256d all_squared = _mm256_fmadd_pd(z_squared_0, z_squared_0, x_y_squared);
    __m256d sqrt_root = _mm256_sqrt_pd(all_squared);
    __m256d ret_val = _mm256_blendv_pd(m_ret, sqrt_root, m_mask);

    // return dist object
    _mm256_store_pd(destPtr, ret_val);  
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
    Vector2 cb = q - this->k1 + this->k2 * std::clamp((this->k2 * (this->k1 - q)) *this->k2_dot_inv, 0.0, 1.0);
    ftype s = cb.x < 0.0 && ca.y < 0.0 ? -1.0 : 1.0;
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

/**
 * @brief computes the distance function for four cones starting at index idx
 * in a vectorized manner
 * 
 * @param wCone pointer to the ConeWrapper
 * @param pos position to compute distance to
 * @param idx idx to start computations at
 * @returns the four distances
 */
void sphere::Cone::vectDistFunc(ConeWrapper const *wCone, Vector const &pos, itype idx, ftype *destPtr)
{
    __m256d zero = _mm256_setzero_pd();
    __m256d ones = _mm256_set1_pd(1.0);
    __m256d minusones = _mm256_set1_pd(-1.0);
    __m256d abs_mask = _mm256_set1_pd(-0.0);


    // broadcast the components of the position vector to avx registers
    __m256d xPos = _mm256_broadcast_sd(&pos.x);
    __m256d yPos = _mm256_broadcast_sd(&pos.y);
    __m256d zPos = _mm256_broadcast_sd(&pos.z);

    // load the cone positions
    __m256d conePosX = _mm256_load_pd(wCone->xPos + idx);
    __m256d conePosY = _mm256_load_pd(wCone->yPos + idx);
    __m256d conePosZ = _mm256_load_pd(wCone->zPos + idx);

    // translate the vector to the center of the cone
    xPos = _mm256_sub_pd(xPos, conePosX);
    yPos = _mm256_sub_pd(yPos, conePosY);
    zPos = _mm256_sub_pd(zPos, conePosZ);

    // load rotation matrix values
    ftype *rotPtr = wCone->rotMatrix + idx;
    __m256d rot0 = _mm256_load_pd(rotPtr);
    __m256d rot1 = _mm256_load_pd(rotPtr + MAX_OBJECTS);
    __m256d rot2 = _mm256_load_pd(rotPtr + 2 * MAX_OBJECTS);
    __m256d rot3 = _mm256_load_pd(rotPtr + 3 * MAX_OBJECTS);
    __m256d rot4 = _mm256_load_pd(rotPtr + 4 * MAX_OBJECTS);
    __m256d rot5 = _mm256_load_pd(rotPtr + 5 * MAX_OBJECTS);
    __m256d rot6 = _mm256_load_pd(rotPtr + 6 * MAX_OBJECTS);
    __m256d rot7 = _mm256_load_pd(rotPtr + 7 * MAX_OBJECTS);
    __m256d rot8 = _mm256_load_pd(rotPtr + 8 * MAX_OBJECTS);

    // rotate the matrix
    xPos = _mm256_fmadd_pd(zPos, rot2, _mm256_fmadd_pd(yPos, rot1, _mm256_mul_pd(xPos, rot0)));
    yPos = _mm256_fmadd_pd(zPos, rot5, _mm256_fmadd_pd(yPos, rot4, _mm256_mul_pd(xPos, rot3)));
    zPos = _mm256_fmadd_pd(zPos, rot8, _mm256_fmadd_pd(yPos, rot7, _mm256_mul_pd(xPos, rot6)));

    // load the form parameters 
    __m256d heights = _mm256_load_pd(wCone->zForm + idx);
    __m256d rad1 = _mm256_load_pd(wCone->xForm + idx);
    __m256d rad2 = _mm256_load_pd(wCone->yForm + idx);
    __m256d k1_x = _mm256_load_pd(wCone->xK1 + idx);
    __m256d k1_y = _mm256_load_pd(wCone->yK1 + idx);
    __m256d k2_x = _mm256_load_pd(wCone->xK2 + idx);
    __m256d k2_y = _mm256_load_pd(wCone->yK2 + idx);
    __m256d k2DotInv = _mm256_load_pd(wCone->k2DotInv + idx);

    // calculate vector q values (q.y = yPos)
    __m256d x_squared = _mm256_mul_pd(xPos, xPos);
    __m256d q_x_0 = _mm256_fmadd_pd(zPos, zPos, x_squared);
    __m256d q_x = _mm256_sqrt_pd(q_x_0);

    // calculate vector ca
    __m256d q_y_mask = _mm256_cmp_pd(yPos, zero, _CMP_LT_OQ);
    __m256d to_min = _mm256_blendv_pd(rad2, rad1, q_y_mask);
    __m256d ca_x = _mm256_sub_pd(q_x, _mm256_min_pd(q_x, to_min));
    __m256d ca_y = _mm256_sub_pd(_mm256_andnot_pd(abs_mask, yPos), heights);

    // clamp
    __m256d clamp_0x = _mm256_mul_pd(k2_x, _mm256_sub_pd(k1_x, q_x));
    __m256d clamp_0y_pre = _mm256_sub_pd(k1_y, yPos);
    __m256d clamp_0 = _mm256_fmadd_pd(k2_y, clamp_0y_pre, clamp_0x);
    __m256d clamp = _mm256_mul_pd(clamp_0, k2DotInv);
    __m256d sub_mask = _mm256_cmp_pd(clamp, zero, _CMP_LT_OQ);
    __m256d top_mask = _mm256_cmp_pd(ones, clamp, _CMP_LT_OQ);
    __m256d clamped_0 = _mm256_blendv_pd(clamp, zero, sub_mask);
    __m256d clamped = _mm256_blendv_pd(clamped_0, ones, top_mask);

    // get cb
    __m256d cb_0x = _mm256_fmadd_pd(clamped, k2_x, q_x);
    __m256d cb_x = _mm256_sub_pd(cb_0x, k1_x);
    __m256d cb_0y = _mm256_fmadd_pd(clamped, k2_y, yPos);
    __m256d cb_y = _mm256_sub_pd(cb_0y, k1_y);

    // calc s
    __m256d cbx_mask = _mm256_cmp_pd(cb_x, zero, _CMP_LT_OQ);
    __m256d cay_mask = _mm256_cmp_pd(ca_y, zero, _CMP_LT_OQ);
    __m256d s_mask = _mm256_and_pd(cbx_mask, cay_mask);
    __m256d s = _mm256_blendv_pd(ones, minusones, s_mask);

    // calc returnval
    __m256d ca_dot_0 = _mm256_mul_pd(ca_x, ca_x);
    __m256d ca_dot = _mm256_fmadd_pd(ca_y, ca_y, ca_dot_0);
    __m256d cb_dot_0 = _mm256_mul_pd(cb_x, cb_x);
    __m256d cb_dot = _mm256_fmadd_pd(cb_y, cb_y, cb_dot_0);

    __m256d ret_val_0 = _mm256_sqrt_pd(_mm256_min_pd(ca_dot, cb_dot));
    __m256d ret_val = _mm256_mul_pd(s, ret_val_0);

    _mm256_store_pd(destPtr, ret_val);   
}

/*****************************************************************************/
/******************************* WRAPPERS ************************************/
/*****************************************************************************/

/**
 * @brief allocates arrays in new ShapeWrapper object
 */
sphere::ShapeWrapper::ShapeWrapper()
{
    // allocate space for the position data
    xPos = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    yPos = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    zPos = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();

    // allocate 12*MAX_OBJECTS elements for rotation matrix
    rotMatrix = new (std::align_val_t(32)) ftype[MAX_OBJECTS * 9]();
    
    // initially there are 0 elements
    numElems = 0;
    numIters = 0;
    iterCounter = 0.0;
}

/**
 * @brief frees the allocated memory in ShapeWrapper
 */
sphere::ShapeWrapper::~ShapeWrapper()
{
    delete[] xPos;
    delete[] yPos;
    delete[] zPos;
    delete[] rotMatrix;
}

/**
 * @brief adds a new shape to the wrapper
 * @param shape shape to add to wrapper
 */
void sphere::ShapeWrapper::addShape(Shape *shape)
{
    // add the shape position 
    xPos[numElems] = shape->position.x;
    yPos[numElems] = shape->position.y;
    zPos[numElems] = shape->position.z;

    // add the shape's rotation matrix
    rotMatrix[numElems] = shape->inverseRotation[0];
    rotMatrix[numElems + 1 * MAX_OBJECTS] = shape->inverseRotation[1];
    rotMatrix[numElems + 2 * MAX_OBJECTS] = shape->inverseRotation[2];
    rotMatrix[numElems + 3 * MAX_OBJECTS] = shape->inverseRotation[3];
    rotMatrix[numElems + 4 * MAX_OBJECTS] = shape->inverseRotation[4];
    rotMatrix[numElems + 5 * MAX_OBJECTS] = shape->inverseRotation[5];
    rotMatrix[numElems + 6 * MAX_OBJECTS] = shape->inverseRotation[6];
    rotMatrix[numElems + 7 * MAX_OBJECTS] = shape->inverseRotation[7];
    rotMatrix[numElems + 8 * MAX_OBJECTS] = shape->inverseRotation[8];

    // add to the iter counter
    iterCounter += 0.25;
    numIters = static_cast<itype>(std::ceil(iterCounter));
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::ShapeWrapper::fillEmptyPositions()
{
    constexpr ftype lrgVal = 12345678.9;
    
    // fill the shape coordinates with large values first 
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        xPos[idx] = lrgVal;
        yPos[idx] = lrgVal;
        zPos[idx] = lrgVal;
    }

    // fill the rotation matrix with all 1.0 
    for (itype j = 0; j < 9; ++j) {
        for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
            rotMatrix[j*MAX_OBJECTS + idx] = 1.0;
        }
    }
}

/**
 * @brief constructs a PlaneWrapper 
 */
sphere::PlaneWrapper::PlaneWrapper()
{
    xNor = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    yNor = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    zNor = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    displacement = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    planes.reserve(MAX_OBJECTS);
}

/**
 * @brief destroys a PlaneWrapper object
 */
sphere::PlaneWrapper::~PlaneWrapper()
{
    delete[] xNor;
    delete[] yNor;
    delete[] zNor;
    delete[] displacement;
}

/**
 * @brief adds a plane to the wrapper
 * @param plane plane object to be added
 */
void sphere::PlaneWrapper::addPlane(Plane *plane)
{
    // call base class addShape function
    ShapeWrapper::addShape(plane);

    // add the plane to the vector
    planes.push_back(plane);

    // initialize the remaining fields
    xNor[numElems] = plane->normal.x;
    yNor[numElems] = plane->normal.y;
    zNor[numElems] = plane->normal.z;
    displacement[numElems] = plane->displacement;

    // increase the shape counter
    numElems++;
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::PlaneWrapper::fillEmptyPositions()
{
    // call base function to fill position and rot matrix
    ShapeWrapper::fillEmptyPositions();

    // fill the normal vector and the displacement values
    constexpr ftype lrgVal = 12345678.9;
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        xNor[idx] = lrgVal;
        yNor[idx] = lrgVal;
        zNor[idx] = lrgVal;
        displacement[idx] = 114;
    }
}

/**
 * @brief constructs a BoxWrapper 
 */
sphere::BoxWrapper::BoxWrapper()
{
    xExt = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    yExt = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    zExt = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    boxes.reserve(MAX_OBJECTS);
}

/**
 * @brief destroys a BoxWrapper object
 */
sphere::BoxWrapper::~BoxWrapper()
{
    delete[] xExt;
    delete[] yExt;
    delete[] zExt;
}

/**
 * @brief adds a box to the wrapper
 * @param box the box to be added
 */
void sphere::BoxWrapper::addBox(Box *box)
{
    // call base class addShape function
    ShapeWrapper::addShape(box);

    // add the plane to the vector
    boxes.push_back(box);

    // initialize the remaining fields
    xExt[numElems] = box->extents.x;
    yExt[numElems] = box->extents.y;
    zExt[numElems] = box->extents.z;

    // increase the shape counter
    numElems++;    
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::BoxWrapper::fillEmptyPositions()
{
    // call base function to fill position and rot matrix
    ShapeWrapper::fillEmptyPositions();

    // fill the normal vector and the displacement values
    constexpr ftype lrgVal = 12345678.9;
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        xExt[idx] = lrgVal;
        yExt[idx] = lrgVal;
        zExt[idx] = lrgVal;
    }    
}

/**
 * @brief constructs a SphereWrapper 
 */
sphere::SphereWrapper::SphereWrapper()
{
    radiuses = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    spheres.reserve(MAX_OBJECTS);
}

/**
 * @brief destroys a BoxWrapper object
 */
sphere::SphereWrapper::~SphereWrapper()
{
    delete[] radiuses;
}

/**
 * @brief adds a sphere to the wrapper
 * @param sphere the sphere to be added
 */
void sphere::SphereWrapper::addSphere(Sphere *sph)
{
    // call base class addShape function
    ShapeWrapper::addShape(sph);

    // add the plane to the vector
    spheres.push_back(sph);

    // initialize the remaining fields
    radiuses[numElems] = sph->radius;

    // increase the shape counter
    numElems++;    
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::SphereWrapper::fillEmptyPositions()
{
    // call base function to fill position and rot matrix
    ShapeWrapper::fillEmptyPositions();

    // fill the normal vector and the displacement values
    constexpr ftype lrgVal = 12345678.9;
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        radiuses[idx] = lrgVal;
    }    
}


/**
 * @brief constructs a TorusWrapper 
 */
sphere::TorusWrapper::TorusWrapper()
{
    r1s = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    r2s = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    tori.reserve(MAX_OBJECTS);
}

/**
 * @brief destroys a TorusWrapper object
 */
sphere::TorusWrapper::~TorusWrapper()
{
    delete[] r1s;
    delete[] r2s;
}

/**
 * @brief adds a torus to the wrapper
 * @param torus the torus to be added
 */
void sphere::TorusWrapper::addTorus(Torus *torus)
{
    // call base class addShape function
    ShapeWrapper::addShape(torus);

    // add the plane to the vector
    tori.push_back(torus);

    // initialize the remaining fields
    r1s[numElems] = torus->r1;
    r2s[numElems] = torus->r2;

    // increase the shape counter
    numElems++;    
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::TorusWrapper::fillEmptyPositions()
{
    // call base function to fill position and rot matrix
    ShapeWrapper::fillEmptyPositions();

    // fill the normal vector and the displacement values
    constexpr ftype lrgVal = 12345678.9;
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        r1s[idx] = lrgVal;
        r2s[idx] = lrgVal;
    }    
}

/**
 * @brief constructs a OctaWrapper 
 */
sphere::OctaWrapper::OctaWrapper()
{
    s = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    octas.reserve(MAX_OBJECTS);
}

/**
 * @brief destroys a OctaWrapper object
 */
sphere::OctaWrapper::~OctaWrapper()
{
    delete[] s;
}

/**
 * @brief adds a octahedron to the wrapper
 * @param octa the octahedron to be added
 */
void sphere::OctaWrapper::addOcta(Octahedron *octa)
{
    // call base class addShape function
    ShapeWrapper::addShape(octa);

    // add the plane to the vector
    octas.push_back(octa);

    // initialize the remaining fields
    s[numElems] = octa->s;

    // increase the shape counter
    numElems++;    
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::OctaWrapper::fillEmptyPositions()
{
    // call base function to fill position and rot matrix
    ShapeWrapper::fillEmptyPositions();

    // fill the normal vector and the displacement values
    constexpr ftype lrgVal = 12345678.9;
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        s[idx] = lrgVal;
    }    
}

/**
 * @brief constructs a ConeWrapper 
 */
sphere::ConeWrapper::ConeWrapper()
{
    xForm = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    yForm = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    zForm = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    xK1 = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    yK1 = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    xK2 = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    yK2 = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    k2DotInv = new (std::align_val_t(32)) ftype[MAX_OBJECTS]();
    cones.reserve(MAX_OBJECTS);
}

/**
 * @brief destroys a ConeWrapper object
 */
sphere::ConeWrapper::~ConeWrapper()
{
    delete[] xForm;
    delete[] yForm;
    delete[] zForm;
    delete[] xK1;
    delete[] yK1;
    delete[] xK2;
    delete[] yK2;
    delete[] k2DotInv;
}

/**
 * @brief adds a cone to the wrapper
 * @param cone the octahedron to be added
 */
void sphere::ConeWrapper::addCone(Cone *cone)
{
    // call base class addShape function
    ShapeWrapper::addShape(cone);

    // add the plane to the vector
    cones.push_back(cone);

    // initialize the remaining fields
    xForm[numElems] = cone->form.x;
    yForm[numElems] = cone->form.y;
    zForm[numElems] = cone->form.z;
    xK1[numElems] = cone->k1.x;
    yK1[numElems] = cone->k1.y;
    xK2[numElems] = cone->k2.x;
    yK2[numElems] = cone->k2.y;
    k2DotInv[numElems] = cone->k2_dot_inv;

    // increase the shape counter
    numElems++;    
}

/**
 * @brief fills empty positions with very large values such that the computed
 * distance for these entries will certainly be large
 */
void sphere::ConeWrapper::fillEmptyPositions()
{
    // call base function to fill position and rot matrix
    ShapeWrapper::fillEmptyPositions();

    // fill the normal vector and the displacement values
    constexpr ftype lrgVal = 12345678.9;
    for (itype idx = numElems; idx < MAX_OBJECTS; ++idx) {
        xForm[idx] = lrgVal;
        yForm[idx] = lrgVal;
        zForm[idx] = lrgVal;
        xK1[idx] = lrgVal;
        xK2[idx] = lrgVal;
        k2DotInv[idx] = lrgVal;
    }    
}