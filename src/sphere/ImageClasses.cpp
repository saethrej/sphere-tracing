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
 * @file ImageClasses.cpp
 * 
 * @brief implementation of the image and pixel class
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 29.04.2021
 */

#include <cmath>

#include "ImageClasses.h"

/**
 * @brief constructs an image object by initializing all pixels and calculating
 * their x,y coordinates in camera coordinates.
 * 
 * @param cameraFov field of view of the camera, in degrees
 * @param width width of the image (in pixel)
 * @param height height of the image (in pixel)
 */
sphere::Image::Image(ftype cameraFov, itype width, itype height)
{
    ftype x, y;
    ftype width_f = static_cast<ftype>(this->width = width);
    ftype height_f = static_cast<ftype>(this->height = height);
    ftype ratio = width_f / height_f;
    ftype angle = tan((cameraFov * 0.5) * M_PI / 180.0);
    COUNT_OPS(34);
    // create pixels and add them to the vector
    this->pixels.reserve(this->height*this->width);    
    for (itype i = 0; i < this->height; ++i){
        for (itype j = 0; j < this->width; ++j){
            x = (2 * j / width_f - 1) * ratio * angle;
            y = (1 - i / height_f * 2) * angle;
            this->pixels.push_back(Pixel(x,y));
            COUNT_OPS(8);
        }
    }
}

/**
 * @brief constructs a pixel object consisting of its x,y coordinate in camera coordinates
 * 
 * @param x x-coordinate of pixel in camera-coordinates
 * @param y y-coordinate of pixel in camera-coordinates
 */
sphere::Pixel::Pixel(ftype x, ftype y)
{
    this->cameraCoord.x = x;
    this->cameraCoord.y = y;
}

/**
 * @brief writes rgb colors into the color field of a pixel.
 * 
 * @param r r-color value
 * @param g g-color value
 * @param b b-color value
 */
void sphere::Pixel::writeColor(ColorVal r, ColorVal g, ColorVal b){
    this->color.r = r;
    this->color.g = g;
    this->color.b = b;
}

void sphere::Pixel::writeColor(Color col){
    this->color.r = col.r;
    this->color.g = col.g;
    this->color.b = col.b;
}