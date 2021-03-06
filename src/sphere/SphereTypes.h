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
 * @file SphereTypes.h
 * 
 * @brief various typedefs and constants that are used throughout the library
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 25.04.2021
 */

#ifndef SPHERE_TYPES_H
#define SPHERE_TYPES_H

#include <stdint.h>
#include <stdexcept>

namespace sphere {

/******************************** Constants **********************************/
constexpr uint8_t PAD_VECTOR = sizeof(double);
constexpr uint8_t PAD_COLOR = sizeof(float);
constexpr uint8_t PAD_2DVECT = 0;

/**************************** Type Definitions *******************************/
using VectorVal = double; // type used for coordinates in vectors
using ColorVal = float;   // type used for color components
using ftype = double;     // the floating point type used in SPHERE
using itype = int32_t;    // the integer type used in SPHERE

/**************************** Exception Types ********************************/

/**
 * @brief exception object that is thrown whenever an error occurs within the
 * SPHERE library.
 */
class SphereException : public std::logic_error
{
public:

    /**
     * @brief error codes for the different causes of exceptions 
     */
    enum ErrorCode {
        SceneFileNotFound,
        InvalidParams,
        JsonSyntaxError
    };

    /**
     * @brief creates a SphereException object with an associated error code
     * @param code the error code
     */
    SphereException(ErrorCode code) : SphereException(decodeError(code)) {}

private:
    /**
     * @brief decodes an error code into a human-readable error message
     * @param code the error code to decode into string
     * @returns a char array with the error message
     */
    const char* decodeError(ErrorCode code) {
        switch (code) {
            case SceneFileNotFound:
                return "The scene description file (.json) could not be found.";
            case InvalidParams:
                return "The scene description file (.json) contains invalid parameters";
            case JsonSyntaxError:
                return "The scene description file (.json) contains syntax errors and could not be parsed.";
            default:
                return "Undefined exception occurred.";
        }
    }

    /**
     * @brief private and thus uncallable constructor for SphereException object
     * that, once the emssage is decoded, creates a std::logic_error object
     * with the desired error message
     * 
     * @param msg the error message to be displayed 
     * @todo this seems like bad practice, rethink exception mechanism
     */
    SphereException(const char* msg) : std::logic_error(msg) {}
};

} // namespace sphere

#endif // SPHERE_TYPES_H