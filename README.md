> [!CAUTION]
> **This repository is no longer actively maintained or supported by Amazon.**
> Thank you for your interest in this project.

> [!NOTE]
> This fork includes community enhancements — see [ENHANCEMENTS.md](./ENHANCEMENTS.md) for details.

# Alexa Auto SDK

## About

The Alexa Auto SDK contains essential client-side software required to integrate Alexa into the automobile. The Auto SDK provides libraries that connect to Alexa and expose interfaces for your vehicle software to implement the platform-specific behavior for audio input, media streaming, calling through a connected phone, turn-by-turn navigation, controlling vehicle features such as heaters and lights, and more. You can use the included sample application to learn about the Auto SDK interfaces and to test interactions before integration.

[Learn more in the Auto SDK documentation »](https://alexa.github.io/alexa-auto-sdk/docs/get-started/)

## Repository Layout

```
alexa-auto-sdk/
├── modules/              # SDK modules — each self-contained with engine + platform + docs
│   ├── core/             # Engine core, audio pipeline, property manager, storage
│   ├── alexa/            # Alexa integration (speech recogniser, media player, alerts…)
│   ├── cbl/              # Code-Based Linking (CBL) authorisation flow
│   ├── aasb/             # Alexa Auto Services Bridge (message-broker transport)
│   ├── system-audio/     # Platform audio I/O via GStreamer / OpenMAX AL / QSA
│   ├── address-book/     # Contact & navigation-favourites upload to Alexa
│   ├── alexa/            # Full Alexa client capabilities
│   ├── apl/              # Alexa Presentation Language (APL) rendering
│   ├── bluetooth/        # Bluetooth device integration
│   ├── car-control/      # In-vehicle feature control (HVAC, lights, locks…)
│   ├── cbl/              # Code-Based Linking for device authorisation
│   ├── connectivity/     # Network connectivity status reporting
│   ├── custom-domain/    # Custom Alexa skill domain support
│   ├── loopback-detector/# Echo / loopback detection to prevent false wakewords
│   ├── messaging/        # SMS / messaging via connected phone
│   ├── mobile-bridge/    # Mobile-phone bridging (tethering, hotspot)
│   ├── navigation/       # Turn-by-turn navigation integration
│   ├── phone-control/    # Phone call control via connected device
│   ├── text-to-speech/   # TTS platform interface
│   ├── text-to-speech-provider/ # Alexa cloud TTS provider
│   └── voice-transformer/ # 🎉 NEW — Fun real-time DSP voice effects (ROBOT, CHIPMUNK…)
├── samples/cpp/          # Native C++ sample application
├── builder/              # Builder Tool (build.py) — Conan-based build orchestrator
├── conan/                # Conan recipe overrides and configuration
├── tools/                # Developer utilities (a2ml etc.)
└── docs/                 # Full documentation sources
```

## 🎉 Voice Transformer Module (Community Enhancement)

This fork adds the **Voice Transformer** module — a pure-C++14 DSP engine that
transforms Alexa's speech or microphone audio in real time with fun effects:

| Mode       | Effect                                           |
|------------|--------------------------------------------------|
| `NONE`     | Passthrough (default)                            |
| `ROBOT`    | Classic Transformers robotic voice               |
| `CHIPMUNK` | Playfully high-pitched voice                     |
| `DEEP`     | Slow, imposing low-frequency voice               |
| `ECHO`     | Echo / delay with configurable feedback          |
| `ALIEN`    | Sci-fi ring-mod + LFO chorus flange              |
| `WHISPER`  | Soft whisper via amplitude + high-pass shaping   |

→ See [modules/voice-transformer/README.md](modules/voice-transformer/README.md) for full docs and API.

## Product guidelines

The [Product Requirements and Guidelines](https://alexa.github.io/alexa-auto-sdk/docs/product-guidelines/) docs describe requirements and principals to follow when designing and implementing an Auto SDK client integration for your vehicle. Your integration must follow the product requirements and pass Amazon's automotive certification process.

## What's new in Auto SDK?

Auto SDK is always improving. See the [release notes](https://alexa.github.io/alexa-auto-sdk/docs/releases/) in the Auto SDK documentation to learn about the latest features and enhancements.

## Developer resources

* Follow the [Get Started with Auto SDK](https://alexa.github.io/alexa-auto-sdk/docs/get-started/) guide to set up the Auto SDK development prerequisites.

* Read the [Explore Auto SDK Concepts](https://alexa.github.io/alexa-auto-sdk/docs/explore/concepts/) docs to learn how Auto SDK works and understand its core APIs.

* Read the [Explore Auto SDK Features](https://alexa.github.io/alexa-auto-sdk/docs/explore/features/) docs to learn about all the different features Auto SDK provides.
  
* Follow the developer docs for [Android](https://alexa.github.io/alexa-auto-sdk/docs/android/) or [Native C++](https://alexa.github.io/alexa-auto-sdk/docs/native/) to guide you through integrating Auto SDK into your vehicle.

* Use the [Auto SDK Migration Guide](https://alexa.github.io/alexa-auto-sdk/docs/releases/migration) to ease your upgrade to the latest Auto SDK version.

* Read [CONTRIBUTING.md](./CONTRIBUTING.md) for contribution guidelines and local development tips.

## Quick Start (Native C++)

```bash
# 1. Install prerequisites: Python 3, Conan 1.x, CMake ≥ 3.6, GCC/Clang
pip install conan==1.64.1

# 2. Configure Conan for the SDK
python conan/setup.py

# 3. Build with the Builder Tool (Linux x86_64, release)
./builder/build.py

# 4. Build with the sample app and unit tests included
./builder/build.py --with-sampleapp --with-unit-tests

# 5. Build only the voice-transformer module
./builder/build.py -m voice-transformer
```

## License

The contents of this repository are distributed under several license agreements. Refer to the [LICENSE](https://github.com/alexa/alexa-auto-sdk/blob/master/LICENSE) file for the license terms applicable to the materials that you use.
