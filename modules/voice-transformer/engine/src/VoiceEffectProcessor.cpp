/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "AACE/Engine/VoiceTransformer/VoiceEffectProcessor.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace aace {
namespace engine {
namespace voiceTransformer {

static constexpr double TWO_PI = 2.0 * M_PI;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

VoiceEffectProcessor::VoiceEffectProcessor(int sampleRate)
    : m_sampleRate(sampleRate), m_mode("NONE") {
    // Populate default params for every supported mode
    EffectParams robot;
    robot.carrierHz = 100.0f;
    m_params["ROBOT"] = robot;

    EffectParams chipmunk;
    chipmunk.pitchFactor = 1.5f;
    m_params["CHIPMUNK"] = chipmunk;

    EffectParams deep;
    deep.pitchFactor = 0.7f;
    m_params["DEEP"] = deep;

    EffectParams echo;
    echo.delayMs = 250;
    echo.decay = 0.4f;
    m_params["ECHO"] = echo;

    EffectParams alien;
    alien.carrierHz = 50.0f;
    alien.flangeDepth = 0.005f;
    m_params["ALIEN"] = alien;

    EffectParams whisper;
    whisper.amplitude = 0.3f;
    m_params["WHISPER"] = whisper;

    m_params["NONE"] = EffectParams{};

    // Pre-allocate OLA buffers
    m_grainBuf.resize(GRAIN_SIZE * 2, 0.0f);
    m_overlapBuf.resize(GRAIN_SIZE, 0.0f);
}

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

bool VoiceEffectProcessor::setMode(const std::string& mode) {
    if (m_params.find(mode) == m_params.end()) {
        return false;
    }
    if (m_mode != mode) {
        m_mode = mode;
        reset();
    }
    return true;
}

const std::string& VoiceEffectProcessor::getMode() const {
    return m_mode;
}

bool VoiceEffectProcessor::setParam(const std::string& mode, const std::string& key, float value) {
    auto it = m_params.find(mode);
    if (it == m_params.end()) {
        return false;
    }
    EffectParams& p = it->second;
    if (key == "carrierHz") {
        p.carrierHz = value;
    } else if (key == "pitchFactor") {
        p.pitchFactor = value;
    } else if (key == "delayMs") {
        p.delayMs = static_cast<int>(value);
        // Rebuild delay buffer at new size if echo is active
        if (m_mode == "ECHO") {
            reset();
        }
    } else if (key == "decay") {
        p.decay = value;
    } else if (key == "flangeDepth") {
        p.flangeDepth = value;
    } else if (key == "amplitude") {
        p.amplitude = value;
    } else {
        return false;
    }
    return true;
}

void VoiceEffectProcessor::reset() {
    m_ringPhase = 0.0;
    m_lfoPhase = 0.0;
    m_hpPrev = 0.0f;
    m_hpPrevOut = 0.0f;
    m_readPos = 0.0;
    std::fill(m_overlapBuf.begin(), m_overlapBuf.end(), 0.0f);
    std::fill(m_grainBuf.begin(), m_grainBuf.end(), 0.0f);

    // (Re-)allocate delay buffer for ECHO
    if (m_mode == "ECHO" || m_mode == "NONE") {
        int delaySamples = static_cast<int>(m_params["ECHO"].delayMs * m_sampleRate / 1000) + 1;
        m_delayBuf.assign(static_cast<std::size_t>(delaySamples), 0.0f);
        m_delayWrite = 0;
    }
}

// ---------------------------------------------------------------------------
// Process dispatch
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::process(int16_t* samples, std::size_t count) {
    if (!samples || count == 0) return;

    if (m_mode == "NONE") {
        applyNone(samples, count);
    } else if (m_mode == "ROBOT") {
        applyRobot(samples, count);
    } else if (m_mode == "CHIPMUNK") {
        applyChipmunk(samples, count);
    } else if (m_mode == "DEEP") {
        applyDeep(samples, count);
    } else if (m_mode == "ECHO") {
        applyEcho(samples, count);
    } else if (m_mode == "ALIEN") {
        applyAlien(samples, count);
    } else if (m_mode == "WHISPER") {
        applyWhisper(samples, count);
    }
}

// ---------------------------------------------------------------------------
// Helper
// ---------------------------------------------------------------------------

int16_t VoiceEffectProcessor::clampToInt16(float v) {
    if (v > 32767.0f) return 32767;
    if (v < -32768.0f) return -32768;
    return static_cast<int16_t>(v);
}

// ---------------------------------------------------------------------------
// Effect: NONE
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyNone(int16_t* /*samples*/, std::size_t /*count*/) {
    // Identity — nothing to do
}

// ---------------------------------------------------------------------------
// Effect: ROBOT
//
// Classic Transformer robot voice: amplitude ring modulation.
// Each sample is multiplied by a sine carrier to create sum and difference
// frequency components, producing the characteristic metallic robotic sound.
//
//   y[n] = x[n] * sin(2π * f_c * n / Fs)
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyRobot(int16_t* samples, std::size_t count) {
    const float carrierHz = m_params["ROBOT"].carrierHz;
    const double phaseIncrement = TWO_PI * carrierHz / m_sampleRate;

    for (std::size_t i = 0; i < count; ++i) {
        float modulator = static_cast<float>(std::sin(m_ringPhase));
        float out = static_cast<float>(samples[i]) * modulator;
        samples[i] = clampToInt16(out);
        m_ringPhase += phaseIncrement;
        if (m_ringPhase >= TWO_PI) m_ringPhase -= TWO_PI;
    }
}

// ---------------------------------------------------------------------------
// Effect: CHIPMUNK — pitch shift up
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyChipmunk(int16_t* samples, std::size_t count) {
    applyPitchShift(samples, count, m_params["CHIPMUNK"].pitchFactor);
}

// ---------------------------------------------------------------------------
// Effect: DEEP — pitch shift down
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyDeep(int16_t* samples, std::size_t count) {
    applyPitchShift(samples, count, m_params["DEEP"].pitchFactor);
}

// ---------------------------------------------------------------------------
// Effect: ECHO
//
// Single-tap comb delay with feedback:
//   y[n] = x[n] + decay * y[n - D]
// where D is the delay in samples.
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyEcho(int16_t* samples, std::size_t count) {
    if (m_delayBuf.empty()) {
        int delaySamples = static_cast<int>(m_params["ECHO"].delayMs * m_sampleRate / 1000) + 1;
        m_delayBuf.assign(static_cast<std::size_t>(delaySamples), 0.0f);
        m_delayWrite = 0;
    }

    const float decay = m_params["ECHO"].decay;
    const std::size_t D = m_delayBuf.size();

    for (std::size_t i = 0; i < count; ++i) {
        float x = static_cast<float>(samples[i]);
        // Read from delay line (circular buffer read position)
        float delayed = m_delayBuf[m_delayWrite];
        float y = x + decay * delayed;
        m_delayBuf[m_delayWrite] = y;
        m_delayWrite = (m_delayWrite + 1) % D;
        samples[i] = clampToInt16(y);
    }
}

// ---------------------------------------------------------------------------
// Effect: ALIEN
//
// Ring modulation (lower carrier) + LFO chorus/flange.
// The flange modulates the read pointer on a tiny delay to create comb
// filtering that sweeps in and out — the classic sci-fi "alien" timbral
// quality.
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyAlien(int16_t* samples, std::size_t count) {
    const float carrierHz = m_params["ALIEN"].carrierHz;
    const float flangeDepth = m_params["ALIEN"].flangeDepth;
    // LFO at 0.5 Hz sweeps the flange delay
    const double lfoFreq = 0.5;
    const double phaseInc = TWO_PI * carrierHz / m_sampleRate;
    const double lfoInc = TWO_PI * lfoFreq / m_sampleRate;

    // Small flange buffer (max depth = flangeDepth * sampleRate samples)
    const std::size_t maxFlange = static_cast<std::size_t>(flangeDepth * m_sampleRate) + 2;
    if (m_delayBuf.size() < maxFlange) {
        m_delayBuf.assign(maxFlange, 0.0f);
        m_delayWrite = 0;
    }

    for (std::size_t i = 0; i < count; ++i) {
        float x = static_cast<float>(samples[i]);

        // Ring modulation
        float mod = static_cast<float>(std::sin(m_ringPhase));
        x *= mod;
        m_ringPhase += phaseInc;
        if (m_ringPhase >= TWO_PI) m_ringPhase -= TWO_PI;

        // Chorus flange: mix with a delayed (and slowly sweeping) copy
        float lfoVal = static_cast<float>((std::sin(m_lfoPhase) + 1.0) * 0.5);  // [0,1]
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= TWO_PI) m_lfoPhase -= TWO_PI;

        // Compute fractional delay read position
        float delayFrac = lfoVal * static_cast<float>(maxFlange - 1);
        auto d0 = static_cast<std::size_t>(delayFrac);
        float t = delayFrac - static_cast<float>(d0);
        std::size_t rd0 = (m_delayWrite + maxFlange - d0) % maxFlange;
        std::size_t rd1 = (rd0 == 0) ? maxFlange - 1 : rd0 - 1;
        float flanged = m_delayBuf[rd0] * (1.0f - t) + m_delayBuf[rd1] * t;

        m_delayBuf[m_delayWrite] = x;
        m_delayWrite = (m_delayWrite + 1) % maxFlange;

        float y = 0.7f * x + 0.3f * flanged;
        samples[i] = clampToInt16(y);
    }
}

// ---------------------------------------------------------------------------
// Effect: WHISPER
//
// Low amplitude + first-order high-pass filter to remove low-frequency power,
// emulating the spectral character of a whisper.
//
//   H(z) = (1 - z^{-1}) / (1 - α z^{-1}),  α close to 1 (e.g. 0.98)
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyWhisper(int16_t* samples, std::size_t count) {
    const float amp = m_params["WHISPER"].amplitude;
    // High-pass pole — close to 1 gives strong HP character
    const float alpha = 0.95f;

    for (std::size_t i = 0; i < count; ++i) {
        float x = static_cast<float>(samples[i]);
        // First-order difference (high-pass)
        float y = x - m_hpPrev + alpha * m_hpPrevOut;
        m_hpPrev = x;
        m_hpPrevOut = y;
        samples[i] = clampToInt16(y * amp);
    }
}

// ---------------------------------------------------------------------------
// Granular Overlap-Add pitch shift
//
// Time-domain pitch scaling using OLA (Overlap-Add):
//  - Reads input at a rate of 1/factor relative to output
//  - Uses a Hann window to smoothly blend overlapping grains
//
// factor > 1.0 → pitch up (CHIPMUNK)
// factor < 1.0 → pitch down (DEEP)
// ---------------------------------------------------------------------------

void VoiceEffectProcessor::applyPitchShift(int16_t* samples, std::size_t count, float factor) {
    if (factor <= 0.0f || std::abs(factor - 1.0f) < 0.001f) {
        return;  // Passthrough for unity or invalid factor
    }

    // Hann window
    auto hann = [](std::size_t n, std::size_t N) -> float {
        return 0.5f * (1.0f - std::cos(static_cast<float>(TWO_PI * n / (N - 1))));
    };

    std::vector<float> input(count);
    for (std::size_t i = 0; i < count; ++i) {
        input[i] = static_cast<float>(samples[i]);
    }

    std::vector<float> output(count, 0.0f);
    // Grain-hop in the OUTPUT domain
    const std::size_t hopOut = GRAIN_HOP;
    // Corresponding hop in INPUT domain (scaled by 1/factor for pitch change)
    const double hopIn = hopOut / static_cast<double>(factor);

    double readPos = m_readPos;
    std::size_t writePos = 0;

    while (writePos < count) {
        // Build a grain from the input at readPos
        for (std::size_t g = 0; g < GRAIN_SIZE && g < count; ++g) {
            auto ri = static_cast<std::size_t>(readPos) + g;
            float sample = (ri < count) ? input[ri] : 0.0f;
            m_grainBuf[g] = sample * hann(g, GRAIN_SIZE);
        }

        // Add grain into output with overlap
        for (std::size_t g = 0; g < GRAIN_SIZE; ++g) {
            std::size_t op = writePos + g;
            if (op < count) {
                output[op] += m_grainBuf[g];
            }
        }

        readPos += hopIn;
        writePos += hopOut;
    }

    // Save read position for next call (wrap within reason)
    m_readPos = readPos;
    if (m_readPos > static_cast<double>(count) * 2.0) {
        m_readPos = 0.0;
    }

    // Write back, clamping
    for (std::size_t i = 0; i < count; ++i) {
        samples[i] = clampToInt16(output[i]);
    }
}

}  // namespace voiceTransformer
}  // namespace engine
}  // namespace aace
