# SPHERE TRACING
This is the project repository for the Sphere Tracing project (team 23) in the Advanced Systems Lab course in the spring semester 2021 at ETH Zurich. 

## Finding your way
Please note that the different versions are spread across different branches. The branches mentioned in the report are called:
```
stable/baseline
stable/algorithmic
stable/compiler-help # this is referred to as mathematical in the report
stable/vectorization # Compile with openMP for the multithreaded version
```

If you want to benchmark larger scenes (more than 16 objects per shape type) in the vectorized version, you need to change the `MAX_OBJECTS` variable in `Sphere_Shapes.h`

## Prerequisites
- CMake version 3.15 or newer.
- A Linux distribution.

## Building SPHERE
The library can be built by running the following commands.
```bash
# get SPHERE 
git clone https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team23.git sphere && cd sphere

# set-up the compiler, e.g. with:
export CC='which cc'
export CXX='which CC'

# build the library with the desired options
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 8
```

Other build options are summarized in the following table:
| Build Option             | Description                                                                                                                                        | Default Value | Possible Values                        |
|--------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------|---------------|----------------------------------------|
| `CMAKE_BUILD_TYPE`       | Type of the build, determines optimization levels, etc.                                                                                            | `Release`     | `DEBUG`, `MinSizeRel`, `RelWithDebInf` |
| `SPHERE_WITH_PROFILING`  | Enables profiling the library with semiprof to generate a table where most of the time is spent.<br>Sets the `SPHERE_WITH_PROFILING` macro during compilation. | `OFF`         | `ON`                                   |
| `SPHERE_WITH_BENCHMARKS` | Builds the library with the `SPHERE_WITH_BENCHMARKS` macro and builds all programs in the benchmarks directory.                                                 | `OFF`         | `ON`                                   |
| `SPHERE_WITH_MICROBENCHMARKS` | Builds the library with `SPHERE_WITH_MICROBENCMARKS` macro and builds an application for microbenchmarking. Note that this option disables both `SPHERE_WITH_OPENMP` and `SPHERE_WITH_BENCHMARKS` as these are not compatible. | `OFF` | `ON` |
| `BENCHMARKS_COUNT_OP`    | Builds the library with the `BENCHMARKS_COUNT_OP` macros. If activated, the  `FlopCounter` class will be built, otherwise the `Timer` class will be built. Note that this option will be ignored if `SPHERE_WITH_BENCHMARKS` is not set. | `OFF` | `ON`
| `SPHERE_WITH_TESTS`      | Builds unit tests for the library relying on Google's C++ test infrastructure.                                                                     | `OFF`          | `ON`                                  |
| `SPHERE_WITH_OPENMP`     | Builds the library with OpenMP activated if supported by your compiler. Run `export OMP_NUM_THREADS=XXX` to set the desired number of threads before running one of the SPHERE executables. | `ON` | `OFF` |


## Running SPHERE
We recommend running SPHERE from the build folder of this project  for the paths to possible inputs/outputs to resolve correctly. To launch SPHERE, run:
```bash
./examples/sphere_miniapp --scenes=0,1,2 --dim=1000,600
```
This renders scenes 0,1,2 with 1000x600 pixel resolution.

## Benchmarking SPHERE
There's a benchmarking utility script written in Python, namely `benchmarks/run_benchmark.py` that you can use to simplify benchmarking. Checkout the branch you want to benchmark, choose a number of repetitions (e.g. 5) and a scene number (e.g. 0). Then, simply run
```bash
./benchmarks/run_benchmark.py --scene=0 --repetitions=5
```
This can take a while, but the program reports its progress in stdout. After running, you'll find two csv files in the `benchmarks` folder. Each line represents one input size (the first and second entries are width and height, respectively), while the following entry/entries are measurements. Note that times are reported in [ms].

## Microbenchmarking SPHERE 
To microbenchmark parts of SPHERE (currently only the distance functions) compile SPHERE with the option for microbenchmarks, i.e.
```bash
cmake -DCMAKE_BUILD_TYPE=RELEASE -DSPHERE_WITH_MICROBENCHMARKS=ON ..
cd build && make -j 8
``` 
and then run the sample microbenchmark program via
```bash
./benchmarks/sphere_microbenchmark
``` 
This creates a file `benchmarks/microbenchmarks-df.txt` with the number of cycles that were measured.

## Authors
- [Andr?? Gaillard](mailto:andrega@ethz.ch)
- [Christopher Raffl](mailto:rafflc@ethz.ch)
- [Jens Eirik Saethre](mailto:saethrej@ethz.ch)
- [David Spielmann](mailto:spdavid@ethz.ch)
