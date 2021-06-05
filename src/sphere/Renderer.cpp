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
    this->distances = nullptr;
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
    if (this->distances != nullptr) {
        delete this->distances;
    }
}

/**
 * @brief Adds a new scene to the Renderer by constructing a new Scene object
 * 
 * @param pathToSceneFile path to the scene description json file
 */
void sphere::Renderer::addScene(std::string pathToSceneFile)
{
    // add new scene
    this->scene = new Scene(pathToSceneFile);
    
    // determine how many spaces are required for the distances - We use a lambda
    // function for it
    auto rndToFour = [] (itype numShapes) -> itype {
        return 4 * (numShapes / 4 + ((numShapes % 4) == 0 ? 0 : 1));
    };
    this->numDist = rndToFour(scene->wBox->numElems) + rndToFour(scene->wCone->numElems)
                  + rndToFour(scene->wPlane->numElems) + rndToFour(scene->wOcta->numElems)
                  + rndToFour(scene->wSphere->numElems) + rndToFour(scene->wTorus->numElems);
    #ifdef SPHERE_WITH_OPENMP
    itype numThreads = omp_get_max_threads();
    #else
    itype numThreads = 1;
    #endif
    this->distances = new (std::align_val_t(32)) ftype[this->numDist * numThreads];
    
    // fill the distances with random large values that are later overwritten if 
    // there is a valid shape at this position
    for (itype i = 0; i < this->numDist; ++i) {
        this->distances[i] = 12481241241.12412;
    }

    // compute the thresholds to assign a shape to an index
    this->threshBox = rndToFour(scene->wBox->numElems);
    this->threshCone = this->threshBox + rndToFour(scene->wCone->numElems);
    this->threshOcta = this->threshCone + rndToFour(scene->wOcta->numElems);
    this->threshPlane = this->threshOcta + rndToFour(scene->wPlane->numElems);
    this->threshSphere = this->threshPlane + rndToFour(scene->wSphere->numElems);
    this->threshTorus = this->threshSphere + rndToFour(scene->wTorus->numElems);
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
 * @brief computes the min-distances among all shapes for the current ray
 * 
 * @param minDist [OUT] the computed minimum distance
 * @param min2Dist [OUT] the computed second minimum distance
 * @param closestShape [OUT] pointer to the closest shape
 * @param ray [IN] current ray position for which to compute distance
 */
void sphere::Renderer::getMinDistances(ftype &minDist, ftype &min2Dist, Shape *&closestShape, Vector const &ray)
{
    // compute distances to all shape types individually
    ftype *destPtr = this->distances + numDist * omp_get_thread_num();
    ftype *dists = destPtr;

    BoxWrapper *box = scene->wBox;
    for (itype i = 0; i < box->numIters; ++i) {
        Box::vectDistFunc(box, ray, i * 4, destPtr);
        destPtr += 4;
    }
    ConeWrapper *cone = scene->wCone;
    for (itype i = 0; i < cone->numIters; ++i) {
        Cone::vectDistFunc(cone, ray, i * 4, destPtr);
        destPtr += 4;
    }
    OctaWrapper *octa = scene->wOcta;
    for (itype i = 0; i < octa->numIters; ++i) {
        Octahedron::vectDistFunc(octa, ray, i * 4, destPtr);
        destPtr += 4;
    }
    PlaneWrapper *plane = scene->wPlane;
    for (itype i = 0; i < plane->numIters; ++i) {
        Plane::vectDistFunc(plane, ray, i * 4, destPtr);
        destPtr += 4;
    }
    SphereWrapper *sphe = scene->wSphere;
    for (itype i = 0; i < sphe->numIters; ++i) {
        Sphere::vectDistFunc(sphe, ray, i * 4, destPtr);
        destPtr += 4;
    }
    TorusWrapper *torus = scene->wTorus;
    for (itype i = 0; i < torus->numIters; ++i) {
        Torus::vectDistFunc(torus, ray, i * 4, destPtr);
        destPtr += 4;
    }

    // get the minimum distance, second minimum distance
    minDist = min2Dist = std::numeric_limits<ftype>::max();
    itype minIdx = -1;
    for (itype i = 0; i < numDist; ++i) {
        if (dists[i] < minDist) {
            min2Dist = minDist;
            minDist = dists[i];
            minIdx = i;

        } else if (dists[i] < min2Dist) {
            min2Dist = dists[i];
        }
    }
    
    // determine the closest shape
    if (minIdx < threshBox) {
        closestShape = box->boxes[minIdx];
    } else if (minIdx < threshCone) {
        closestShape = cone->cones[minIdx - threshBox];
    } else if (minIdx < threshOcta) {
        closestShape = octa->octas[minIdx - threshCone];
    } else if (minIdx < threshPlane) {
        closestShape = plane->planes[minIdx - threshOcta];
    } else if (minIdx < threshSphere) {
        closestShape = sphe->spheres[minIdx - threshPlane];
    } else if (minIdx < threshTorus) {
        closestShape = torus->tori[minIdx - threshSphere];
    }
}

/**
 * @brief computes the min distances among all shapes in a vectorized way
 * 
 * @param minDist [OUT] the minimum distance that was encountered
 * @param min2Dist [OUT] the second minimum distance encountered
 * @param closest pointer to the closest shape
 * @param ray the ray to compute distance to objects
 */
void sphere::Renderer::getMinDistancesVectorized(ftype &minDist, ftype &min2Dist, Shape *&closest, Vector const &ray)
{
    ftype d = std::numeric_limits<ftype>::max();

    // iterate over each types of shapes individually
    
}

/**
 * @brief Renders an image by performing sphere tracing for every pixel and writing its color.
 */
void sphere::Renderer::renderPixels()
{
    // pre-compute min-distances in the first sphere tracing step

    Vector ray_origin = this->scene->cameraPos;
    ftype minDistance = std::numeric_limits<ftype>::max();
    ftype min2Distance = std::numeric_limits<ftype>::max();
    Shape *closestShape;
    getMinDistances(minDistance, min2Distance, closestShape, ray_origin);

    // render each pixel individually, with OpenMP threads
    #pragma omp parallel for
    for (itype i = 0; i < this->image->height; ++i) {
        for (itype j = 0; j < this->image->width; ++j) {
            Vector ray_direction(
                image->pixels[i * this->image->width +j].cameraCoord.x, 
                image->pixels[i * this->image->width +j].cameraCoord.y, 
                1.0
            );
            ray_direction = ray_direction.normalize();
            Color col = sphereTrace(ray_origin, ray_direction, 0.0, closestShape, minDistance, min2Distance);
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
sphere::Color sphere::Renderer::sphereTrace(Vector const &ray_origin, Vector const &ray_direction, ftype distance, 
        Shape *const firstShape, ftype const firstDistance, ftype const secondDistance)
{
    Shape *closestShape = firstShape;
    ftype minDistance = firstDistance;
    ftype min2Distance = secondDistance;
    Vector ray = ray_origin;

    if (minDistance <= 0.0) {
        return shade(ray, ray_direction, closestShape, distance);
    }
    ftype t = minDistance;
    ftype totalDistance = minDistance;

    while ((distance + t) < MAX_DISTANCE) {
        ray = ray_origin + ray_direction * t;
        minDistance = closestShape->distanceFunction(ray);
        totalDistance = totalDistance + minDistance;
        if (min2Distance < totalDistance) {
            minDistance = std::numeric_limits<ftype>::max();
            min2Distance = std::numeric_limits<ftype>::max();
            getMinDistances(minDistance, min2Distance, closestShape, ray);
            totalDistance = minDistance;
        }
        if (minDistance <= TRACE_THRESHOLD * t) {
            COUNT_OPS(1);
            return shade(ray, ray_direction, closestShape, distance + t);
        }
        t = t + minDistance;
        COUNT_OPS(4);
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
    Vector dx(NORMAL_DELTA, 0.0, 0.0), dy(0.0, NORMAL_DELTA, 0.0), dz(0.0, 0.0, NORMAL_DELTA);
    Vector normal(
        shape->distanceFunction(ray + dx) - shape->distanceFunction(ray - dx),
        shape->distanceFunction(ray + dy) - shape->distanceFunction(ray - dy),
        shape->distanceFunction(ray + dz) - shape->distanceFunction(ray - dz)
    );
    normal = normal.normalize();

    // get the vector of the light point to the intersection point, and compute the
    // dot product of said vector with the normal vector of the tangential plane computed
    // above. If it's larger than zero, this indicates that the ray is hitting the 
    // shape from the front, which means it's important to our image
    constexpr ftype colorScale = 1.0/255.;
    Vector scaledLightEmission = scene->lightEmi * colorScale;
    Vector L = (this->scene->lightPos - ray);
    Vector L_norm = L.normalize();
    ftype NdotL = L_norm * normal;
    ftype NdotL_half = NdotL * 0.5;
    Color ambient = shape->color;
    Color diffuse = scaledLightEmission * std::max(0.0, NdotL_half);
    Vector refl =  L_norm - normal * (2.0 * NdotL);//L_norm - normal * NdotL;

    // compute specular highlights
    Vector refl_norm = refl.normalize();
    ftype RefldotRay = refl_norm * ray_normalized;
    ftype scaledShininess = 100.0/shape->shininess;

    ftype specular_weight_central = pow(RefldotRay, scaledShininess);
    ftype specular_weight_middle = pow(RefldotRay, shape->shininess) * 0.5;
    ftype specular_weight_wide = RefldotRay * RefldotRay;
    ftype specular_weight_broad = RefldotRay;
    constexpr ftype specularFactor = 0.33333333333333 - SPECULAR_BIAS_THIRD;
    ftype specularMiddleSummand = specularFactor * specular_weight_middle * 2.0;
    ftype specular_weight = SPECULAR_BIAS * specular_weight_central 
                            + specularFactor * specular_weight_broad 
                            + specularMiddleSummand;

    Color specular = scaledLightEmission * std::max(0.0, specular_weight);  //cannot be negative std::max(0.0,specular_weight);

    // compute the reflection if object reflects
    Color reflection_color(0.f, 0.f, 0.f);
    ftype reflection_weight = shape->reflection;
    COUNT_OPS(72);
    if (reflection_weight > 0) {
        Vector refldir = ray_normalized + normal * (2.0 * std::exp((ray_normalized * normal) + 1.0));
        refldir = refldir.normalize();

        ftype minDistance = std::numeric_limits<ftype>::max();
        ftype min2Distance = std::numeric_limits<ftype>::max();
        Shape *closestShape;
        Vector start_ray = ray + normal*REFLECTION_BIAS;
        getMinDistances(minDistance, min2Distance, closestShape, start_ray);

        reflection_color = sphereTrace(start_ray, refldir, distance, closestShape, minDistance, min2Distance);     
        COUNT_OPS(32);
        if (reflection_color.equals(Color(0.f, 0.f, 0.f))) {
            reflection_weight = reflection_weight * 0.25;
            COUNT_OPS(1);
        }
    }

    // compute the shadow
    ftype light_len = L.length();
    ftype shadow_weight = shadow(ray, L_norm, light_len);

    // combine the different color values
    Color col = (ambient + diffuse + specular) * (1.0 - reflection_weight) + reflection_color * reflection_weight;
    //col += (ambient + diffuse + specular) * (1 - reflection_weight) * shadow_weight;
    //col += reflection_color               * reflection_weight       * (shadow_weight + (1-shadow_weight)/4.0);

    // @TODO: do we need this here?
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
    std::tuple<bool,bool,bool> axes = lightDir.shadowAxes();

    // always shoot at least one ray and determine if object in between
    if (ObjectInBetween(ray_to_shade, lightDir, dist)) {
        shadow_weight -= SHADOW_STEP;
        COUNT_OPS(1);
    }

    // shoot four more rays per circle defined
    Vector dx(SHADOW_DELTA, 0.0, 0.0), dy(0.0, SHADOW_DELTA, 0.0), dz(0.0, 0.0, SHADOW_DELTA);
    for (itype i = 0; i < SHADOW_CIRCLES; ++i) {
        if (std::get<0>(axes)) {
            if (ObjectInBetween(ray_to_shade, lightDir + dx * (i + 1), dist)) {
                shadow_weight -= SHADOW_STEP;
                COUNT_OPS(1);
            }
            if (ObjectInBetween(ray_to_shade, lightDir - dx * (i + 1), dist)) {
                shadow_weight -= SHADOW_STEP;
                COUNT_OPS(1);
            }
        }
        if (std::get<1>(axes)) {
            if (ObjectInBetween(ray_to_shade, lightDir + dy * (i + 1), dist)) {
                shadow_weight -= SHADOW_STEP;
                COUNT_OPS(1);
            }
            if (ObjectInBetween(ray_to_shade, lightDir - dy * (i + 1), dist)) {
                shadow_weight -= SHADOW_STEP;
                COUNT_OPS(1);
            }
        }
        if (std::get<2>(axes)) {
            if (ObjectInBetween(ray_to_shade, lightDir + dz * (i + 1), dist)) {
                shadow_weight -= SHADOW_STEP;
                COUNT_OPS(1);
            }
            if (ObjectInBetween(ray_to_shade, lightDir - dz * (i + 1), dist)) {
                shadow_weight -= SHADOW_STEP;
                COUNT_OPS(1);
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
    Shape *closestShape;
    ftype minDistance = std::numeric_limits<ftype>::max();
    ftype min2Distance = std::numeric_limits<ftype>::max();
    Vector ray = ray_origin;

    getMinDistances(minDistance, min2Distance, closestShape, ray);
    if (minDistance <= 0) {
        return true;
    }
    ftype t = minDistance;
    ftype totalDistance = minDistance;

    while (t < max_dist) {
        ray = ray_origin + ray_direction * t;
        minDistance = closestShape->distanceFunction(ray);
        totalDistance = totalDistance + minDistance;
        if (min2Distance < (totalDistance)) {
            minDistance = std::numeric_limits<ftype>::max();
            min2Distance = std::numeric_limits<ftype>::max();
            getMinDistances(minDistance, min2Distance, closestShape, ray);
            totalDistance = minDistance;
        }
        if (minDistance <= SHADOW_THRESHOLD * t) {
            return true;
        }
        t = t + minDistance;
        COUNT_OPS(3);
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
    // open a file stream and write image metadata
    unsigned char r, g, b;
    std::ofstream outstream;
    outstream.open(pathToFile);
    outstream << "P6\n" << this->image->width << " " << this->image->height << "\n255\n";
    
    // write the individual pixel values
    for (itype i = 0; i < this->image->width*this->image->height; ++i) {
        r = static_cast<unsigned char>(std::clamp(this->image->pixels[i].color.r * 255., 0., 255.));
        g = static_cast<unsigned char>(std::clamp(this->image->pixels[i].color.g * 255., 0., 255.));
        b = static_cast<unsigned char>(std::clamp(this->image->pixels[i].color.b * 255., 0., 255.));
        outstream << r << g << b;
          COUNT_OPS(3);
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
    ftype dist = 0.0;
    for (Shape *shape : this->scene->shapes) {
        // microbenchmark the distance function
        TSC_CLEAR();
        auto func = [this, &testVec, &dist] () {
            Spheres s = {this->scene->spheres[0], this->scene->spheres[1], this->scene->spheres[2], this->scene->spheres[3]};
            Distances d = Sphere::vectDistFunc(s, testVec);
            dist += d.d0;
        };
        TSC_MEASURE(func);
        double cycles = TSC_GET();

        out << std::left << std::setw(12) << shape->name << std::right 
            << std::setw(12) << std::fixed << std::setprecision(1) << cycles << " cycles\n"
            << "dist = " << dist << "\n";
    }
    // close the file and end microbenchmarking
    out.close();
}
#endif // SPHERE_WITH_MICROBENCHMARKS