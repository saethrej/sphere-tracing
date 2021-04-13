/**
 * Copyright (C) 2021, ETH Zurich
 * 
 * This product includes software developed at the Advanced Computing Laboratory
 * (ACL) at ETH Zurich and was written for the Advanced Systems Lab lecture. For
 * more information, visit https://acl.inf.ethz.ch/teaching/fastcode/2021/. Unless
 * you have an agreement with ETH Zurich for a separate license, the following
 * terms and conditions apply:
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program. If not, see http://www.gnu.org/licenses.
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

int main(int argc, char *argv[])
{
    // set-up parser for command line arguments
    cxxopts::Options options(
        "Sphere Mini-App for Rendering",
        "This is a miniapp rendering selected scenes with SPHERE"
    );
    options.add_options()
        ("s,scenes", "Scene(s) to render",
        cxxopts::value<std::vector<uint8_t>>())
        ("a,allscenes", "render all scenes. --scenes will be ignored",
        cxxopts::value<bool>()->default_value("false"))
        ("r,rep", "number of repetitions",
        cxxopts::value<uint8_t>()->default_value("5"))
        ("h,help", "Print usage.");

    const char **argv2 = const_cast<const char**>(argv);
    auto result = options.parse(argc, argv2);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return -1;
    }

    // parse the command line arguments
    bool allScenes = result["allscenes"].as<bool>();
    std::vector<uint8_t> scenes = result["scenes"].as<std::vector<uint8_t>>();
    uint8_t rep = result["rep"].as<uint8_t>();

    // do sth useful here
}