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
 * @file sphere_miniapp.cpp
 * 
 * @brief miniapp for the sphere implementation, a highly-optimized single-core
 * sphere tracing library. It can be used to render sample scenes.
 * 
 * @authors Christopher Raffl, André Gaillard, Jens Eirik Saethre, David Spielmann
 * Contact: {rafflc, andrega, saethrej, spdavid}@ethz.ch
 * 
 * @date 13.04.2021
 */

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <chrono>

#include <cxxopts.hpp>
#include <json.hpp>

#include "sphere/Renderer.h"

int main(int argc, char *argv[])
{
    // set-up parser for command line arguments
    cxxopts::Options options(
        "Sphere Mini-App for Rendering",
        "This is a miniapp rendering selected scenes with SPHERE"
    );
    options.add_options()
        ("s,scenes", "Scene(s) to render",
        cxxopts::value<std::vector<uint8_t>>()->default_value("0"))
        ("d,dim", "Image Dimension of Ouput File",
        cxxopts::value<std::vector<uint16_t>>()->default_value("1200,800"))
        ("h,help", "Print usage.");

    const char **argv2 = const_cast<const char**>(argv);
    auto result = options.parse(argc, argv2);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return -1;
    }

    // parse the command line arguments
    std::vector<uint8_t> scenes = result["scenes"].as<std::vector<uint8_t>>();
    uint16_t width = result["dim"].as<std::vector<uint16_t>>()[0];
    uint16_t height = result["dim"].as<std::vector<uint16_t>>()[1];

    // generate all scenes in a loop
    for (const auto &sceneNr : scenes) {
        // generate path-to-scene and path-to-output first
        std::stringstream scenePath, outputPath;
        scenePath << "../scenes/scene" << std::to_string(sceneNr) << ".json";
        outputPath << "../scenes/output_scene" << std::to_string(sceneNr) << ".ppm";
        
        // initialize the Renderer, add a new scene and render it
        std::cout << "Rendering scene " << std::to_string(sceneNr) << " ..." << std::endl;
        sphere::Renderer r;
        r.addScene(scenePath.str());
        r.renderScene(outputPath.str(), width, height);
    }
}