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
 * @file ImageClasses.h
 * 
 * @brief Decleration of the two classes Image and Pixel that are used to 
 * represent an image
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 29.04.2021
 */

#ifndef IMAGE_CLASSES_H
#define IMAGE_CLASSES_H

#include <vector>

#include "SphereTypes.h"
#include "CustomVector.h"

namespace sphere {

/**
 * @brief a pixel object consisting of a 2D vector representing the
 * pixel in camera coordinates and its color in rgb format.
 */
class Pixel 
{
public:
    // constructor
    Pixel(ftype x, ftype y);

    // public member functions
    void writeColor(ColorVal r, ColorVal g, ColorVal b);
    void writeColor(Color col);

    // public member fields
    Vector2 cameraCoord; //!< the coordinates of this pixel in the camera system
    Color color; //!< the color of this pixel
};

/**
 * @brief an image object consisting of a pixel vector and its 
 * width and height
 */
class Image
{
public:
    // constructor
    Image(ftype cameraFov, itype width, itype height);

    // public member fields
    itype width;  //!< the width of the image (in pixels)
    itype height; //!< the height of the image (in pixels)
    std::vector<Pixel> pixels; //!< vector containing all pixels
};

} // namespace sphere

#endif // IMAGE_CLASSES_H