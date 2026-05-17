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

#include "AACE/VoiceTransformer/VoiceTransformer.h"

namespace aace {
namespace voiceTransformer {

// Mode string constants
const std::string VoiceTransformer::MODE_NONE = "NONE";
const std::string VoiceTransformer::MODE_ROBOT = "ROBOT";
const std::string VoiceTransformer::MODE_CHIPMUNK = "CHIPMUNK";
const std::string VoiceTransformer::MODE_DEEP = "DEEP";
const std::string VoiceTransformer::MODE_ECHO = "ECHO";
const std::string VoiceTransformer::MODE_ALIEN = "ALIEN";
const std::string VoiceTransformer::MODE_WHISPER = "WHISPER";

VoiceTransformer::~VoiceTransformer() = default;

bool VoiceTransformer::setVoiceEffect(const std::string& mode) {
    if (auto engine = m_engineInterface.lock()) {
        return engine->onSetVoiceEffect(mode);
    }
    return false;
}

std::string VoiceTransformer::getVoiceEffect() {
    if (auto engine = m_engineInterface.lock()) {
        return engine->onGetVoiceEffect();
    }
    return MODE_NONE;
}

bool VoiceTransformer::setEffectParam(
    const std::string& mode,
    const std::string& key,
    const std::string& value) {
    if (auto engine = m_engineInterface.lock()) {
        return engine->onSetEffectParam(mode, key, value);
    }
    return false;
}

void VoiceTransformer::setEngineInterface(std::shared_ptr<VoiceTransformerEngineInterface> engineInterface) {
    m_engineInterface = engineInterface;
}

}  // namespace voiceTransformer
}  // namespace aace
