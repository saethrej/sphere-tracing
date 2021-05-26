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
#include <iomanip>
#include <queue>

#ifdef SPHERE_WITH_OPENMP
#include <omp.h>
#endif // SPHERE_WITH_OPENMP

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
 * @param noOutput flag that, if set, prevents writing the computed image to a file
 */
void sphere::Renderer::renderScene(std::string pathToOutputFile, itype width, itype height, bool noOutput)
{
    this->image = new Image(this->scene->cameraFov, width, height);
    this->renderPixels();
    if (!noOutput) {
        this->writeImageToFile(pathToOutputFile);
    } 
}

/**
 * @brief Renders an image by performing sphere tracing for every pixel and writing its color.
 */
void sphere::Renderer::renderPixels()
{
    Vector ray_origin = this->scene->cameraPos;

    #pragma omp parallel for
    for (itype i = 0; i < this->image->height; ++i){
        for (itype j = 0; j < this->image->width; ++j){
            Vector ray_direction = {
                (VectorVal) image->pixels[i * this->image->width +j].cameraCoord.x, 
                (VectorVal) image->pixels[i * this->image->width +j].cameraCoord.y, 
                1
            };
            ray_direction = ray_direction.normalize();
            Color col = sphereTrace(ray_origin, ray_direction, 0.0);
            this->image->pixels[i * (this->image->width) + j].writeColor(col);
        }
    }
}

/**
 * @brief performs sphereTracing for a given ray starting at a given origin
 * 
 * @param ray_origin starting coordinates of the ray
 * @param ray_direction direction of the ray
 * @param distance distance already considered for this ray
 * @return sphere::Color with which the ray should be shaded
 */
sphere::Color sphere::Renderer::sphereTrace(Vector const &ray_origin, Vector const &ray_direction, ftype distance)
{
    ftype t = 0, d = 0;
    Shape *closestShape;
    ftype minDistance;
    ftype totalDistance;
    std::priority_queue<shape_dist, std::vector<shape_dist>, shape_dist_comp> shape_prio;

    Vector ray = ray_origin + ray_direction * t;
    for (Shape *shape : this->scene->shapes) {
        d = shape->distanceFunction(ray);
        shape_prio.push(shape_dist{d, shape});
    }
    closestShape = shape_prio.top().shape;
    minDistance = shape_prio.top().distance;
    totalDistance = 0;
    shape_prio.pop();

    while((distance + t) < MAX_DISTANCE) {
        ray = ray_origin + ray_direction * t;
        minDistance = closestShape->distanceFunction(ray);
        if(shape_prio.top().distance < (totalDistance + minDistance)){
            shape_prio = std::priority_queue<shape_dist, std::vector<shape_dist>, shape_dist_comp>();
            for (Shape *shape : this->scene->shapes) {
                d = shape->distanceFunction(ray);
                shape_prio.push(shape_dist{d, shape});
            }
            closestShape = shape_prio.top().shape;
            minDistance = shape_prio.top().distance;
            totalDistance = 0;
            shape_prio.pop();
        }
        if(minDistance <= TRACE_THRESHOLD * t){
            return shade(ray, ray_direction, closestShape, distance + t);
        }
        t = t + minDistance;
        totalDistance = totalDistance + minDistance;
    }
    return Color(0,0,0);
}

/**
 * @brief shades an object, taking into consideration 
 * the objcet itself, lightning, reflections and shadows
 * 
 * @param ray intersection of the ray and the object
 * @param ray_normalized direction of the ray normalized
 * @param shape shape with which the ray intersects
 * @param distance distance already considered
 * @return sphere::Color whith which the ray should be shaded
 */
sphere::Color sphere::Renderer::shade(Vector const &ray, Vector const &ray_normalized, Shape *shape, ftype distance)
{
    // create delta vectors and use them to compute the normal vector of the 
    // tangential plane at the point where the ray and the shape intersect
    Vector dx = {NORMAL_DELTA, 0.0, 0.0}, dy = {0.0, NORMAL_DELTA, 0.0}, dz = {0.0, 0.0, NORMAL_DELTA};
    Vector normal = Vector(
        shape->distanceFunction(ray + dx) - shape->distanceFunction(ray - dx),
        shape->distanceFunction(ray + dy) - shape->distanceFunction(ray - dy),
        shape->distanceFunction(ray + dz) - shape->distanceFunction(ray - dz)
    );
    normal = normal.normalize();

    // get the vector of the light point to the intersection point, and compute the
    // dot product of said vector with the normal vector of the tangential plane computed
    // above. If it's larger than zero, this indicates that the ray is hitting the 
    // shape from the front, which means it's important to our image
    Vector light_dir = (this->scene->lightPos - ray);
    Vector light_dir_norm = light_dir.normalize();
    ftype NdotL = (light_dir_norm * normal)/2;
    Color ambient = shape->color;
    Color diffuse = Vector(scene->lightEmi.x/255., scene->lightEmi.y/255., scene->lightEmi.z/255.) * std::max(0.0, NdotL);

    // compute specular highlights
    Vector lightrefl = light_dir_norm - normal * 2 * (light_dir_norm * normal);
    lightrefl = lightrefl.normalize();
    ftype specular_weight_central = pow(lightrefl * ray_normalized, 100.0/shape->shininess);
    ftype specular_weight_middle = pow(lightrefl * ray_normalized, shape->shininess)/2;
    ftype specular_weight_wide = pow(lightrefl * ray_normalized, 2);
    ftype specular_weight_broad = lightrefl * ray_normalized;
    ftype specular_weight = SPECULAR_BIAS * specular_weight_central;
    specular_weight += ((1 - SPECULAR_BIAS)/3) * specular_weight_broad;
    specular_weight += ((1 - SPECULAR_BIAS)/3) * specular_weight_middle;
    specular_weight += ((1 - SPECULAR_BIAS)/3) * specular_weight_middle;
    Color specular = Vector(scene->lightEmi.x/255., scene->lightEmi.y/255., scene->lightEmi.z/255.) * std::max(0.0,specular_weight);
    
    // compute the reflection if object reflects
    Color reflection_color = Color(0,0,0);
    ftype reflection_weight = shape->reflection;
    if(reflection_weight > 0){
        // exp version
        Vector refldir = ray_normalized + normal * 2 * std::exp((ray_normalized * normal) + 1);
        // normal version
        // Vector refldir = ray_normalized - normal * 2 * (ray_normalized * normal);
        refldir = refldir.normalize();
        reflection_color = sphereTrace(ray + normal*REFLECTION_BIAS, refldir, distance);
        if (reflection_color.equals(Color(0,0,0))){
            reflection_weight = reflection_weight/4.0;
        }
    }

    // compute the shadow
    ftype light_len = light_dir.length();
    ftype shadow_weight = shadow(ray, light_dir_norm, light_len);

    // combine the different color values
    Color col = Color();
    //col += (ambient + diffuse + specular) * (1 - reflection_weight) * shadow_weight;
    //col += reflection_color               * reflection_weight       * (shadow_weight + (1-shadow_weight)/4.0);
    col += (ambient + diffuse + specular) * (1 - reflection_weight);
    col += reflection_color               * reflection_weight;
    col.r = std::min(col.r, 1.0f);
    col.g = std::min(col.g, 1.0f);
    col.b = std::min(col.b, 1.0f);
    return col * shadow_weight;
    //return col;
}

/**
 * @brief returns a weight for the shadow depending on the presence
 * of an object between the ray_to_shade and the light source
 * 
 * @param ray_to_shade origin of the ray
 * @param lightDir direction towards the light source (normalized)
 * @param dist maximum distance considered
 * @return sphere::ftype weight of the shadowing 
 */
sphere::ftype sphere::Renderer::shadow(Vector const &ray_to_shade, Vector const &lightDir, ftype dist)
{
    ftype shadow_weight = 1.0;
    ftype shadow_step = SHADOW_MAX/(1.0 + SHADOW_CIRCLES*4.0);
    std::tuple<bool, bool, bool> axes = lightDir.shadowAxes();

    // always shoot at least one ray and determine if object in between
    if(ObjectInBetween(ray_to_shade, lightDir, dist)){
        shadow_weight -= shadow_step;
    }

    // shoot four more rays per circle defined
    Vector dx = {SHADOW_DELTA, 0.0, 0.0}, dy = {0.0, SHADOW_DELTA, 0.0}, dz = {0.0, 0.0, SHADOW_DELTA};
    for(itype i = 0; i < SHADOW_CIRCLES; ++i){
        if(std::get<0>(axes)){
            if(ObjectInBetween(ray_to_shade, lightDir + dx * (i + 1), dist)){
                shadow_weight -= shadow_step;
            }
            if(ObjectInBetween(ray_to_shade, lightDir - dx * (i + 1), dist)){
                shadow_weight -= shadow_step;
            }
        }
        if(std::get<1>(axes)){
            if(ObjectInBetween(ray_to_shade, lightDir + dy * (i + 1), dist)){
                shadow_weight -= shadow_step;
            }
            if(ObjectInBetween(ray_to_shade, lightDir - dy * (i + 1), dist)){
                shadow_weight -= shadow_step;
            }
        }
        if(std::get<2>(axes)){
            if(ObjectInBetween(ray_to_shade, lightDir + dz * (i + 1), dist)){
                shadow_weight -= shadow_step;
            }
            if(ObjectInBetween(ray_to_shade, lightDir - dz * (i + 1), dist)){
                shadow_weight -= shadow_step;
            }
        }
    }

    return shadow_weight;
}

/**
 * @brief returns true if there is an object starting from ray_origin in ray_direction within max_distance
 * 
 * @param ray_origin origin of the ray
 * @param ray_direction direction of the ray
 * @param max_dist max distance from the ray origin in ray direction considered
 * @return true if object in between
 * @return false if no object in between
 */
bool sphere::Renderer::ObjectInBetween(Vector const &ray_origin, Vector const &ray_direction, ftype max_dist)
{
    ftype t = 0, d = 0;
    Shape *closestShape;
    ftype minDistance;
    ftype totalDistance;
    std::priority_queue<shape_dist, std::vector<shape_dist>, shape_dist_comp> shape_prio;

    Vector ray = ray_origin + ray_direction * t;
    for (Shape *shape : this->scene->shapes) {
        d = shape->distanceFunction(ray);
        shape_prio.push(shape_dist{d, shape});
    }
    closestShape = shape_prio.top().shape;
    minDistance = shape_prio.top().distance;
    totalDistance = 0;
    shape_prio.pop();

    while(t < max_dist) {
        ray = ray_origin + ray_direction * t;
        minDistance = closestShape->distanceFunction(ray);
        if(shape_prio.top().distance < (totalDistance + minDistance)){
            shape_prio = std::priority_queue<shape_dist, std::vector<shape_dist>, shape_dist_comp>();
            for (Shape *shape : this->scene->shapes) {
                d = shape->distanceFunction(ray);
                shape_prio.push(shape_dist{d, shape});
            }
            closestShape = shape_prio.top().shape;
            minDistance = shape_prio.top().distance;
            totalDistance = 0;
            shape_prio.pop();
        }
        if(minDistance <= SHADOW_THRESHOLD * t){
            return true;
        }
        t = t + minDistance;
        totalDistance = totalDistance + minDistance;
    }
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

// microbenchmarking functionality
#ifdef SPHERE_WITH_MICROBENCHMARKS

/**
 * @brief microbenchmarks all distance functions of shapes that are part
 * of the current scene and prints them to a file
 * 
 * @details If you want to write your own microbenchmark function, you can
 * follow the approach of this function. You have to do the following steps:
 * 
 * 1.) initialize the TSC (cycle counter) via TSC_INIT() macro
 * 2.) prepare all of the arguments of the function you want to microbenchmark
 * 3.) create a lambda function out of this function. There should be not arguments
 *     because TSC_MEASURE() has to call the function without arguments, and thus
 *     every argument has to be captured inside the [] clause.
 * 4.) pass the function to the TSC_MEASURE() macro to start microbenchmarking
 * 5.) get the number of cycles elapsed via TSC_GET() and clear via TSC_CLEAR()
 *     to get ready for the next function
 */
void sphere::Renderer::microbenchmarkDistanceFunctions()
{
    // intialize microbenchmarking
    TSC_INIT();

    // use some hard-coded vector for the microbenchmarking
    Vector testVec(M_PI, M_E, M_SQRT2);

    // open a file to write the distance functions
    std::ofstream out("../benchmarks/microbenchmark-df.txt", std::ios::out);

    // microbenchmark the distance functions individually by looping over all
    // shapes in the container
    for (Shape *shape : this->scene->shapes) {
        // microbenchmark the distance function
        TSC_CLEAR();
        //auto func = std::bind(&Shape::distanceFunction, shape, testVec);
        auto func = [shape, testVec] () {
            shape->distanceFunction(testVec);
        };
        TSC_MEASURE(func);
        double cycles = TSC_GET();
        
        // determine the type of the shape and print the result to the file
        out << std::left << std::setw(12) << shape->name << std::right 
            << std::setw(12) << std::fixed << std::setprecision(1) << cycles << " cycles\n";
    }
    // close the file and end microbenchmarking
    out.close();
}
#endif // SPHERE_WITH_MICROBENCHMARKS