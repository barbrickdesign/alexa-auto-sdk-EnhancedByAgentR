cmake_minimum_required(VERSION 3.6)

# ---------------------------------------------------------------------------
# AACE Voice Transformer Engine Library
# ---------------------------------------------------------------------------
# Provides fun DSP voice-effect modes (ROBOT, CHIPMUNK, DEEP, ECHO, ALIEN,
# WHISPER) that transform Alexa speech and microphone audio in real time.
# No external DSP libraries required — all effects are pure C++14.
# ---------------------------------------------------------------------------

project(AACEVoiceTransformerEngine CXX)

add_library(AACEVoiceTransformerEngine SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VoiceEffectProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VoiceTransformerEngineImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VoiceTransformerEngineService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../platform/src/VoiceTransformer.cpp
)

target_include_directories(AACEVoiceTransformerEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../platform/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(AACEVoiceTransformerEngine PUBLIC cxx_std_14)

target_link_libraries(AACEVoiceTransformerEngine
    PUBLIC
        AACECoreEngine
)

install(
    TARGETS AACEVoiceTransformerEngine
    DESTINATION lib
    EXPORT AACEVoiceTransformer
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../platform/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
