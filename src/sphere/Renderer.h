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
 * @file Renderer.h
 * 
 * @brief Declaration of a class that defines a renderer that is responsible for the entire
 * logic of rendering an image.
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 25.04.2021
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "SphereTypes.h"
#include "ImageClasses.h"
#include "Scene.h"

namespace sphere {

// maximum distance of a ray traced (including reflections)
constexpr itype MAX_DISTANCE = 100;
// threshold for distance to object (sphere tracing)
constexpr ftype TRACE_THRESHOLD = 10e-12;
// delta used to compute the normal vector on an object surface
constexpr ftype NORMAL_DELTA = 10e-5;
// bias for reflection
constexpr ftype REFLECTION_BIAS = 10e-5;
// threshold for distance to object (shadow)
constexpr ftype SHADOW_THRESHOLD = 10e-5;
// number of additional additional circles with 4 rays shot to compute shadow
constexpr ftype SHADOW_CIRCLES = 0.0;
// distance of rays on the circle to actual point
constexpr ftype SHADOW_DELTA = 10e-3;

/**
 * @brief Renderer object that stores information about the scene it should render
 * and the image that should be produced.
 */
class Renderer
{
public:
    // constructor and destructor
    Renderer();
    ~Renderer();

    // public member functions
    void addScene(std::string pathToSceneFile);
    void renderScene(std::string pathToOutputFile, itype width, itype height);

    // public member fields
    Scene *scene;
    Image *image;

private:
    // private member functions
    void renderPixels();
    void writeImageToFile(std::string pathToFile);
    Color sphereTrace(Vector const &ray_origin, Vector const &ray_direction, ftype distance);
    Color shade(Vector const &ray_to_shape, Vector const &ray_normalized, Shape *shape, ftype distance);
    ftype shadow(Vector const &ray_to_shade, Vector const &lightDir, ftype dist);
    bool ObjectInBetween(Vector const &ray_origin, Vector const &ray_direction, ftype max_dist);
};

} // namespace sphere

#endif // RENDERER_H