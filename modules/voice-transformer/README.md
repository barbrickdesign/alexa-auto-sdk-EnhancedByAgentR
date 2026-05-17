# Voice Transformer Module

The Voice Transformer module adds real-time DSP voice-effect modes to the Alexa Auto SDK.
Switch Alexa's output (or the microphone input) to sound like a robot, chipmunk, deep giant,
alien, echo chamber, or whispering ghost — all in pure C++14 with no external DSP libraries.

## Voice Effect Modes

| Mode        | Description                                                                     |
|-------------|---------------------------------------------------------------------------------|
| `NONE`      | Default passthrough — no transformation applied.                                |
| `ROBOT`     | Classic Transformers robotic voice via amplitude ring modulation.               |
| `CHIPMUNK`  | Playfully high-pitched voice via granular pitch-up (OLA resampling, ×1.5).     |
| `DEEP`      | Slow, imposing low-frequency voice via granular pitch-down (OLA, ×0.7).        |
| `ECHO`      | Single-tap comb delay with configurable delay (default 250 ms) and decay.      |
| `ALIEN`     | Sci-fi extraterrestrial sound: ring modulation + LFO chorus/flange sweep.      |
| `WHISPER`   | Soft whisper: amplitude attenuation + first-order high-pass spectral shaping.  |

## Quick Start

### 1. Register the platform interface

```cpp
#include <AACE/VoiceTransformer/VoiceTransformer.h>

class MyVoiceTransformer : public aace::voiceTransformer::VoiceTransformer {
public:
    void voiceEffectChanged(const std::string& newMode) override {
        std::cout << "[VoiceTransformer] Active mode: " << newMode << "\n";
    }
};

// During engine setup:
auto vt = std::make_shared<MyVoiceTransformer>();
engine->registerPlatformInterface(vt);
```

### 2. Switch modes at runtime

```cpp
// Engage robot voice (Transformers-style!)
vt->setVoiceEffect(aace::voiceTransformer::VoiceTransformer::MODE_ROBOT);

// Back to normal
vt->setVoiceEffect(aace::voiceTransformer::VoiceTransformer::MODE_NONE);
```

### 3. Fine-tune effect parameters

```cpp
// Make the robot voice use a higher carrier frequency (more metallic)
vt->setEffectParam("ROBOT", "carrierHz", "200.0");

// Adjust echo delay and decay
vt->setEffectParam("ECHO", "delayMs", "400");
vt->setEffectParam("ECHO", "decay", "0.6");

// Make chipmunk extra squeaky
vt->setEffectParam("CHIPMUNK", "pitchFactor", "2.0");
```

## Configuration (optional)

An optional JSON configuration block can be passed to the engine to set the
PCM sample rate (default 16 000 Hz, matching standard Alexa speech):

```json
{
    "aace.voiceTransformer": {
        "sampleRate": 16000
    }
}
```

## Effect Parameter Reference

### ROBOT
| Key          | Type  | Default | Description                                      |
|--------------|-------|---------|--------------------------------------------------|
| `carrierHz`  | float | 100.0   | Ring-modulation carrier frequency in Hz.         |

### CHIPMUNK
| Key           | Type  | Default | Description                                   |
|---------------|-------|---------|-----------------------------------------------|
| `pitchFactor` | float | 1.5     | Pitch-scaling ratio (>1.0 raises pitch).      |

### DEEP
| Key           | Type  | Default | Description                                   |
|---------------|-------|---------|-----------------------------------------------|
| `pitchFactor` | float | 0.7     | Pitch-scaling ratio (<1.0 lowers pitch).      |

### ECHO
| Key        | Type  | Default | Description                                         |
|------------|-------|---------|-----------------------------------------------------|
| `delayMs`  | int   | 250     | Delay line length in milliseconds.                  |
| `decay`    | float | 0.4     | Feedback coefficient \[0, 1). Higher = longer tail. |

### ALIEN
| Key           | Type  | Default | Description                                       |
|---------------|-------|---------|---------------------------------------------------|
| `carrierHz`   | float | 50.0    | Ring-modulation carrier frequency in Hz.          |
| `flangeDepth` | float | 0.005   | LFO chorus depth (fraction of sample rate).       |

### WHISPER
| Key         | Type  | Default | Description                                         |
|-------------|-------|---------|-----------------------------------------------------|
| `amplitude` | float | 0.3     | Output amplitude scalar \[0, 1].                    |

## DSP Algorithm Notes

| Mode      | Algorithm                                                                         |
|-----------|-----------------------------------------------------------------------------------|
| ROBOT     | Amplitude modulation: `y[n] = x[n] · sin(2π·f_c·n/Fs)` at f_c = 100 Hz          |
| CHIPMUNK  | Granular OLA pitch-shift (Hann-windowed, grain = 512 samples, hop = 256)          |
| DEEP      | Same OLA pitch-shift with factor < 1                                              |
| ECHO      | Single-tap comb filter: `y[n] = x[n] + α·y[n−D]`                                 |
| ALIEN     | Ring modulation at 50 Hz + 0.5-Hz LFO flange chorus                               |
| WHISPER   | `y[n] = A·(x[n] − x[n−1] + 0.95·y[n−1])` (high-pass + amplitude attenuation)    |

## Building

The module is built as part of the Auto SDK using the Builder Tool:

```bash
./builder/build.py -m voice-transformer
```

Or with Conan directly:

```bash
conan install modules/voice-transformer -if=build-voice-transformer -b missing
conan build modules/voice-transformer -bf=build-voice-transformer
```

## Unit Tests

Unit tests cover all six DSP modes, null safety, reset behaviour, and custom
sample rates. Run them after building with unit tests enabled:

```bash
./builder/build.py -m voice-transformer --with-unit-tests
ctest --test-dir builder/deploy --output-on-failure
```

## License

Apache License 2.0 — see the [LICENSE](../../LICENSE) file.
