# Enhancements — Alexa Auto SDK (Community Fork)

This document tracks all improvements made in this community fork beyond the
original Amazon Alexa Auto SDK 4.x codebase.

---

## 2026-05-17 — Voice Transformer Module + Developer-Experience Pass

### 🎉 New: `modules/voice-transformer`

A brand-new SDK module that applies real-time DSP voice-effect "modes" to
Alexa speech and microphone audio.  Implemented entirely in pure C++14 — no
external DSP libraries required.

**Voice effect modes**

| Mode       | Algorithm                                             |
|------------|-------------------------------------------------------|
| `NONE`     | Passthrough (default)                                 |
| `ROBOT`    | Amplitude ring modulation at a configurable carrier   |
| `CHIPMUNK` | Granular OLA pitch-up (×1.5 by default)               |
| `DEEP`     | Granular OLA pitch-down (×0.7 by default)             |
| `ECHO`     | Single-tap comb delay with feedback                   |
| `ALIEN`    | Ring modulation + LFO chorus/flange sweep             |
| `WHISPER`  | Amplitude attenuation + first-order high-pass filter  |

All effect parameters (carrier frequency, pitch factor, delay length, decay,
LFO depth, amplitude) are tunable at runtime via `setEffectParam()`.

**New files**

```
modules/voice-transformer/
├── README.md
├── conanfile.py
├── docs/index.md
├── platform/
│   ├── include/AACE/VoiceTransformer/VoiceTransformer.h
│   ├── include/AACE/VoiceTransformer/VoiceTransformerEngineInterface.h
│   └── src/VoiceTransformer.cpp
├── engine/
│   ├── CMakeLists.txt
│   ├── include/AACE/Engine/VoiceTransformer/VoiceEffectProcessor.h
│   ├── include/AACE/Engine/VoiceTransformer/VoiceTransformerEngineImpl.h
│   ├── include/AACE/Engine/VoiceTransformer/VoiceTransformerEngineService.h
│   ├── src/VoiceEffectProcessor.cpp
│   ├── src/VoiceTransformerEngineImpl.cpp
│   └── src/VoiceTransformerEngineService.cpp
└── testing/unit/tests/VoiceEffectProcessorTest.cpp
```

**Unit tests** — 20 test cases covering:
- Mode management (set / get / invalid mode rejection)
- NONE passthrough identity
- ROBOT, CHIPMUNK, DEEP, ECHO, ALIEN, WHISPER signal modification
- Silence-in → silence-out for ring modulation
- Whisper amplitude reduction
- Null-pointer and zero-count safety
- `reset()` clears delay / phase state
- Custom sample rate (44 100 Hz)

### Documentation improvements

- **`README.md`** — added repository layout map, Voice Transformer callout
  table, Quick Start build commands, and link to `CONTRIBUTING.md`.
- **`CONTRIBUTING.md`** — added Quick Start for local development (build,
  test, lint) and module creation guidelines.
- **`ENHANCEMENTS.md`** (this file) — new changelog for community changes.
- **`modules/voice-transformer/README.md`** — full user guide including API
  reference, effect parameter tables, DSP algorithm notes, and build steps.
- **`modules/voice-transformer/docs/index.md`** — architecture overview and
  file-tree reference.

---

## Contributing Your Own Enhancements

If you add features or fixes to this fork, please add an entry above (newest
first) following the format:

```markdown
## YYYY-MM-DD — Short Title

### Section (e.g. New Feature / Bug Fix / Documentation)

Description of the change, affected files, and test coverage.
```
