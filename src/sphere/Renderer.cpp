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