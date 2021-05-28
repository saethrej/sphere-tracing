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
 * @file SphereMicroBenchmark.cpp
 * 
 * @brief implementation of the microbenchmarks functionality defined in the header
 * 
 * @authors André Gaillard, Christopher Raffl, Jens Eirik Saethre, David Spielmann
 * Contact: {andrega, rafflc, saethrej, spdavid}@ethz.ch
 * 
 * @date 26.05.2021
 */

#include "SphereMicroBenchmark.h"

// there is only something to implement if we are in microbenchmark mode
#ifdef SPHERE_WITH_MICROBENCHMARKS

/**
 * @brief initializes the MicroBenchmarker class
 * @note this currently does nothing and need not be called
 */
void sphere::MicroBenchmarker::init()
{
    // nothing to do here currently
}

/**
 * @brief resets the cycle counter to 0
 */
void sphere::MicroBenchmarker::clear()
{
    cycles = 0.0;
}

/**
 * @brief returns the number of cycles that elapsed during the last measurement
 * @return the number of elapsed cycles
 */
double sphere::MicroBenchmarker::get()
{
    return cycles;
}

/**
 * @brief starts a cycle measurement
 * @return the cycle number at the start
 */
MicroBenchInt64 sphere::MicroBenchmarker::startMeasurement()
{
    TscCounter start;
    CPUID(); // serializes the pipeline and acts as execution barrier
    RDTSC(start);
    return COUNTER_VAL(start);
}

/**
 * @brief stops a cycle measurement
 * @return the cycle number at the end 
 */
MicroBenchInt64 sphere::MicroBenchmarker::stopMeasurement()
{
    TscCounter end;
    RDTSC(end);
    CPUID(); // serializes the pipeline and acts as execution barrier
    return COUNTER_VAL(end);
}

#endif // SPHERE_WITH_MICROBENCHMARKS