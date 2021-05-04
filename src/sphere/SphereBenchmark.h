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
 * @file SphereBenchmark.h
 * 
 * @brief this header makes the benchmarking utility an optional dependency that
 * need not be included when SPHERE is shipped.
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 04.05.2021
 */

#ifndef SPHERE_BENCHMARK_H
#define SPHERE_BENCHMARK_H

// if SPHERE is built with BENCHMARKS define ea
#ifdef SPHERE_WITH_BENCHMARKS

#include <stdint.h>

// the library is built with the intention to count the flops
#ifdef BENCHMARKS_COUNT_OP
// first, define empty macros for the timer-related stuff
#define TIMER_START()
#define TIMER_END()
#define TIMER_CLEAR()
#define TIMER_GET()

// define a singleton class for the flop counter
namespace sphere {
/**
 * @brief singleton that acts as a flop counter. Cannot be instantiated and
 * offers 3 functions as an API 
 */
class FlopCounter
{
public:
    // static member functions
    static void increment(uint64_t numFlops);
    static void clear();
    static uint64_t get();
private:
    explicit FlopCounter(); // do not allow instantiation
    inline static uint64_t flops = 0; //!< the number of flops encountered so far
};
} // namespace sphere

// define macros to simplify the handling of the flop counter
#define COUNT_OPS(num) sphere::FlopCounter::increment(#num)
#define COUNT_CLEAR() sphere::FlopCounter::clear()
#define COUNT_GET() sphere::FlopCounter::get()

#else // BENCHMARKS_COUNT_OP
// the library should not count ops but rather measure the time

#include <chrono>

// likewise, define empty macros for op-count-related stuff
#define COUNT_OPS(num)
#define COUNT_CLEAR()
#define COUNT_GET()

// define a singleton class for the timer
namespace sphere {
/**
 * @brief singleton that acts as a timer for benchmarking purposes. It can
 * either be used to count the time as seconds or as cycles 
 */
class Timer
{
public:
    // static member functions
    static void start();
    static void end();
    static void clear();
    static double get();
private:
    explicit Timer();
    inline static double timeInMs = 0.0; //!< time elapsed in [ms]
    static std::chrono::time_point<std::chrono::high_resolution_clock> startTime; //!< start time point
};
} // namespace sphere

// define macros to simply the handling of the flop counter
#define TIMER_START() sphere::Timer::start()
#define TIMER_END() sphere::Timer::end()
#define TIMER_GET() sphere::Timer::get()
#define TIMER_CLEAR() sphere::Timer::clear()

#endif // BENCHMARKS_COUNT_OP

#else // SPHERE_WITH_BENCHMARKS
// define empty macros which leads to the pre-processor simply removing the
// lines in the source code that contain the following macros
#define COUNT_OPS(num)
#define COUNT_CLEAR()
#define COUNT_GET()
#define TIMER_START()
#define TIMER_END()
#define TIMER_GET()
#define TIMER_CLEAR()

#endif // SPHERE_WITH_BENCHMARKS

#endif // SPHERE_BENCHMARK_H