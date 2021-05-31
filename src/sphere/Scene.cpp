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
 * @file Scene.cpp
 * 
 * @brief implementation of scene object that is to be rendered
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 28.04.2021
 */

#include <string>
#include <fstream>
#include <iostream>

#include "SphereTypes.h"
#include "Scene.h"

/**
 * @brief constructs a scene object by opening json file and creating all the
 * shapes that are described.
 * 
 * The Scene object handles and determines the lifetime of the different shape
 * objects, i.e. it is the sole owner of them. As such, the shapes are created
 * in this constructor and consequently also deleted in this object's destructor.
 * 
 * @param pathToFile path to the scene description json file
 * @throws SphereException if the scene cannot be constructed
 */
sphere::Scene::Scene(std::string pathToFile)
{
    // create a wrapper object for each type of shape
    this->wBox = new BoxWrapper();
    this->wCone = new ConeWrapper();
    this->wOcta = new OctaWrapper();
    this->wPlane = new PlaneWrapper();
    this->wSphere = new SphereWrapper();
    this->wTorus = new TorusWrapper();

    // try to open the file whose path is provided and dump to json variable
    std::ifstream jsonFile(pathToFile.c_str(), std::ios::in);
    if (jsonFile.fail()) {
        throw SphereException(SphereException::ErrorCode::SceneFileNotFound);
    }
    jsonFile >> this->sceneDescription;
    jsonFile.close();

    // try to read the contents from the json object
    try {
        // read camera information from json file
        json cam = this->sceneDescription["camera"];
        this->cameraFov = cam.value("fov", 0.0);
        json camPos = cam["position"];
        json camRot = cam["rotation"];
        this->cameraPos = {camPos.value("x", 0.0), camPos.value("y", 0.0), camPos.value("z", 0.0)};
        this->cameraRot = {camRot.value("x", 0.0), camRot.value("y", 0.0), camRot.value("z", 0.0)};

        // read pointlight information
        json lghPos = this->sceneDescription["pointlight"]["position"];
        json lghEmi = this->sceneDescription["pointlight"]["emission"];
        this->lightPos = {lghPos.value("x", 0.0), lghPos.value("y", 0.0), lghPos.value("z", 0.0)};
        this->lightEmi = {lghEmi.value("x", 0.0), lghEmi.value("y", 0.0), lghEmi.value("z", 0.0)};

        // create the different shapes that are contained within the "objects" part
        // of the json scene description. Note that shapes that are not defined in
        // SphereShapes.h will be ignored
        this->numShapes = 0;
        json objects = this->sceneDescription["objects"];
        this->shapes.reserve(objects.size());

        for (json const &shp : objects) {
            ShapeType type = Shape::getShapeType(shp.value("kind", "none"));
            this->numShapes++;
            switch (type) {
                case ShapeType::BOX: { 
                    Box *box = new Box(shp);
                    this->shapes.push_back(box);
                    this->wBox->addBox(box);
                    break;
                }
                case ShapeType::CONE: {
                    Cone *cone = new Cone(shp);
                    this->shapes.push_back(cone);
                    this->wCone->addCone(cone);
                    break;
                }
                case ShapeType::OCTAHEDRON: {
                    Octahedron *oct = new Octahedron(shp);
                    this->shapes.push_back(oct);
                    this->wOcta->addOcta(oct);
                    break;
                }
                case ShapeType::PLANE: {
                    Plane *plane = new Plane(shp);
                    this->shapes.push_back(plane);
                    this->wPlane->addPlane(plane);
                    break;
                }
                case ShapeType::SPHERE: {
                    Sphere *sphe = new Sphere(shp);
                    this->shapes.push_back(sphe);
                    this->wSphere->addSphere(sphe);
                    break;
                }
                case ShapeType::TORUS: {
                    Torus *torus = new Torus(shp);
                    this->shapes.push_back(torus);
                    this->wTorus->addTorus(torus);
                    break;
                }
                default:
                    // counter the ++ before because this shape cannot be handled
                    this->numShapes--;
                    continue;
            }
        }

    } catch (json::exception &e) {
        // print exception information and rethrow as SphereException
        std::cerr << e.what() << std::endl;
        throw SphereException(SphereException::ErrorCode::JsonSyntaxError);
    }
}

/**
 * @brief deletes a Scene object and all of the shapes it contains
 */
sphere::Scene::~Scene()
{
    // iterate over all shapes and delete them
    for (size_t i = 0; i < this->numShapes; ++i) {
        delete this->shapes[i];
    }

    // delete all of the wrappers
    delete this->wBox;
    delete this->wCone;
    delete this->wOcta;
    delete this->wPlane;
    delete this->wSphere;
    delete this->wTorus;
}

/**
 * @brief add new shape to the scene
 * @param shape pointer to the new shape
 * 
 * @note deletes the shape if its type is not supported
 */
void sphere::Scene::addShape(sphere::Shape *shape)
{
    shapes.push_back(shape);
    numShapes++;

    switch (shape->type) {
        case ShapeType::BOX: { 
            this->wBox->addBox(dynamic_cast<Box*>(shape));
            break;
        }
        case ShapeType::CONE: {
            this->wCone->addCone(dynamic_cast<Cone*>(shape));
            break;
        }
        case ShapeType::OCTAHEDRON: {
            this->wOcta->addOcta(dynamic_cast<Octahedron*>(shape));
            break;
        }
        case ShapeType::PLANE: {
            this->wPlane->addPlane(dynamic_cast<Plane*>(shape));
            break;
        }
        case ShapeType::SPHERE: {
            this->wSphere->addSphere(dynamic_cast<Sphere*>(shape));
            break;
        }
        case ShapeType::TORUS: {
            this->wTorus->addTorus(dynamic_cast<Torus*>(shape));
            break;
        }
        default:
            // counter the ++ and remove shape
            this->numShapes--;
            this->shapes.pop_back();
            delete shape;
    }
}