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
 * @file customVector.h
 * 
 * @brief implements a custom vector
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 25.04.2021
 * 
 */


#ifndef CUSTOM_VECTOR_H
#define CUSTOM_VECTOR_H

#include "SphereTypes.h"

namespace sphere {

/**
 * @brief type that stores a 3d vector of integers
 */
class Vector {

public:
    VectorVal x; // the vector's x-coordinate
    VectorVal y; // the vector's y-coordinate
    VectorVal z; // the vector's z-coordinate
    uint8_t _pad[PAD_VECTOR]; // auxiliary padding for cache performance

    Vector(){};

    Vector(VectorVal x, VectorVal y, VectorVal z);

    Vector operator+(const Vector &a);
    Vector operator-(const Vector &a);
    Vector operator*(const Vector &a);
    Vector operator*(const ftype &a);
    Vector absVal();
    Vector componentwiseMax(const Vector &a);
    Vector componentwiseMin(const Vector &a);
    VectorVal length();
    VectorVal distance(const Vector &a);
    Vector normalize();
    VectorVal maxComponent();
    VectorVal minComponent();

};

class ZeroVector : public Vector {
public:
    ZeroVector() {
        x = 0;
        y = 0;
        z = 0;
    }
};

}





#endif //CUSTOM_VECTOR_H
