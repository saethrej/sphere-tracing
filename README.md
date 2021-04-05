# SPHERE TRACING
This is the project repository for the Sphere Tracing project (team 23) in the Advanced Systems Lab course in the spring semester 2021 at ETH Zurich. 

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
cd .. # leave build directory
```

Other build options are summarized in the following table:
| Build Option             | Description                                                                                                                                        | Default Value | Possible Values                        |
|--------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------|---------------|----------------------------------------|
| `CMAKE_BUILD_TYPE`       | Type of the build, determines optimization levels, etc.                                                                                            | `Release`     | `DEBUG`, `MinSizeRel`, `RelWithDebInf` |
| `SPHERE_WITH_APPS`       | Specifies whether example apps should be built or not.                                                                                             | `ON`          | `OFF`                                  |
| `SPHERE_WITH_PROFILING`  | Enables profiling the library with semiprof to generate a table where most of the time is spent.<br>Sets the `PROFILING` macro during compilation. | `OFF`         | `ON`                                   |
| `SPHERE_WITH_BENCHMARKS` | Builds the library with the `BENCHMARK` macro and builds all programs in the benchmarks directory.                                                 | `OFF`         | `ON`                                   |
| `SPHERE_WITH_TESTS`      | Builds unit tests for the library relying on Google's C++ test infrastructure.                                                                     | `ON`          | `OFF`                                  |
| `BUILD_SHARED_LIBS`      | Creates a shared library object that other programs can link to dynamically                                                                        | `OFF`         | `ON`                                   |


## Running SPHERE
We recommend running SPHERE from the root folder of this project (thus the `cd ..` command earlier in this tutorial) for the paths to possible inputs/outputs to resolve correctly. To launch SPHERE, run:
```bash
./build/examples/sphere_miniapp --arg1=val1 --arg2=val2 --arg3
```

## Creating Documentation for SPHERE
We use the doxygen system to automatically create a documentation for the code. If you wish to generate the documentation, make sure that you have Doxygen installed, otherwise check [this page](https://www.doxygen.nl/manual/install.html). If you want to have inheritance diagrams etc. in the documentation, also make sure you have graphviz installed.

With these pre-requisites, you can generate the HTML documentation by running
```bash
doxygen doc/doxygen.config
``` 
then entering the `doc/html/` directory and opening the `index.html` file with your favourite browser.


## Authors
- [André Gaillard](mailto:andrega@ethz.ch)
- [Christopher Raffl](mailto:rafflc@ethz.ch)
- [Jens Eirik Saethre](mailto:saethrej@ethz.ch)
- [David Spielmann](mailto:spdavid@ethz.ch)
