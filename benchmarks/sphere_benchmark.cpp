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
 * @file sphere_benchmark.cpp
 * 
 * @brief benchmark suite for the sphere implementation
 * 
 * @authors Christopher Raffl, André Gaillard, Jens Eirik Saethre, David Spielmann
 * Contact: {rafflc, andrega, saethrej, spdavid}@ethz.ch
 * 
 * @date 05.05.2021
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/types.h>

#include <cxxopts.hpp>

#include "sphere/Renderer.h"
#include "sphere/SphereBenchmark.h"
#include "sphere/SphereProfiler.h"

int main(int argc, char *argv[])
{
#ifdef SPHERE_WITH_BENCHMARKS

    // define the parameters that specify the benchmarking run
    // TODO: parametrize this somehow, e.g. via CMD argument
    constexpr sphere::itype startWidth = 100, startHeight = 60;
    constexpr sphere::itype incStepW = 100, incStepH = 60;
    constexpr sphere::itype numImages = 3;

    // parse the command line arguments
    cxxopts::Options options(
        "Sphere Benchmark Suite",
        "This is a benchmark suite to assess performance of SPHERE"
    );
    options.add_options()
        ("s,scene", "scene to render", cxxopts::value<uint8_t>())
        ("c,countops", "indicates that this is a preliminary run that is used to assess the number of flops in the given parameter range",
         cxxopts::value<bool>()->default_value("false"))
        ("r,repetitions", "Number of repetitions per scene", cxxopts::value<uint8_t>())
        ("h,help", "Print usage.");

    const char **argv2 = const_cast<const char**>(argv);
    auto result = options.parse(argc, argv2);

    // if the --help flag was used, print help section and return with error
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return -1; // unsuccessful
    }

    // get the scene name and file location
    uint8_t sceneNr = result["scene"].as<uint8_t>();
    std::stringstream nameBlder;
    nameBlder << "../scenes/scene" << std::to_string(sceneNr) << ".json";
    std::string sceneName = nameBlder.str();

    // get remaining arguments
    bool countOpsMode = result["countops"].as<bool>();
    uint8_t reps = result["repetitions"].as<uint8_t>();

    // if the purpose of this run is to compute the flops that are carried out
    // in the process of rendering this scene, only compute each image once
    if (countOpsMode) {
#ifndef BENCHMARKS_COUNT_OP
        std::cout << "This program was compiled without BENCHMARKS_COUNT_OP, cannot count flops - aborting" << std::endl;
        return -1;
#endif // BENCHMARKS_COUNT_OP
        std::vector<uint64_t> flops;
        flops.resize(numImages);
        for (size_t i = 0; i < numImages; ++i) {
            // compute the image dimensions
            const sphere::itype width = startWidth + i * incStepW;
            const sphere::itype height = startHeight + i * incStepH;

            // clear the op count from the previous iteration
            COUNT_CLEAR();

            // use SPHERE to render the scene and count the number of flops in the
            // region of interest
            sphere::Renderer r;
            TIMER_START();
            r.renderScene("../scenes/temp_scene.ppm", width, height);

            // store the number of flops
#ifdef BENCHMARKS_COUNT_OP
            flops[i] = COUNT_GET();
#endif // BENCHMARKS_COUNT_OP       
        }

        // store flops back into a csv file
        std::stringstream csvName;
        csvName << "../benchmarks/flop-count_scene" << std::to_string(sceneNr) << ".csv";
        std::ofstream out(csvName.str().c_str(), std::ios::out);

        for (size_t i = 0; i < numImages; ++i) {
            out << startWidth + i * incStepW << "," << startHeight + i * incStepH
                << "," << flops[i] << std::endl;
        }
        out.close();
    }
    // otherwise, this is a timed run where we want to measure the execution time
    // or the number of cycles that the library takes.
    else {
#ifdef BENCHMARKS_COUNT_OP
        std::cout << "This program was compiled with BENCHMARKS_COUNT_OP, cannot measure time - aborting" << std::endl;
        return -1;
#endif // BENCHMARKS_COUNT_OP
        // store all runtimes in a vector of vectors
        std::vector<std::vector<double>> runtimes;
        runtimes.resize(numImages);
        for (auto &vec : runtimes) vec.resize(reps);

        for (size_t i = 0; i < numImages; ++i) {
            // compute the image dimensions
            const sphere::itype width = startWidth + i * incStepW;
            const sphere::itype height = startHeight + i * incStepH;

            // loop over the number of repetitions
            for (size_t run = 0; run < reps; ++run) {
                // clear the timer
                TIMER_CLEAR();
                
                // use SPHERE to render the scene and measure time in region of interest
                sphere::Renderer r;
                r.addScene(sceneName);
                TIMER_START();
                r.renderScene("./scenes/temp_scene.ppm", width, height);
                TIMER_END();

                // put the result into the vector
#ifndef BENCHMARKS_COUNT_OP
                runtimes[i][run] = TIMER_GET();
#endif // BENCHMARKS_COUNT_OP
            }
        }

        // store the runtimes back into the csv files
        std::stringstream csvName;
        csvName << "../benchmarks/runtimes_scene" << std::to_string(sceneNr) << ".csv";
        std::ofstream out(csvName.str().c_str(), std::ios::out);

        for (size_t i = 0; i < numImages; ++i) {
            out << startWidth + i * incStepW << "," << startHeight + i * incStepH;
            for (size_t run = 0; run < reps-1; ++run) {
                out << "," << runtimes[i][run];
            }
            out << "," << runtimes[i][reps-1] << std::endl;
        }
        out.close();
    }

#else // SPHERE_WITH_BENCHMARKS
    std::cout << "This program was compiled without SPHERE_WITH_BENCHMARKS - aborting" << std::endl;
    return -1;
#endif // SPHERE_WITH_BENCHMARKS
}