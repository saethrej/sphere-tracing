#!/usr/bin/env python3
"""
Project: SPHERE - a sphere tracing library
Authors: André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
Date: 05.05.2021
Description: This is a utility script that can be used to more easily benchmark
the SPHERE library. It essentially operates in the following steps:

    1.) accept this benchmark run's params such as repetitions, scene nr
    2.) build the SPHERE library in benchmark mode and count-flops submode
    3.) run SPHERE once on every input size for the selected scene. We do this via
        the benchmarks/sphere_benchmark program, which generates a csv containing
        the number of flops of each input size
    4.) rebuild the SPHERE library in benchmark mode, but disable count-flops submode
        this results in a timer being available
    5.) run the SPHERE library repetition times for every input size, collect timings
        that are again dumped into a csv file

Usage: ./benchmarks/run_benchmark.py --scene=0 --repetitions=5
       Note that it has to be run from the root folder for the paths to resolve correctly
"""
import os
import logging as log 
import subprocess
import pandas as pd 
import numpy as np 
import argparse

# configure the logging functionality
log.basicConfig(level=log.INFO, format='[%(levelname)s] %(asctime)s:\t %(message)s', datefmt='%H:%M:%S')

def run_benchmark_app(scene: int, reps: int, count_ops: bool) -> None:
    os.chdir("./build/")
    if count_ops:
        subprocess.run("./benchmarks/sphere_benchmark --scene={} --countops --repetitions=1".format(scene),  shell=True)
    else:
        subprocess.run("./benchmarks/sphere_benchmark --scene={} --repetitions={}".format(scene, reps),  shell=True)
    log.info("Ran SPHERE benchmark with --countsops option. Computed Flops.")
    os.chdir("../")

def build_sphere_library(benchmark: bool, count_ops: bool) -> None:
    os.chdir("./build/")
    subprocess.run("rm -rf *", shell=True)
    if not benchmark:
        subprocess.run("cmake -DCMAKE_BUILD_TYPE=RELEASE -DSPHERE_WITH_BENCHMARKS=OFF ..", shell=True)
    else:
        if count_ops:
            subprocess.run("cmake -DCMAKE_BUILD_TYPE=RELEASE -DSPHERE_WITH_BENCHMARKS=ON -DBENCHMARKS_COUNT_OP=ON ..", shell=True)
        else:
            subprocess.run("cmake -DCMAKE_BUILD_TYPE=RELEASE -DSPHERE_WITH_BENCHMARKS=ON ..", shell=True)
    subprocess.run("make -j 8", shell=True)
    log.info("Built sphere library with -DSPHERE_WITH_BENCHMARKS={} -DBENCHMARKS_COUNT_OP={}".format(
        "ON" if benchmark else "OFF", "ON" if count_ops else "OFF"
    ))    
    os.chdir("../")

def get_processor_frequency() -> float:
    """ returns the processor max frequency in GHz """
    return 1000000 * int(subprocess.check_output("lscpu | grep 'MHz' | awk 'FNR == 2 {print $4}'", shell=True), base=2)


if __name__ == "__main__":
    # 1.) parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--scene', type=int, help='the scene to benchmark on')
    parser.add_argument('-r', '--repetitions', type=int, help='number of repetitions per input size')
    parser.add_argument('--no_opscount', action='store_true', help='disables counting of ops', default=False)
    args = vars(parser.parse_args())
    scene = args['scene']
    reps = args["repetitions"]
    opscount = not args["no_opscount"]

    if(opscount):
        # 2.) build the library with BENCHMARKS_COUNT_OP on
        log.info("**** Building SPHERE with BENCHMARKS_COUNT_OP")
        build_sphere_library(True, True)

        # 3.) run SPHERE benchmarks with --countops
        log.info("**** Running SPHERE benchmark with --countops")
        run_benchmark_app(scene, reps, True)

    # 4.) rebuild the library without BENCHMARKS_COUNT_OP
    log.info("**** Rebuilding SPHERE without BENCHMARKS_COUNT_OP")
    build_sphere_library(True, False)

    # 5.) run the SPHERE timing benchmark
    log.info("**** Starting to run SPHERE timing benchmark")
    run_benchmark_app(scene, reps, False)
    log.info("**** Finished running sphere timing benchmark")
