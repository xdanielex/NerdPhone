# Building and testing

## Android toolchain

- JDK 17
- Android SDK platform 35 and Build Tools 35.0.0
- Android NDK 27.0.12077973
- CMake 3.22.1
- Gradle 8.9 through the included Wrapper (distribution checksum pinned)

Open the repository root in Android Studio and install the SDK components, or configure `ANDROID_HOME`/an untracked `local.properties` and run:

```sh
./gradlew --no-daemon :app:assembleDebug
# Windows: gradlew.bat --no-daemon :app:assembleDebug
```

Result: `app/build/outputs/apk/debug/app-debug.apk`.

The APK supports arm64-v8a, armeabi-v7a and x86_64; minimum Android API 26, target API 35. Compressed/extracted JNI packaging is enabled. No Google account, wallet secret or pool credentials are required to compile. The first build needs network access for dependencies.

The supplied JVM settings target a small environment. If compilation runs out of memory, increase `org.gradle.jvmargs` appropriately for your machine. Host compilation/testing and Android APK build success do not imply a successful device installation or universal runtime compatibility.

## Host tests

```sh
bash tests/run_host_tests.sh
```

Requirements: bash, Python 3, a C++17 compiler and pthread support. Set `CXX` to override g++ if needed. Tests cover known SHA vectors/genesis, target arithmetic, header/merkle encoding, fixed-width nonce formatting, CPU detection, three-second offline benchmarking, independent strict mock share validation, multiple workers and difficulty negotiation. The mock uses loopback only. No live public pool is contacted.

An optional live `tests/probe_pool.py` script uses Python 3 and OpenSSL. It creates an ephemeral test-only address, briefly subscribes/authorizes and submits **no shares**. It is never run by CI. Do not send funds to its generated address; its key is not stored.

## Android logs

For your own connected test device:

```sh
adb logcat -c
# Reproduce the problem.
adb logcat -d > nerdphone-logcat.txt
```

Review/redact the log before sharing it. Include the app version, model, Android version, worker count and the exact on-screen error. Do not assume all connection failures are Android-version bugs.

## Signing

Debug builds use a development keystore. Updates require the same package name/signing identity and a valid version progression. CI may generate a different debug key on a fresh runner. Distribution signing is intentionally separate from Gradle/CI; see [the signing procedure](DISTRIBUTION-SIGNING.md). The private distribution identity used for release APKs is not in this repository and must be backed up by the maintainer. Never commit a key or paste a private key/token into an issue or chat.
