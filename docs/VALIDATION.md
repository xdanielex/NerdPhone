# Validation summary

Artifact: **`0.2.4-midstate-test`**, versionCode **10**, package `it.nerdphone`, min API 26, target API 35. Release build, `debuggable` disabled, distribution-signed. The version name is intentionally unchanged so that it matches the APK that was actually tested.

- JDK 17, Gradle 8.9, Android SDK 35, NDK 27.0.12077973, CMake 3.22.1.
- Build, lint and signature verification passed. Three ABIs: `arm64-v8a`, `armeabi-v7a`, `x86_64`.
- Host tests passed: SHA-256 vectors and genesis block, target arithmetic, merkle/header construction, **20,480 optimised-versus-reference header comparisons**, eight-hex-digit nonce formatting, CPU detection and worker partitioning, duplicate-share checking, difficulty negotiation, and an independent mock pool that validates submitted shares.
- Field test: one UMIDIGI Power 3 on Android 10 with eight workers, screen off, pool-assigned difficulty 0.001. Pool estimate stayed near **4 MH/s** for about **12 hours**; share-equivalent averages from the counters were **4.00–4.08 MH/s**.
- Acceptance in the captured session: **56,952 accepted of 56,999 submitted**, **47 rejected** and all reported as `Stale` — about **0.08%**.
- Battery: about **10% per hour** with the screen off; ~1.8 W estimated for the whole phone under the stated nominal-capacity assumption. Not a wall-plug measurement.
- Tester reports the phone remained cold to the touch with no cooling accessory. Internal and battery temperatures were **not measured**.
- One device, one operator, one pool. Not a safety, energy-efficiency or compatibility certification.

APK SHA-256: `fcc28ff97a62e363348078bc5b6f0186d2fc47b4930c822d5768862cc66812b3`
Distribution certificate SHA-256: `43bb0e9fd881b7bbe8f0739a03774e51319a94992e72efbc3e6dbaec69be2962`

Still open and useful: reconnect and job-transition coverage, Stop/update lifecycle tests, longer runs, additional devices, and a full block-candidate alert and submission test.

See the [field report](benchmarks/first-device.md) and the [release checklist](RELEASING.md). Public CI runs the tests and compiles a debug APK without uploading APK/AAB artefacts. Private signing credentials are never part of the source tree.
