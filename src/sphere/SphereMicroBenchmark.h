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
 * @file SphereMicroBenchmark.h
 * 
 * @brief this header makes the microbenchmarking utility an optional dependency that
 * need not be included when SPHERE is shipped.
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 26.05.2021
 */

#ifndef SPHERE_MICRO_BENCHMARK_H
#define SPHERE_MICRO_BENCHMARK_H

// if SPHERE is built with MICROBENCHMARKS then define a singleton object that
// can access the CPU's time stamp counter
#ifdef SPHERE_WITH_MICROBENCHMARKS

// microbenchmarks can only be used under linux with gcc  
#if !defined(__GNUC__) || !defined(__linux__) 
    #error "Microbenchmarks are only supported under Linux & GCC"
#endif // __GNUC__ || __linux__

// Microbenchmarks cannot be run with OpenMP
#ifdef SPHERE_WITH_OPENMP
    #error "Microbenchmark cannot be compiled with OpenMP"
#endif // SPHERE_WITH_OPENMP

// define stuff to use in the inlined assembly code
#define VOLATILE __volatile__
#define ASM __asm__
#define MicroBenchInt64 unsigned long long
#define MicroBenchInt32 unsigned int

// define counter operations
#define COUNTER_LO(arg) ((arg).int32.lo)
#define COUNTER_HI(arg) ((arg).int32.hi)
#define COUNTER_VAL(arg) ((arg).int64)
#define COUNTER(arg) (static_cast<MicroBenchInt64>(COUNTER_VAL(arg)))
#define COUNTER_DIFF(arg1,arg2) (COUNTER(arg1) - COUNTER(arg2))

// define the tsc_counter as a union with 1 64 bit int and 2 32 bit ints
typedef union {
    MicroBenchInt64 int64;
    struct {
        MicroBenchInt32 lo;
        MicroBenchInt32 hi;
    } int32;
} TscCounter;

// define the operations to get the CPU's time stamp counter
#define RDTSC(arg) ASM VOLATILE ("rdtsc" : "=a" ((arg).int32.lo), "=d"((arg).int32.hi))
#define CPUID() ASM VOLATILE ("cpuid" : : "a" (0) : "bx", "cx", "dx" )

namespace sphere {
/**
 * @brief singleton that acts as a microbenchmarker Cannot be instantiated and
 * offers 4 functions as an API to microbenchmark parts of the code
 */
class MicroBenchmarker
{
public:
    // static class functions
    static void init();
    static void clear();
    template<typename F> static void measure(F func);
    static double get();

private:
    // disallow instantiation with this private explicit c'tor
    explicit MicroBenchmarker();

    // private static class functions
    static MicroBenchInt64 startMeasurement();
    static MicroBenchInt64 stopMeasurement();
    
    // private static class fields
    inline static double cycles = 0.0; //!< cycles elapsed
};
} // namespace sphere

/**
 * @brief measures the cycles that the passed function takes to execute and
 * writes this result into the cycle class field
 * @tparam F is a callable type
 * @param func the function to measure, i.e. microbenchmark
 */
template <typename F>
void sphere::MicroBenchmarker::measure(F func)
{
    // define local variables for the measurement first
    MicroBenchInt64 c0, c1 = 0;
    int i = 0; int numRuns = 1;
    MicroBenchInt64 requiredCycles = 2e10;

    // establish the number of runs required to get an accurate measurement of 
    // the number of cycles (i.e. where overhead of timing is negligible)
    while (numRuns < 300000000) {
        c0 = startMeasurement();
        for (i = 0; i < numRuns; ++i) {
            func();
        }
        c1 = stopMeasurement();

        // if the number of cycles exceeded the required amount, we can start
        // with the microbenchmarking
        if (c1 - c0 > requiredCycles) break;

        // otherwise we increase the number of runs by a factor of 2
        numRuns *= 2;
    }

    // here the effective microbenchmarking happens
    c0 = startMeasurement();
    for (i = 0; i < numRuns; ++i) {
        func();
    }
    c1 = stopMeasurement();

    // save the average number of cycles
    cycles = static_cast<double>(c1 - c0) / numRuns;
}

// define macros to simplify handling of the MicroBenchmarker
#define TSC_INIT() sphere::MicroBenchmarker::init()
#define TSC_CLEAR() sphere::MicroBenchmarker::clear()
#define TSC_MEASURE(arg) sphere::MicroBenchmarker::measure(arg)
#define TSC_GET() sphere::MicroBenchmarker::get()

#else // SPHERE_WITH_MICROBENCHMARKS

// define empty macros for TSC commands
#define TSC_INIT()
#define TSC_MEASURE(arg)
#define TSC_GET()
#define TSC_CLEAR()

#endif // SPHERE_WITH_MICROBENCHMARKS

#endif // SPHERE_MICRO_BENCHMARK_H