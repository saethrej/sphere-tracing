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
    if (this->scene != nullptr)
    {
        delete this->scene;
    }
    if (this->image != image)
    {
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
 * 
 */
void sphere::Renderer::renderPixel()
{
    for(itype i = 0; i < this->image->height; ++i){
        for(itype j = 0; j < this->image->width; ++j){
            sphereTrace(i, j);
        }
    }
}

/**
 * @brief executes sphereTracing for the pixel at pos [y][x]
 * 
 * #TODO as soon as algo actually implemented
 * 
 * @param pix_y row index of pixel
 * @param pix_x column index of pixel
 */
void sphere::Renderer::sphereTrace(itype pix_y, itype pix_x)
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
    constexpr ftype threshold = 10e-2; 
    while(t < maxDistance){
        //computes rayOrigin + t*rayDirection
        Vector ray_to_shape = Vector(rayOrigin.x + t * rayDirection.x, rayOrigin.y + t * rayDirection.y,  rayOrigin.z + t * rayDirection.z);
        ftype minDistance = std::numeric_limits<ftype>::max();
        for(itype i=0; i < this->scene->numShapes; ++i){
            d = this->scene->shapes[i]->distanceFunction(&ray_to_shape);
            if(d < minDistance){
                minDistance = d;
            }
            if(minDistance <= threshold * t){
                //intersection, this->scene->shapes[i]
                Vector col = shade(&ray_to_shape, this->scene->shapes[i]);
                //this->image->pixel[pix_y * (this->image->width) + pix_x].writeColor(col.x,col.y, col.z);
                this->image->pixel[pix_y * (this->image->width) + pix_x].writeColor(col.x,col.y, col.z);
                return;
            }
        }
        t = t + minDistance;
    }
    //no intersection
}


/**
 * @brief computes the color for the pixel
 * 
 * 
 * @param ray_to_shape Vector from rayOrigin to shape
 */
sphere::Vector sphere::Renderer::shade(Vector const *ray_to_shape, Shape *shape)
{
    constexpr ftype delta = 10e-5;
    Vector c_1 = {ray_to_shape->x + delta, ray_to_shape->y, ray_to_shape->z};
    Vector c_2 = {ray_to_shape->x - delta, ray_to_shape->y, ray_to_shape->z};
    Vector *c_x_1 = &c_1;
    Vector *c_x_2 = &c_2;
    ftype x1 = shape->distanceFunction(c_x_1) - shape->distanceFunction(c_x_2);
    c_1 = {ray_to_shape->x, ray_to_shape->y+delta, ray_to_shape->z};
    c_2 = {ray_to_shape->x, ray_to_shape->y-delta, ray_to_shape->z};
    Vector *c_y_1 = &c_1;
    Vector *c_y_2 = &c_2;
    ftype y1 = shape->distanceFunction(c_y_1) - shape->distanceFunction(c_y_2);
    c_1 = {ray_to_shape->x, ray_to_shape->y, ray_to_shape->z+delta};
    c_2 = {ray_to_shape->x, ray_to_shape->y, ray_to_shape->z-delta};
    Vector *c_z_1 = &c_1;
    Vector *c_z_2 = &c_2;
    ftype z1 = shape->distanceFunction(c_z_1) - shape->distanceFunction(c_z_2);
    Vector n = {x1, y1, z1};
    n.normalize();
    Vector light_to_intersection = {this->scene->lightPos.x - ray_to_shape->x, this->scene->lightPos.y - ray_to_shape->y, this->scene->lightPos.z - ray_to_shape->z};
    Vector R = {0,0,0};
    ftype lightDir_n_dotproduct = {light_to_intersection.x * n.x + light_to_intersection.y *n.y + light_to_intersection.z * n.z};
    if( lightDir_n_dotproduct > 0){
        ftype dist = light_to_intersection.x * light_to_intersection.x + light_to_intersection.y * light_to_intersection.y + light_to_intersection.z * light_to_intersection.z;
        dist = sqrtf(dist); 
        bool sha = 1 - shadow(ray_to_shape, light_to_intersection, dist);
        //TODO light->col, light->emission
        R += {sha * lightDir_n_dotproduct * this->scene->lightEmi.x  / (4 * M_PI * dist),sha * lightDir_n_dotproduct * this->scene->lightEmi.y  / (4 * M_PI * dist),sha * lightDir_n_dotproduct * this->scene->lightEmi.x  / (4 * M_PI * dist)}  ;
       // R += shadow * lightDir.dotProduct(n) * light->col * light->intensity / (4 * M_PI * dist2);
    }
   
    return R;
}

/**
 * @brief returns true if there is a shape between the point and the light 
 * 
 * 
 * @param ray_to_shape Vector from rayOrigin to shape
 * @param lightDir direction of the light ray
 * @param dist maxDistance 
 */
bool sphere::Renderer::shadow(Vector const *ray_to_shape, Vector lightDir, ftype dist)
{
    constexpr ftype threshold = 10e-2; 
    ftype t = 0;
    ftype d = 0;
    ftype maxDistance = dist;
    Vector *from = new Vector(ray_to_shape->x + t * lightDir.x, ray_to_shape->y + t * lightDir.y,  ray_to_shape->z + t * lightDir.z);
    while (t < maxDistance){
        ftype minDistance = std::numeric_limits<ftype>::max();
        for (itype i=0;i<this->scene->numShapes;++i){
            d = this->scene->shapes[i]->distanceFunction(from);
            if (d < minDistance){
                minDistance = d;
            }
            if (minDistance <= threshold * t){
                return true;
            }
        }
        t = t + minDistance;
    }
    delete from;
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
        r = static_cast<unsigned char>(std::min(1.0f, this->image->pixel[i].color.r * 255));
        g = static_cast<unsigned char>(std::min(1.0f, this->image->pixel[i].color.g * 255));
        b = static_cast<unsigned char>(std::min(1.0f, this->image->pixel[i].color.b * 255));
        outstream << r << g << b;
    }
    outstream.close();
}