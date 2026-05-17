# Voice Transformer Module

## Overview

The Voice Transformer module provides real-time DSP audio effect "modes" for
Alexa Auto SDK that transform speech in fun and creative ways.

Seven modes are available out of the box:

| Mode       | Description                                            |
|------------|--------------------------------------------------------|
| NONE       | Passthrough (default, no transformation).              |
| ROBOT      | Ring-modulated Transformers-style robotic voice.       |
| CHIPMUNK   | High-pitched playful voice via OLA pitch shift.        |
| DEEP       | Low, imposing voice via OLA pitch shift.               |
| ECHO       | Delay/echo with feedback.                              |
| ALIEN      | Sci-fi ring-mod + LFO flange chorus.                   |
| WHISPER    | Low-amplitude high-pass whisper effect.                |

## Using the Module

See the [Voice Transformer module README](../README.md) for the full usage
guide, API reference, effect parameter documentation, and build instructions.

## Architecture

```
VoiceTransformer (platform interface)
        │
        │  registers with engine
        ▼
VoiceTransformerEngineService (AACE service, depends on core only)
        │
        │  creates
        ▼
VoiceTransformerEngineImpl (engine ↔ platform bridge, thread-safe)
        │
        │  delegates DSP work to
        ▼
VoiceEffectProcessor (pure-C++14 DSP core, no external dependencies)
        │
        ├── applyRobot()    → ring modulation
        ├── applyChipmunk() → OLA pitch-up
        ├── applyDeep()     → OLA pitch-down
        ├── applyEcho()     → comb delay line
        ├── applyAlien()    → ring mod + LFO flange
        └── applyWhisper()  → HP filter + attenuation
```

## Files

```
modules/voice-transformer/
├── README.md                      ← user-facing README
├── conanfile.py                   ← Conan package descriptor
├── docs/
│   └── index.md                   ← this file
├── platform/
│   ├── include/AACE/VoiceTransformer/
│   │   ├── VoiceTransformer.h             ← public platform API
│   │   └── VoiceTransformerEngineInterface.h
│   └── src/
│       └── VoiceTransformer.cpp
└── engine/
    ├── CMakeLists.txt
    ├── include/AACE/Engine/VoiceTransformer/
    │   ├── VoiceEffectProcessor.h         ← DSP core header
    │   ├── VoiceTransformerEngineImpl.h   ← bridge
    │   └── VoiceTransformerEngineService.h← service registration
    └── src/
        ├── VoiceEffectProcessor.cpp       ← all DSP algorithms
        ├── VoiceTransformerEngineImpl.cpp
        └── VoiceTransformerEngineService.cpp
```
