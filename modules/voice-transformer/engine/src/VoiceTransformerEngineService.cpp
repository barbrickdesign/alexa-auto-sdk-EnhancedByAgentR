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

#include "AACE/Engine/VoiceTransformer/VoiceTransformerEngineService.h"
#include "AACE/VoiceTransformer/VoiceTransformer.h"

#include <AACE/Engine/Core/EngineMacros.h>

namespace aace {
namespace engine {
namespace voiceTransformer {

static const std::string TAG("aace.voiceTransformer.VoiceTransformerEngineService");

REGISTER_SERVICE(VoiceTransformerEngineService);

VoiceTransformerEngineService::VoiceTransformerEngineService(
    const aace::engine::core::ServiceDescription& description)
    : aace::engine::core::EngineService(description) {
}

bool VoiceTransformerEngineService::configure(std::shared_ptr<std::istream> configuration) {
    // Optional JSON configuration: {"sampleRate": 16000}
    // If absent the default (16 kHz) is used.
    try {
        if (!configuration) {
            return true;
        }
        std::string content((std::istreambuf_iterator<char>(*configuration)),
                            std::istreambuf_iterator<char>());
        // Simple key search — avoids a full JSON parser dependency
        auto pos = content.find("\"sampleRate\"");
        if (pos != std::string::npos) {
            auto colon = content.find(':', pos);
            if (colon != std::string::npos) {
                m_sampleRate = std::stoi(content.substr(colon + 1));
            }
        }
        return true;
    } catch (std::exception& ex) {
        AACE_WARN(LX(TAG, "configure").d("reason", ex.what()));
        return false;
    }
}

bool VoiceTransformerEngineService::registerPlatformInterface(
    std::shared_ptr<aace::core::PlatformInterface> platformInterface) {
    // Attempt to cast to VoiceTransformer; ignore unrelated platform interfaces
    auto typed = std::dynamic_pointer_cast<aace::voiceTransformer::VoiceTransformer>(platformInterface);
    if (typed) {
        return registerPlatformInterfaceType(typed);
    }
    return false;
}

bool VoiceTransformerEngineService::registerPlatformInterfaceType(
    std::shared_ptr<aace::voiceTransformer::VoiceTransformer> platformInterface) {
    try {
        ThrowIfNotNull(m_engineImpl, "VoiceTransformer platform interface already registered");
        m_engineImpl = VoiceTransformerEngineImpl::create(platformInterface, m_sampleRate);
        ThrowIfNull(m_engineImpl, "Failed to create VoiceTransformerEngineImpl");
        AACE_INFO(LX(TAG).m("VoiceTransformer registered successfully"));
        return true;
    } catch (std::exception& ex) {
        AACE_ERROR(LX(TAG, "registerPlatformInterfaceType").d("reason", ex.what()));
        return false;
    }
}

bool VoiceTransformerEngineService::shutdown() {
    if (m_engineImpl) {
        m_engineImpl->shutdown();
        m_engineImpl.reset();
    }
    return true;
}

}  // namespace voiceTransformer
}  // namespace engine
}  // namespace aace
