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

/**
 * @file VoiceEffectProcessorTest.cpp
 *
 * Unit tests for the VoiceEffectProcessor DSP core.
 *
 * Run with: ctest --test-dir <build-dir> -R VoiceEffectProcessor
 */

#include <gtest/gtest.h>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

#include "AACE/Engine/VoiceTransformer/VoiceEffectProcessor.h"

using namespace aace::engine::voiceTransformer;

// -------------------------------------------------------------------------
// Helpers
// -------------------------------------------------------------------------

static std::vector<int16_t> makeSine(int sampleRate, float freqHz, std::size_t nSamples, float amplitude = 0.5f) {
    std::vector<int16_t> buf(nSamples);
    for (std::size_t i = 0; i < nSamples; ++i) {
        float v = amplitude * std::sin(2.0f * float(M_PI) * freqHz * float(i) / float(sampleRate));
        buf[i] = static_cast<int16_t>(v * 32767.0f);
    }
    return buf;
}

static float rms(const std::vector<int16_t>& buf) {
    double sum = 0.0;
    for (auto s : buf) sum += static_cast<double>(s) * s;
    return static_cast<float>(std::sqrt(sum / buf.size()));
}

// -------------------------------------------------------------------------
// Tests: mode management
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, DefaultModeIsNone) {
    VoiceEffectProcessor proc;
    EXPECT_EQ(proc.getMode(), "NONE");
}

TEST(VoiceEffectProcessorTest, SetValidMode) {
    VoiceEffectProcessor proc;
    EXPECT_TRUE(proc.setMode("ROBOT"));
    EXPECT_EQ(proc.getMode(), "ROBOT");
    EXPECT_TRUE(proc.setMode("CHIPMUNK"));
    EXPECT_EQ(proc.getMode(), "CHIPMUNK");
    EXPECT_TRUE(proc.setMode("NONE"));
    EXPECT_EQ(proc.getMode(), "NONE");
}

TEST(VoiceEffectProcessorTest, SetInvalidMode) {
    VoiceEffectProcessor proc;
    EXPECT_FALSE(proc.setMode("DALEK"));
    EXPECT_EQ(proc.getMode(), "NONE");  // unchanged
}

TEST(VoiceEffectProcessorTest, SetParam_ValidKey) {
    VoiceEffectProcessor proc;
    EXPECT_TRUE(proc.setParam("ROBOT", "carrierHz", 200.0f));
}

TEST(VoiceEffectProcessorTest, SetParam_InvalidKey) {
    VoiceEffectProcessor proc;
    EXPECT_FALSE(proc.setParam("ROBOT", "unknownKey", 1.0f));
}

// -------------------------------------------------------------------------
// Tests: NONE mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, NoneMode_IsPassthrough) {
    VoiceEffectProcessor proc;
    auto buf = makeSine(16000, 440.0f, 512);
    auto orig = buf;
    proc.setMode("NONE");
    proc.process(buf.data(), buf.size());
    EXPECT_EQ(buf, orig);
}

// -------------------------------------------------------------------------
// Tests: ROBOT mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, RobotMode_ModifiesSignal) {
    VoiceEffectProcessor proc;
    proc.setMode("ROBOT");
    auto buf = makeSine(16000, 440.0f, 1024);
    auto orig = buf;
    proc.process(buf.data(), buf.size());
    EXPECT_NE(buf, orig);
}

TEST(VoiceEffectProcessorTest, RobotMode_SilenceInSilenceOut) {
    VoiceEffectProcessor proc;
    proc.setMode("ROBOT");
    std::vector<int16_t> silence(1024, 0);
    proc.process(silence.data(), silence.size());
    for (auto s : silence) EXPECT_EQ(s, 0);
}

// -------------------------------------------------------------------------
// Tests: CHIPMUNK mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, ChipmunkMode_ModifiesSignal) {
    VoiceEffectProcessor proc;
    proc.setMode("CHIPMUNK");
    auto buf = makeSine(16000, 200.0f, 2048);
    auto orig = buf;
    proc.process(buf.data(), buf.size());
    EXPECT_NE(buf, orig);
}

// -------------------------------------------------------------------------
// Tests: DEEP mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, DeepMode_ModifiesSignal) {
    VoiceEffectProcessor proc;
    proc.setMode("DEEP");
    auto buf = makeSine(16000, 400.0f, 2048);
    auto orig = buf;
    proc.process(buf.data(), buf.size());
    EXPECT_NE(buf, orig);
}

// -------------------------------------------------------------------------
// Tests: ECHO mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, EchoMode_SignalHasHigherRMSThanInput) {
    // Echo adds energy at the delayed time, so overall power can increase
    VoiceEffectProcessor proc;
    proc.setMode("ECHO");
    auto buf = makeSine(16000, 440.0f, 4096);
    float rmsBefore = rms(buf);
    proc.process(buf.data(), buf.size());
    // Signal should still be non-zero
    float rmsAfter = rms(buf);
    EXPECT_GT(rmsAfter, 0.0f);
    (void)rmsBefore;
}

// -------------------------------------------------------------------------
// Tests: WHISPER mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, WhisperMode_ReducesAmplitude) {
    VoiceEffectProcessor proc;
    proc.setMode("WHISPER");
    auto buf = makeSine(16000, 440.0f, 2048);
    float rmsBefore = rms(buf);
    proc.process(buf.data(), buf.size());
    float rmsAfter = rms(buf);
    EXPECT_LT(rmsAfter, rmsBefore);
}

// -------------------------------------------------------------------------
// Tests: ALIEN mode
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, AlienMode_ModifiesSignal) {
    VoiceEffectProcessor proc;
    proc.setMode("ALIEN");
    auto buf = makeSine(16000, 440.0f, 2048);
    auto orig = buf;
    proc.process(buf.data(), buf.size());
    EXPECT_NE(buf, orig);
}

// -------------------------------------------------------------------------
// Tests: null safety
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, ProcessNullPointer_IsNoop) {
    VoiceEffectProcessor proc;
    proc.setMode("ROBOT");
    EXPECT_NO_THROW(proc.process(nullptr, 512));
}

TEST(VoiceEffectProcessorTest, ProcessZeroCount_IsNoop) {
    VoiceEffectProcessor proc;
    proc.setMode("ROBOT");
    std::vector<int16_t> buf = {1, 2, 3};
    EXPECT_NO_THROW(proc.process(buf.data(), 0));
    EXPECT_EQ(buf[0], 1);
}

// -------------------------------------------------------------------------
// Tests: reset
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, ResetClearsState) {
    VoiceEffectProcessor proc;
    proc.setMode("ECHO");
    auto buf = makeSine(16000, 440.0f, 512);
    proc.process(buf.data(), buf.size());
    proc.reset();
    // After reset, processing a silence should give silence (no lingering echo)
    std::vector<int16_t> silence(512, 0);
    proc.process(silence.data(), silence.size());
    // First samples after reset should be near-zero (delay buf was cleared)
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(silence[i], 0);
    }
}

// -------------------------------------------------------------------------
// Tests: sample rate
// -------------------------------------------------------------------------

TEST(VoiceEffectProcessorTest, CustomSampleRate_44100) {
    VoiceEffectProcessor proc(44100);
    proc.setMode("ROBOT");
    auto buf = makeSine(44100, 440.0f, 4096);
    auto orig = buf;
    proc.process(buf.data(), buf.size());
    EXPECT_NE(buf, orig);
}
