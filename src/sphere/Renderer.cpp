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
 * @file Renderer.cpp
 * 
 * @brief Implementation of renderer object that is responsible for the entire logic of
 * rendering an image.
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 29.04.2021
 */

#include <fstream>
#include <iostream>

#include "Renderer.h"

/**
 * @brief Construct a new empty sphere::Renderer::Renderer object.
 * 
 */
sphere::Renderer::Renderer()
{
    this->scene = nullptr;
    this->image = nullptr;
}

/**
 * @brief Destroy the sphere::Renderer::Renderer object.
 * 
 */
sphere::Renderer::~Renderer()
{
    if (this->scene != nullptr) {
        delete this->scene;
    }
    if (this->image != nullptr) {
        delete this->image;
    }
}

/**
 * @brief Adds a new scene to the Renderer by constructing a new Scene object
 * 
 * @param pathToSceneFile path to the scene description json file
 */
void sphere::Renderer::addScene(std::string pathToSceneFile)
{
    this->scene = new Scene(pathToSceneFile);
}

/**
 * @brief Renders a given scene and writes the image to a file in .ppm format
 * 
 * Initalizes a image object, then renders the scene and stores it in the image object
 * which is then written into a .ppm file.
 * 
 * @param pathToOutputFile path to output file in which the image is written, should be of type .ppm
 * @param width width of the image that is generated.
 * @param height height of the image that is generated.
 */
void sphere::Renderer::renderScene(std::string pathToOutputFile, itype width, itype height)
{
    this->image = new Image(this->scene->cameraFov, width, height);
    this->renderPixel();
    this->writeImageToFile(pathToOutputFile);
}

/**
 * @brief Renders all pixel for a given scene in a given image
 */
void sphere::Renderer::renderPixel()
{

    for (itype i = 0; i < this->image->height; ++i){
        for (itype j = 0; j < this->image->width; ++j){
            sphereTrace(image->pixels[i * this->image->width +j].cameraCoord.x, image->pixels[i * this->image->width +j].cameraCoord.y, i,j);
        }
    }
}

/**
 * @brief executes sphereTracing for the pixel at pos [y][x]
 * 
 * #TODO as soon as algo actually implemented
 * 
 * @param pix_x row index of pixel
 * @param pix_y column index of pixel
 */
void sphere::Renderer::sphereTrace(ftype pix_x, ftype pix_y, itype imageCoordx, itype imageCoordy)
{
    // make sphere tracing for this->image->pixel[y*width + x]
    // and use this->image->pixel[y*width + x].writeColor(r, g, b) to writeColor;

    //TODO the computation of the rayDirection (what if camerapos is not 0,0,0)?
    Vector rayOrigin = this->scene->cameraPos;
    Vector rayDirection = {(VectorVal) pix_x, (VectorVal) pix_y, 1};
    Vector rayDirection_normalized =  rayDirection.normalize();

    constexpr itype maxDistance = 100;
    ftype t = 0;
    ftype d = 0;
    constexpr ftype threshold = 10e-15; 
    Shape *closestShape;
    while (t < maxDistance) {
        //computes rayOrigin + t*rayDirection
        Vector ray_to_shape = rayOrigin + rayDirection_normalized * t;
        ftype minDistance = std::numeric_limits<ftype>::max();

        for (Shape *shape : this->scene->shapes) {
            d = shape->distanceFunction(ray_to_shape);
            if (d < minDistance) {
                minDistance = d;
                closestShape = shape;
            }
        }
        if (minDistance <= threshold * t) {
                //intersection, this->scene->shapes[i]
                Color col = shade(ray_to_shape, closestShape);
                this->image->pixels[imageCoordx * (this->image->width) + imageCoordy].writeColor(col);
                return;
            }
        t = t + minDistance;
    }
    //no intersection
}

/**
 * @brief computes the color for the pixel
 * @param ray Vector from rayOrigin to shape
 * @returns the color
 */
sphere::Color sphere::Renderer::shade(Vector const &ray, Shape *shape)
{
    // set delta that is used for computing the derivative
    //constexpr ftype delta = 10e-2;

    // create delta vectors and use them to compute the normal vector of the 
    // tangential plane at the point where the ray and the shape intersect
    /*
    Vector dx = {delta, 0.0, 0.0}, dy = {0.0, delta, 0.0}, dz = {0.0, 0.0, delta};
    Vector normal = Vector(
        shape->distanceFunction(ray + dx) - shape->distanceFunction(ray - dx),
        shape->distanceFunction(ray + dy) - shape->distanceFunction(ray - dy),
        shape->distanceFunction(ray + dz) - shape->distanceFunction(ray - dz)
    ).normalize();

    // get the vector of the light point to the intersection point, and compute the
    // dot product of said  with the normal vector of the tangential plane computed
    // above. If it's larger than zero, this indicates that theray is hitting the 
    // shape from the front, which means it's important to our image
    Vector lightItsct = this->scene->lightPos - ray;
    ftype dotProd = lightItsct * normal;

    Color col = Color(); // initially black
    if (dotProd > 0) {
        ftype dist = lightItsct.length();
        bool shadowFlag = 1;// - shadow(ray, lightItsct, dist);
        col += static_cast<Color>(this->scene->lightEmi * (shadowFlag * dotProd / (4.0 * M_PI * dist)));
    }

    return col;
    */
    
    constexpr ftype delta = 10e-5;

    // create delta vectors and use them to compute the normal vector of the 
    // tangential plane at the point where the ray and the shape intersect
    Vector dx = {delta, 0.0, 0.0}, dy = {0.0, delta, 0.0}, dz = {0.0, 0.0, delta};
    Vector normal = Vector(
        shape->distanceFunction(ray + dx) - shape->distanceFunction(ray - dx),
        shape->distanceFunction(ray + dy) - shape->distanceFunction(ray - dy),
        shape->distanceFunction(ray + dz) - shape->distanceFunction(ray - dz)
    );
    normal = normal.normalize();
    // get the vector of the light point to the intersection point, and compute the
    // dot product of said  with the normal vector of the tangential plane computed
    // above. If it's larger than zero, this indicates that theray is hitting the 
    // shape from the front, which means it's important to our image
    Vector lightItsct = (this->scene->lightPos - ray);
    ftype NdotL = lightItsct.normalize() * normal;
    Vector bisector = (ray + lightItsct).normalize();
    ftype NdotH = pow(normal * bisector, shape->shininess);
    Color ambient = shape->color;
    Color diffuse = Vector(scene->lightEmi.x/255., scene->lightEmi.y/255., scene->lightEmi.z/255.) * std::max(0.0, NdotL);
    Color specular = Vector(scene->lightEmi.x/255., scene->lightEmi.y/255., scene->lightEmi.z/255.) * std::max(0.0,NdotH);
    Color col = Color(); // initially black
    bool shadowflag = shadow(ray, lightItsct.normalize(), lightItsct.length());
    
    if(shadowflag){ //&& shape->type == sphere::ShapeType::PLANE){
        col.r = 0;//col.r * 0.0;
        col.g = 0;//col.g * 0.0;
        col.b = 0;//col.b * 0;
        return col;
    }else{
    col += ambient + diffuse + specular;
    col.r = std::min(col.r, 1.0f);
    col.g = std::min(col.g, 1.0f);
    col.b = std::min(col.b, 1.0f);
    }
    return col;
}

/**
 * @brief returns true iff there is a shape between the point and the light 
 * 
 * @param ray_to_shape Vector from rayOrigin to shape
 * @param lightDir direction of the light ray
 * @param dist maxDistance 
 * @returns true iff there is a shape between the point and the light
 */
bool sphere::Renderer::shadow(Vector const &ray_to_shape, Vector lightDir, ftype dist)
{
    constexpr ftype threshold = 10e-8; 
    ftype t = 0, d = 0;
    ftype maxDist = dist;
    // determine the nearest element in the current step

    while (t < maxDist) {
      Vector from = ray_to_shape + lightDir * t;
        // iterate over all shapes to determine the nearest
        ftype minDist = std::numeric_limits<ftype>::max();
        for (Shape *shape : this->scene->shapes) {
            d = shape->distanceFunction(from);
            // keep track of the smallest distance seen so far
            if (d < minDist){
                minDist = d;
            } 
            if (minDist <= threshold * t){
                return true;
            }
            // return true if the smallest distance is below the threshold
           
        }
        
        t += minDist;
    }
    // if no object is in between, return false
    return false;
}


/**
 * @brief Writes the image into a file in .ppm format
 * 
 * @param pathToFile path to output file in which the image is written, should be of type .ppm
 */
void sphere::Renderer::writeImageToFile(std::string pathToFile)
{
    unsigned char r, g, b;
    std::ofstream outstream;
    outstream.open(pathToFile);
    outstream << "P6\n" << this->image->width << " " << this->image->height << "\n255\n";
    for(itype i = 0; i < this->image->width*this->image->height; ++i) {
        r = static_cast<unsigned char>(std::max(1.0f, this->image->pixels[i].color.r * 255));
        g = static_cast<unsigned char>(std::max(1.0f, this->image->pixels[i].color.g * 255));
        b = static_cast<unsigned char>(std::max(1.0f, this->image->pixels[i].color.b * 255));
        outstream << r << g << b;
    }
    outstream.close();
}