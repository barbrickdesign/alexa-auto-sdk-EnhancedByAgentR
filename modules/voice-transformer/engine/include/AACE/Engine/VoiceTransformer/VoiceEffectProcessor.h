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

#ifndef AACE_ENGINE_VOICE_TRANSFORMER_VOICE_EFFECT_PROCESSOR_H
#define AACE_ENGINE_VOICE_TRANSFORMER_VOICE_EFFECT_PROCESSOR_H

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

/** @file */

namespace aace {
namespace engine {
namespace voiceTransformer {

/**
 * @brief DSP parameters for each voice effect mode.
 *
 * All values use SI units where applicable.  The platform may override these
 * defaults via VoiceTransformer::setEffectParam().
 */
struct EffectParams {
    // ROBOT / ALIEN: ring modulation carrier frequency (Hz)
    float carrierHz = 100.0f;

    // CHIPMUNK / DEEP: pitch-scaling ratio (>1 raises, <1 lowers pitch)
    float pitchFactor = 1.0f;

    // ECHO: delay buffer length in milliseconds
    int delayMs = 250;

    // ECHO: feedback decay coefficient [0, 1)
    float decay = 0.4f;

    // ALIEN: LFO depth for chorus flange (fraction of sample rate)
    float flangeDepth = 0.005f;

    // WHISPER: output amplitude scalar [0, 1]
    float amplitude = 0.3f;
};

/**
 * @brief Pure-C++ real-time audio DSP processor for voice-transformer effects.
 *
 * Operates on 16-bit signed PCM samples (little-endian, mono, any sample
 * rate — typically 16 000 Hz for Alexa speech).
 *
 * All effects are implemented without external dependencies so the module
 * compiles in any environment that supports C++14.
 *
 * ### Supported modes
 * | Mode       | Algorithm                                                     |
 * |------------|---------------------------------------------------------------|
 * | NONE       | Identity / passthrough                                        |
 * | ROBOT      | Amplitude ring modulation (carrier sine wave)                 |
 * | CHIPMUNK   | Granular pitch-up via OLA (overlap-add) resampling            |
 * | DEEP       | Granular pitch-down via OLA resampling                        |
 * | ECHO       | Single-tap delay line with feedback                           |
 * | ALIEN      | Ring modulation + LFO chorus flange                           |
 * | WHISPER    | Amplitude attenuation + one-pole high-pass filter             |
 */
class VoiceEffectProcessor {
public:
    static constexpr int DEFAULT_SAMPLE_RATE = 16000;

    explicit VoiceEffectProcessor(int sampleRate = DEFAULT_SAMPLE_RATE);
    ~VoiceEffectProcessor() = default;

    // -----------------------------------------------------------------------
    // Configuration
    // -----------------------------------------------------------------------

    /** Set the current voice effect mode ("NONE", "ROBOT", etc.). */
    bool setMode(const std::string& mode);

    /** Return the current mode string. */
    const std::string& getMode() const;

    /** Set a numeric parameter for the given mode. */
    bool setParam(const std::string& mode, const std::string& key, float value);

    /** Reset internal DSP state (phase accumulators, delay buffers, etc.). */
    void reset();

    // -----------------------------------------------------------------------
    // Processing
    // -----------------------------------------------------------------------

    /**
     * Process a buffer of 16-bit signed PCM samples in-place.
     *
     * @param [in,out] samples  Pointer to the sample buffer.
     * @param [in]     count    Number of samples (not bytes) in the buffer.
     */
    void process(int16_t* samples, std::size_t count);

private:
    // Effect implementations — each operates in-place on the sample buffer
    void applyNone(int16_t* samples, std::size_t count);
    void applyRobot(int16_t* samples, std::size_t count);
    void applyChipmunk(int16_t* samples, std::size_t count);
    void applyDeep(int16_t* samples, std::size_t count);
    void applyEcho(int16_t* samples, std::size_t count);
    void applyAlien(int16_t* samples, std::size_t count);
    void applyWhisper(int16_t* samples, std::size_t count);

    /**
     * Granular pitch-shift using overlap-add.
     *
     * @param factor  Output-to-input ratio.  >1 raises pitch, <1 lowers it.
     */
    void applyPitchShift(int16_t* samples, std::size_t count, float factor);

    // Clamp a float to the int16 range and cast
    static int16_t clampToInt16(float v);

private:
    int m_sampleRate;
    std::string m_mode;

    std::unordered_map<std::string, EffectParams> m_params;

    // ROBOT / ALIEN phase accumulator (radians)
    double m_ringPhase = 0.0;

    // ALIEN LFO phase
    double m_lfoPhase = 0.0;

    // ECHO delay line
    std::vector<float> m_delayBuf;
    std::size_t m_delayWrite = 0;

    // WHISPER one-pole high-pass state
    float m_hpPrev = 0.0f;
    float m_hpPrevOut = 0.0f;

    // OLA pitch-shift grain buffer
    static constexpr std::size_t GRAIN_SIZE = 512;
    static constexpr std::size_t GRAIN_HOP = GRAIN_SIZE / 2;
    std::vector<float> m_grainBuf;
    std::vector<float> m_overlapBuf;
    double m_readPos = 0.0;
};

}  // namespace voiceTransformer
}  // namespace engine
}  // namespace aace

#endif  // AACE_ENGINE_VOICE_TRANSFORMER_VOICE_EFFECT_PROCESSOR_H
