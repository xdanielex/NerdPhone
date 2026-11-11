# Validation summary — selected midstate build

Selected artifact: **0.2.4-midstate-test**, versionCode **10**, package `it.nerdphone`, min API 26, target API 35. Release build, debuggable disabled, distribution signed with the same identity as 0.2.2/0.2.3. The version name is intentionally unchanged to match the tested APK.

- JDK 17, Gradle 8.9, SDK 35, NDK 27.0.12077973, CMake 3.22.1.
- Build/lint and signature verification passed. Three ABIs: arm64-v8a, armeabi-v7a, x86_64.
- Host tests passed: SHA/genesis/targets/header construction, 20,480 optimized/reference header comparisons, nonce formatting, CPU/worker tests, independent simulated-pool share validation, duplicate checking and difficulty negotiation.
- Initial field test: UMIDIGI Power 3, Android 10, eight workers, reported difficulty 0.001 and screen-off operation. Latest supplied interval: +1,214 counter increments / 1,319 seconds, approximately 3.95 MH/s share-equivalent work. All shares accepted at successive tester checks; latest explicit acceptance check at counter 942. Latest worker counter 1,400 is not a rejection-event export.
- Tester reports cold-to-touch operation. Internal temperatures and input/wall power were not measured. One-device observation, not a safety or compatibility certification.
- More lifecycle/reconnect/Stop/update tests, longer sessions and broader device coverage remain useful. Full candidate-alert/confirmation validation remains incomplete.

APK SHA-256: `fcc28ff97a62e363348078bc5b6f0186d2fc47b4930c822d5768862cc66812b3`.

See [field report](benchmarks/first-device.md), [optimization and rollback](MIDSTATE-EXPERIMENT.md), and [release checklist](RELEASING.md). Public CI runs tests and builds without uploading APK/AAB artifacts. Private signing credentials are never included in source exports.

---

## Historical validation (not the selected artifact)

# Validation summary for the supplied 0.2.1 test artifact

- Android debug build completed with JDK 17, Gradle Wrapper 8.9, SDK 35, NDK 27.0.12077973 and CMake 3.22.1.
- APK archive integrity and APK Signature Scheme v2 verified. It uses an Android Debug certificate, not a production signing setup.
- Offline tests cover fixed-width nonce encoding, SHA vectors/genesis, target math, merkle/header construction, strict mock submissions, multiple workers/duplicate checks, suggestion-versus-assigned difficulty and CPU detection/benchmarking.
- First tester reports share acceptance on a public pool after 0.2.1; see the separate field report. This is not a broad compatibility certification or proof of every lifecycle/alert path.
- Full Android candidate-alert simulation, durable block-candidate handling, longer controlled thermal/power tests and a wider device matrix remain open work.

The original environment-specific build logs are not included in Git history. Reproduce the tests/build from the documented commands rather than relying on a historical log dump. The source and version of a newly built artifact should always be checked independently of a previous test APK.
