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
 * @file Scene.h
 * 
 * @brief declaration of a class that defines a scene to be rendered
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 25.04.2021
 */

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>

#include <json.hpp>

#include "SphereTypes.h"
#include "SphereShapes.h"

using json = nlohmann::json;

namespace sphere {

/**
 * @brief Scene description object that stores information about the camera,
 * the pointlight(s) and all shapes in the scene
 */
class Scene
{
public:
    // constructor and destructor
    Scene(std::string pathToFile);
    ~Scene();

    // public member functions
    void addShape(Shape *newShape);

    // public member fields
    ftype cameraFov;  //!< field of view of the camera
    Vector cameraPos; //!< position of the camera
    Vector cameraRot; //!< rotation of the camera
    Vector lightPos;  //!< position of the pointlight
    Vector lightEmi;  //!< emission of the pointlight

    // container and counter for all shapes
    std::vector<Shape*> shapes; //!< vector of shape objects
    itype numShapes;  //!< number of shapes in this scene

    // container for individual shape types
    std::vector<Plane*> planes; //!< vector of all plane shapes
    std::vector<Box*> boxes; //!< vector of all box shapes
    std::vector<Sphere*> spheres; //!< vector of all sphere shapes
    std::vector<Torus*> tori; //!< vector of all tori shapes
    std::vector<Octahedron*> octas; //!< vector of all octahedron shapes
    std::vector<Cone*> cones; //!< vector of all cone shapes

    // counters for all shape types
    itype numPlanes; //!< number of planes in this scene
    itype numBoxes; //!< number of boxes in this scene
    itype numSpheres; //!< number of spheres in this scene
    itype numTori; //!< number of tori in this scene
    itype numOctas; //!< number of octahedrons in this scene
    itype numCones; //!< number of cones in this scene


private:
    // private member fields
    json sceneDescription; //!< json description of the file
};

} // namespace sphere

#endif // SCENE_H
