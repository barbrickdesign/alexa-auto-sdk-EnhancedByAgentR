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

#ifndef AACE_ENGINE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_IMPL_H
#define AACE_ENGINE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_IMPL_H

#include <memory>
#include <mutex>
#include <string>

#include "AACE/VoiceTransformer/VoiceTransformerEngineInterface.h"
#include "VoiceEffectProcessor.h"

namespace aace {
namespace voiceTransformer {
class VoiceTransformer;
}
}

namespace aace {
namespace engine {
namespace voiceTransformer {

/**
 * @brief Engine-side implementation of the VoiceTransformer module.
 *
 * Bridges the @c VoiceTransformer platform interface to the
 * @c VoiceEffectProcessor DSP core.  Thread-safe: mode changes may arrive
 * from any thread while the audio thread calls @c process().
 */
class VoiceTransformerEngineImpl
    : public aace::voiceTransformer::VoiceTransformerEngineInterface
    , public std::enable_shared_from_this<VoiceTransformerEngineImpl> {
private:
    VoiceTransformerEngineImpl(std::shared_ptr<aace::voiceTransformer::VoiceTransformer> platformInterface,
                               int sampleRate);
public:
    ~VoiceTransformerEngineImpl() override = default;

    static std::shared_ptr<VoiceTransformerEngineImpl> create(
        std::shared_ptr<aace::voiceTransformer::VoiceTransformer> platformInterface,
        int sampleRate = VoiceEffectProcessor::DEFAULT_SAMPLE_RATE);

    // -----------------------------------------------------------------------
    // VoiceTransformerEngineInterface
    // -----------------------------------------------------------------------
    bool onSetVoiceEffect(const std::string& mode) override;
    std::string onGetVoiceEffect() override;
    bool onSetEffectParam(const std::string& mode, const std::string& key, const std::string& value) override;

    // -----------------------------------------------------------------------
    // Audio processing — called from the audio pipeline thread
    // -----------------------------------------------------------------------

    /**
     * Process a buffer of 16-bit signed PCM audio in-place.
     * Safe to call concurrently with mode-change calls.
     */
    void process(int16_t* samples, std::size_t count);

    /** Shut down the engine impl and release the platform interface ref. */
    void shutdown();

private:
    std::shared_ptr<aace::voiceTransformer::VoiceTransformer> m_platformInterface;
    std::unique_ptr<VoiceEffectProcessor> m_processor;
    mutable std::mutex m_mutex;
};

}  // namespace voiceTransformer
}  // namespace engine
}  // namespace aace

#endif  // AACE_ENGINE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_IMPL_H
