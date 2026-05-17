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

#include "AACE/Engine/VoiceTransformer/VoiceTransformerEngineImpl.h"
#include "AACE/VoiceTransformer/VoiceTransformer.h"

#include <stdexcept>

namespace aace {
namespace engine {
namespace voiceTransformer {

VoiceTransformerEngineImpl::VoiceTransformerEngineImpl(
    std::shared_ptr<aace::voiceTransformer::VoiceTransformer> platformInterface,
    int sampleRate)
    : m_platformInterface(std::move(platformInterface))
    , m_processor(std::make_unique<VoiceEffectProcessor>(sampleRate)) {
}

std::shared_ptr<VoiceTransformerEngineImpl> VoiceTransformerEngineImpl::create(
    std::shared_ptr<aace::voiceTransformer::VoiceTransformer> platformInterface,
    int sampleRate) {
    if (!platformInterface) {
        return nullptr;
    }
    auto impl = std::shared_ptr<VoiceTransformerEngineImpl>(
        new VoiceTransformerEngineImpl(std::move(platformInterface), sampleRate));
    impl->m_platformInterface->setEngineInterface(impl);
    return impl;
}

bool VoiceTransformerEngineImpl::onSetVoiceEffect(const std::string& mode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bool ok = m_processor->setMode(mode);
    if (ok && m_platformInterface) {
        // Notify platform (unlock first to avoid deadlock if callback re-enters)
        auto platform = m_platformInterface;
        m_mutex.unlock();
        platform->voiceEffectChanged(mode);
        m_mutex.lock();
    }
    return ok;
}

std::string VoiceTransformerEngineImpl::onGetVoiceEffect() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_processor->getMode();
}

bool VoiceTransformerEngineImpl::onSetEffectParam(
    const std::string& mode,
    const std::string& key,
    const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    try {
        float f = std::stof(value);
        return m_processor->setParam(mode, key, f);
    } catch (...) {
        return false;
    }
}

void VoiceTransformerEngineImpl::process(int16_t* samples, std::size_t count) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_processor->process(samples, count);
}

void VoiceTransformerEngineImpl::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_platformInterface) {
        m_platformInterface->setEngineInterface(nullptr);
        m_platformInterface.reset();
    }
}

}  // namespace voiceTransformer
}  // namespace engine
}  // namespace aace
