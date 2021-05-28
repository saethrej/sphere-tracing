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
 * @file sphere_microbenchmark.cpp
 * 
 * @brief very primitive example program to demonstrate the microbenchmarking
 * functionality
 * 
 * @authors Christopher Raffl, André Gaillard, Jens Eirik Saethre, David Spielmann
 * Contact: {rafflc, andrega, saethrej, spdavid}@ethz.ch
 * 
 * @date 26.05.2021
 */

#include <iostream>
#include <sstream>
#include <sys/types.h>

#include "sphere/Renderer.h"
#include "sphere/SphereMicroBenchmark.h"
#include "sphere/SphereProfiler.h"

int main(int argc, char *argv[])
{
#ifdef SPHERE_WITH_MICROBENCHMARKS

    // create a renderer object and add scene 0
    sphere::Renderer r;
    r.addScene("../scenes/scene0.json");

    // run the microbenchmarking suite
    r.microbenchmarkDistanceFunctions();

#else // SPHERE_WITH_MICROBENCHMARKS
    std::cout << "This program was compiled without SPHERE_WITH_MICROBENCHMARKS - aborting" << std::endl;
    return -1;
#endif // SPHERE_WITH_MICROBENCHMARKS
}

