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

#ifndef AACE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_INTERFACE_H
#define AACE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_INTERFACE_H

#include <string>

/** @file */

namespace aace {
namespace voiceTransformer {

/**
 * Engine interface for the VoiceTransformer platform.
 * Implemented by the engine; called by the platform implementation.
 */
class VoiceTransformerEngineInterface {
public:
    virtual ~VoiceTransformerEngineInterface() = default;

    /**
     * Set the active voice effect mode.
     *
     * @param [in] mode Name of the voice effect mode (e.g. "ROBOT", "CHIPMUNK").
     *             Pass "NONE" to disable all effects.
     * @return @c true on success, @c false if the mode is not recognised.
     */
    virtual bool onSetVoiceEffect(const std::string& mode) = 0;

    /**
     * Get the currently active voice effect mode name.
     *
     * @return Mode name string.
     */
    virtual std::string onGetVoiceEffect() = 0;

    /**
     * Set an effect-specific parameter.
     *
     * @param [in] mode   The effect mode the parameter applies to.
     * @param [in] key    Parameter key (e.g. "carrierHz", "pitchFactor", "delayMs").
     * @param [in] value  String-encoded parameter value.
     * @return @c true on success.
     */
    virtual bool onSetEffectParam(const std::string& mode, const std::string& key, const std::string& value) = 0;
};

}  // namespace voiceTransformer
}  // namespace aace

#endif  // AACE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_INTERFACE_H
