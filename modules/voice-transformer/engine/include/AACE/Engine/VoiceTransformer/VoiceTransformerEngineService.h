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

#ifndef AACE_ENGINE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_SERVICE_H
#define AACE_ENGINE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_SERVICE_H

#include <memory>

#include <AACE/Engine/Core/EngineService.h>
#include "VoiceTransformerEngineImpl.h"

namespace aace {
namespace engine {
namespace voiceTransformer {

/**
 * @brief AACE Engine Service for the Voice Transformer module.
 *
 * This service is discovered and instantiated automatically by the AACE
 * Engine when the @c VoiceTransformer platform interface is registered.
 * It depends only on the core engine service.
 */
class VoiceTransformerEngineService : public aace::engine::core::EngineService {
    DESCRIBE("aace.voiceTransformer", VERSION("1.0"))

public:
    ~VoiceTransformerEngineService() override = default;

protected:
    bool configure(std::shared_ptr<std::istream> configuration) override;
    bool registerPlatformInterface(std::shared_ptr<aace::core::PlatformInterface> platformInterface) override;
    bool shutdown() override;

private:
    explicit VoiceTransformerEngineService(const aace::engine::core::ServiceDescription& description);

    // Typed registration helper
    bool registerPlatformInterfaceType(
        std::shared_ptr<aace::voiceTransformer::VoiceTransformer> platformInterface);

private:
    std::shared_ptr<VoiceTransformerEngineImpl> m_engineImpl;
    int m_sampleRate = VoiceEffectProcessor::DEFAULT_SAMPLE_RATE;
};

}  // namespace voiceTransformer
}  // namespace engine
}  // namespace aace

#endif  // AACE_ENGINE_VOICE_TRANSFORMER_VOICE_TRANSFORMER_ENGINE_SERVICE_H
