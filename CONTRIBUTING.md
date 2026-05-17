# Contribution Guidelines

Thank you for your interest in contributing to Alexa Auto SDK. Whether it's a bug report, new feature request, documentation request, or a correction, we greatly value feedback and contributions from the developer community.

Read the following guidelines before submitting any issues to ensure we have all the necessary information to respond to your request or contribution.

## Report a bug or request a feature

We do not use the GitHub issue tracker. See the [Need Help?](https://alexa.github.io/alexa-auto-sdk/docs/help) page of Auto SDK documentation for details about reporting issues.

## Pull requests

At this time, we do **not** accept pull requests to the upstream Amazon repository. If you want to contribute a code change, follow the same process used to report bugs and request features.

**For this community fork:** Pull requests are welcome! Please follow the quick-start guide below to set up your development environment.

## Quick Start — Local Development

### Prerequisites

| Tool    | Version          | Notes                            |
|---------|------------------|----------------------------------|
| Python  | 3.8+             |                                  |
| Conan   | 1.x (≥ 1.60)    | `pip install conan==1.64.1`      |
| CMake   | ≥ 3.6            |                                  |
| GCC     | ≥ 7 or Clang ≥ 9 | C++14 support required           |
| GTest   | any              | For unit tests (auto-fetched)    |

### One-time setup

```bash
# Install Conan and configure the SDK packages
pip install conan==1.64.1
python conan/setup.py
```

### Build the SDK

```bash
# Default build (all core modules, release, host platform)
./builder/build.py

# Debug build
./builder/build.py --debug

# Include the C++ sample application
./builder/build.py --with-sampleapp

# Include unit tests
./builder/build.py --with-unit-tests

# Build a single module (e.g. voice-transformer)
./builder/build.py -m voice-transformer

# Build multiple specific modules
./builder/build.py -m core alexa cbl voice-transformer
```

The build archive is written to `builder/deploy/`.

### Run unit tests

```bash
./builder/build.py --with-unit-tests
ctest --test-dir builder/deploy --output-on-failure
```

### Clean the cache

```bash
# Clean everything
./builder/build.py clean '*'

# Clean a single module
./builder/build.py clean 'aac-module-voice-transformer'
```

## Adding a New Module

New modules should follow the structure of an existing simple module (e.g.
`modules/loopback-detector` or `modules/voice-transformer`):

```
modules/<your-module>/
├── README.md              ← User-facing documentation
├── conanfile.py           ← Declares module_name and module_requires
├── docs/
│   └── index.md           ← Architecture overview
├── platform/
│   ├── include/AACE/<YourModule>/
│   │   ├── YourModule.h                  ← Public platform interface
│   │   └── YourModuleEngineInterface.h   ← Engine-side callbacks
│   └── src/
│       └── YourModule.cpp
└── engine/
    ├── CMakeLists.txt
    ├── include/AACE/Engine/<YourModule>/
    │   ├── YourModuleEngineImpl.h
    │   └── YourModuleEngineService.h
    └── src/
        ├── YourModuleEngineImpl.cpp
        └── YourModuleEngineService.cpp   ← Must call REGISTER_SERVICE(...)
```

Key rules:
- `conanfile.py` must set `module_name` and `module_requires`.
- The engine service class must use the `DESCRIBE(...)` and `REGISTER_SERVICE(...)`
  macros from `<AACE/Engine/Core/EngineService.h>` and `<AACE/Engine/Core/EngineMacros.h>`.
- Platform headers go under `platform/include/AACE/<YourModule>/`.
- Engine headers go under `engine/include/AACE/Engine/<YourModule>/`.
- Add unit tests under `testing/unit/tests/`.
- Update this repo's `README.md` module inventory table.
- Add an entry to `ENHANCEMENTS.md`.

## Code Style

C++ code follows the LLVM clang-format style defined in `.clang-format` at
the repository root.  Format your changes before submitting:

```bash
find modules/<your-module> -name "*.cpp" -o -name "*.h" \
  | xargs clang-format -i --style=file
```

## Report Security Issues

If you discover a potential security issue in this project, we ask that you notify AWS/Amazon Security via our [vulnerability reporting page](http://aws.amazon.com/security/vulnerability-reporting/). Please do **not** create a public Github issue.

## License

See the [LICENSE](https://github.com/alexa/alexa-auto-sdk/blob/master/LICENSE) file for Auto SDK licensing. We will ask you to confirm the licensing of your contribution.

We may ask you to sign a [Contributor License Agreement (CLA)](http://en.wikipedia.org/wiki/Contributor_License_Agreement) for larger changes.
