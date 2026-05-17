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

#ifndef AACE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_H
#define AACE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_H

#include <memory>
#include <string>

#include "AACE/Core/PlatformInterface.h"
#include "VoiceTransformerEngineInterface.h"

/** @file */

namespace aace {
namespace voiceTransformer {

/**
 * @brief Platform interface for the Voice Transformer module.
 *
 * The Voice Transformer module applies real-time DSP audio effects to Alexa
 * speech and microphone audio, enabling fun "transformer" voice modes that
 * change how Alexa sounds or how the user's voice is processed.
 *
 * ## Supported Voice Modes
 *
 * | Mode       | Description                                                          |
 * |------------|----------------------------------------------------------------------|
 * | NONE       | No effect — passthrough audio (default).                             |
 * | ROBOT      | Classic Transformers robotic voice via ring modulation + distortion. |
 * | CHIPMUNK   | High-pitched playful voice via time-domain pitch shift.              |
 * | DEEP       | Slow, imposing low-pitched voice via time-domain pitch shift.        |
 * | ECHO       | Echo / delay effect with configurable delay and decay.               |
 * | ALIEN      | Sci-fi alien voice: ring modulation + chorus flanging.               |
 * | WHISPER    | Soft whisper voice: amplitude reduction + slight high-pass filter.   |
 *
 * ## Usage
 *
 * 1. Extend this class in your platform implementation.
 * 2. Call @c setVoiceEffect() to switch between modes at runtime.
 * 3. Override @c voiceEffectChanged() to react when the engine confirms
 *    a mode change (e.g. to update your UI).
 *
 * @code
 * class MyVoiceTransformer : public aace::voiceTransformer::VoiceTransformer {
 * public:
 *     void voiceEffectChanged(const std::string& newMode) override {
 *         std::cout << "Voice effect active: " << newMode << std::endl;
 *     }
 * };
 * @endcode
 */
class VoiceTransformer : public aace::core::PlatformInterface {
protected:
    VoiceTransformer() = default;

public:
    virtual ~VoiceTransformer();

    // -----------------------------------------------------------------------
    // Mode constants
    // -----------------------------------------------------------------------

    /// No audio transformation; passthrough.
    static const std::string MODE_NONE;

    /// Classic robotic transformer voice (ring modulation).
    static const std::string MODE_ROBOT;

    /// High-pitched chipmunk voice (pitch shift up).
    static const std::string MODE_CHIPMUNK;

    /// Deep, imposing low voice (pitch shift down).
    static const std::string MODE_DEEP;

    /// Echo / delay effect.
    static const std::string MODE_ECHO;

    /// Sci-fi alien voice (ring mod + flanging).
    static const std::string MODE_ALIEN;

    /// Soft whisper voice (low amplitude + high-pass filter).
    static const std::string MODE_WHISPER;

    // -----------------------------------------------------------------------
    // Platform → Engine calls
    // -----------------------------------------------------------------------

    /**
     * Set the active voice effect mode.
     *
     * @param [in] mode  One of the @c MODE_* constants defined on this class.
     * @return @c true if the engine accepted the mode, @c false otherwise.
     */
    bool setVoiceEffect(const std::string& mode);

    /**
     * Get the currently active voice effect mode.
     *
     * @return Mode name string (one of the @c MODE_* constants).
     */
    std::string getVoiceEffect();

    /**
     * Set an effect-specific parameter for fine-tuning.
     *
     * Example parameter keys per mode:
     * - ROBOT:    "carrierHz"   (float, default 100.0)
     * - CHIPMUNK: "pitchFactor" (float > 1.0, default 1.5)
     * - DEEP:     "pitchFactor" (float < 1.0, default 0.7)
     * - ECHO:     "delayMs"     (int, default 250), "decay" (float 0–1, default 0.4)
     * - ALIEN:    "carrierHz"   (float, default 50.0), "flangeDepth" (float, default 0.005)
     * - WHISPER:  "amplitude"   (float 0–1, default 0.3)
     *
     * @param [in] mode   Effect mode the parameter applies to.
     * @param [in] key    Parameter key.
     * @param [in] value  String-encoded parameter value.
     * @return @c true on success.
     */
    bool setEffectParam(const std::string& mode, const std::string& key, const std::string& value);

    // -----------------------------------------------------------------------
    // Engine → Platform callbacks
    // -----------------------------------------------------------------------

    /**
     * Called by the engine when the active voice effect mode changes.
     *
     * Override this in your platform implementation to update any UI indicators.
     *
     * @param [in] newMode  The mode that is now active.
     */
    virtual void voiceEffectChanged(const std::string& /*newMode*/) {
    }

    // -----------------------------------------------------------------------
    // Internal — do not call from platform code
    // -----------------------------------------------------------------------

    /** @internal */
    void setEngineInterface(std::shared_ptr<VoiceTransformerEngineInterface> engineInterface);

private:
    std::weak_ptr<VoiceTransformerEngineInterface> m_engineInterface;
};

}  // namespace voiceTransformer
}  // namespace aace

#endif  // AACE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_H
