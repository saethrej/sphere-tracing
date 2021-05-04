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
 * @file SphereBenchmark.cpp
 * 
 * @brief implementation of the benchmarks functionality defined in the header
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 04.05.2021
 */

#include "SphereBenchmark.h"

// there is only something to implement if we are in benchmark mode
#ifdef SPHERE_WITH_BENCHMARKS

// code for the case when we want to count flops
#ifdef BENCHMARKS_COUNT_OP

/**
 * @brief adds numFlops to the global flop counter
 * @param numFlops the number of flops to add 
 */
void sphere::FlopCounter::increment(uint64_t numFlops)
{
    flops += numFlops;
}

/**
 * @brief resets the flop counter to 0
 */
void sphere::FlopCounter::clear()
{
    flops = 0;
}

/**
 * @brief returns the current value of the flop counter
 * @return the current flop count 
 */
uint64_t sphere::FlopCounter::get()
{
    return flops;
}

#else // BENCHMARKS_COUNT_OP
// code for the case when we want to measure time/cycles

/**
 * @brief starts a timer and stores the time_point in the singleton's variable
 */
void sphere::Timer::start()
{
    startTime = std::chrono::high_resolution_clock::now();
}

/**
 * @brief ends the currently running timer and computes the time elapsed
 */
void sphere::Timer::end()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    timeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime).count();
}

/**
 * @brief clears the Timer singleton by setting time to 0.0 and erasing the 
 * startTime time_point
 */
void sphere::Timer::clear()
{
    timeInMs = 0.0;
    startTime = std::chrono::time_point<std::chrono::high_resolution_clock>();
}

/**
 * @brief returns the current timer value
 * @return the timer value in [ms]
 */
double sphere::Timer::get()
{
    return timeInMs;
}

#endif // BENCHMARKS_COUNT_OP

#endif // SPHERE_WITH_BENCHMARKS