# Technical guide

An experimental Android Bitcoin solo miner: **C++17 SHA-256d core, Stratum V1 networking, Kotlin/Jetpack Compose UI**.

**Not affiliated with or endorsed by NerdMiner or any pool operator. Educational software; not a realistic source of mining income.**

## Features

- Configurable mining workers (1 to the detected available logical CPUs) plus one dedicated pool-socket thread. Android UI/runtime threads are separate.
- Real hashing and real Stratum share submissions; no simulated dashboard data.
- Hash rate; submitted, accepted, rejected and unknown-outcome shares.
- Job monitor: received/assigned/started counts, assigned and working job IDs, hashes and target difficulty for the working job, `clean_jobs`, message age.
- Requested and pool-assigned difficulty shown separately, plus suggestion RPC diagnostics and the exact subscribe user agent.
- Foreground service, persistent Stop action and partial wake lock for screen-off operation.
- No application CPU cap, thermal pause or intentional delay in the hashing loop. Select the number of miners explicitly; Android schedules them without affinity pinning.
- Local network-target detection and **block candidate** notification. Share acceptance is never labelled as block confirmation.
- English interface, notifications and application-generated status/error messages.

## Quick start

1. Build or install the debug APK on Android **8.0/API 26 or newer**.
2. Allow notifications so block candidate alerts can be displayed.
3. Enter your **public Bitcoin address**. Never enter a seed phrase or private key.
4. Default pool: **`pool.nerdminers.org:3333`**, pool password `x`.
5. Start mining. Adjust Android's battery settings to allow unrestricted background operation if you want to test screen-off mining.

Existing saved pool settings are preserved across upgrades. Pool requirements differ: Use a valid mainnet BTC payout address and follow the selected pool’s worker-name rules. Do not assume all pools support worker-name suffixes.

Android/OEM policies may still suspend networking, terminate the app or throttle the CPU. A wake lock does not bypass OS protections. The service does not automatically restart after force-stop, reboot or process termination.

## CPU detection and multi-core mining (0.2.0)

Each activity launch probes this device with native `sched_getaffinity`, `sysconf` configured/online CPU counts and a fallback to available CPU reporting. The picker uses CPUs available to the app, not a hardcoded phone model. Detection is repeated at actual service startup and clamps stale settings to the current available count. Affinity/online availability may change with Android cpusets, power state and foreground/background policy; these counts are observations, not a guarantee of simultaneous execution.

- Default remains **1 miner**. Stop mining before changing the count.
- Use **+ / − / Use all**, then start; the setting is saved on this installation only.
- **Detect CPUs again** refreshes the counts on the current device.
- **Run 3-second offline test** hashes a synthetic 80-byte header using the selected count, with per-worker and total rates. It never contacts a pool or submits shares. Keep the activity open. A benchmark and pool mining cannot start concurrently.
- This is a quick functional throughput test, **not a sustained thermal test** or a benchmark of each physical CPU individually.
- During mining, each worker's real hash rate appears separately. Jobs-started remains a count of distinct job generations begun by at least one worker, not multiplied by worker count.

For each job/extranonce2, worker `i` hashes `i, i+N, i+2N, ...` using 64-bit loop control until the 32-bit nonce range is exhausted. All workers then advance their own extranonce2 counter; fixed residue classes keep their searches disjoint even if they advance at different rates. The pool worker remains separate. Nonce ranges are reset on a new job exactly as before.

More threads can increase heat and cause OS throttling. Compare 1, 2, 4 and all available workers over at least 20 minutes; do not extrapolate sustained performance from the 3-second test. Defaulting to one worker is a configuration choice, not a throttle in the selected workers' hashing loops.

## Difficulty: 0.001 is a request, not a promise

The default host is now `pool.nerdminers.org`, with an **s** and `.org`. It is a different service from `pool.nerdminer.io`. The tester reports an assigned difficulty of 0.001 on the new default; subscription probes also observed it. Prior `.io` research below is historical, not a statement about `.org`. Existing saved hosts are preserved, so verify your host field after updating.

The app sends:

```json
{"id":1,"method":"mining.subscribe","params":["NerdMinerV2/V1.8.3"]}
{"id":2,"method":"mining.authorize","params":["YOUR_PUBLIC_BTC_ADDRESS","x"]}
{"id":3,"method":"mining.suggest_difficulty","params":[0.001]}
```

The subscribe user agent already matches the referenced NerdMiner firmware; this is a documented compatibility identifier, **not a claim that this app is ESP32 firmware**. There is no authentication magic in that string.

**Only inbound `mining.set_difficulty` assigns the session target.** A successful suggestion RPC reply does not prove that 0.001 was assigned. Mining keeps the target attached to the relevant job; a new difficulty applies to subsequent jobs. The initial Stratum difficulty is 1 until announced otherwise.

Short live probes of NerdMiner.io with the original user agent, a fresh test-only address and zero submitted shares continued to receive difficulty 1, both with password `x` and an experimental `d=0.001` password hint. That hint was not shown to work and is **not the default**. These short observations are not proof of a permanent pool minimum or of its backend software. See [research notes](DIFFICULTY-RESEARCH.md).

Do not send low-difficulty shares pretending they meet a higher target. For a reliable low-difficulty test, use a pool that actually assigns it, ask the operator for support, or operate a properly configured private pool.

At 300 kH/s, difficulty 1 means about 3 h 59 min per share on average; 0.001 means about 14.3 s. This is probabilistic, not a timer. An accepted share remains the end-to-end test.

## Build

Requirements:

- JDK **17**
- Android SDK platform **35**, Build Tools **35.0.0**
- NDK **27.0.12077973**
- CMake **3.22.1**
- Internet access for dependencies

Open this folder in Android Studio, install the required SDK components and sync, or run:

```sh
# Set ANDROID_HOME to your SDK, or create local.properties with sdk.dir.
./gradlew :app:assembleDebug
# Windows: gradlew.bat :app:assembleDebug
```

APK: `app/build/outputs/apk/debug/app-debug.apk`.

The debug APK is signed with a local development key. Different computers/CI runs may generate different debug keys; updates need the same signing key. Release signing is deliberately not configured. Never commit a signing key.

ABIs: `arm64-v8a`, `armeabi-v7a`, `x86_64`. Legacy compressed/extracted JNI packaging is enabled for compatibility. `minSdk=26`, `targetSdk=35`. The foreground service uses the `specialUse` category, not an unrestricted guarantee of store eligibility.

The included Gradle memory limits are conservative for a small build environment. Increase them on your own machine if compilation runs out of heap/metaspace.

## Host tests

Requires Python 3 and a C++17 compiler. No Python packages are required.

```sh
mkdir -p /tmp/nerdphone-tests
g++ -std=c++17 -O2 -pthread tests/core_test.cpp -o /tmp/nerdphone-tests/core
/tmp/nerdphone-tests/core
python3 tests/mock_pool.py
python3 tests/negotiation_test.py
NERDPHONE_TEST_THREADS=2 python3 tests/mock_pool.py
g++ -std=c++17 -O2 -pthread tests/cpu_test.cpp -o /tmp/nerdphone-tests/cpu
/tmp/nerdphone-tests/cpu
```

The mock validates submitted header hashes independently with Python `hashlib`, checks handshake/request fields and accepted/rejected counts, and exercises clean/non-clean jobs and job telemetry. It does not contact public pools.

Optional live diagnostics, explicitly run by the developer:

```sh
python3 tests/probe_pool.py
```

This short probe uses OpenSSL to generate an ephemeral test-only address, connects to the current default NerdMiners.org pool and requests 0.001. **It submits no shares and never uses your wallet.** The key is not printed or saved, so do not send funds to the test address. The script is not run by CI.

## Repository / GitHub

This folder is the repository root. It includes Gradle Wrapper, a GitHub Actions build/test workflow, MIT license, upstream notices and contribution/security notes. SDKs, generated files, APKs, credentials and signing keys are excluded by `.gitignore`.

```sh
git init
git add .
git commit -m "Initial NerdPhone release"
# Add your own remote, then push.
```

When using the supplied source ZIP, extract it first and run these commands **inside `NerdPhone/`**. No remote repository has been created or pushed automatically.

## Architecture and upstream reference

- `app/src/main/cpp/engine.cpp`: pool and mining workers, JNI, queue and snapshot telemetry.
- `app/src/main/cpp/hash.hpp`: portable SHA-256d, full-width target comparison and exact integer target division for the binary64 Stratum difficulty.
- `app/src/main/java/it/nerdphone/`: Compose activity, JNI wrapper and foreground service.
- `reference/`: preserved upstream excerpts and notices, not compiled into the Android application.

Reference NerdMiner revision: `e3a04b7d9f0b6e60ca6e02f7433c94d78d9dded4`, reporting `V1.8.3`, from https://github.com/BitMaker-hub/NerdMiner_v2.

This is a port of the relevant Stratum flow, not a byte-for-byte copy of the complete firmware. It verifies authorization, supports negotiated extranonce2 sizes 1–32 bytes, increments extranonce2 after nonce exhaustion, parses complete newline-delimited JSON, and tracks share IDs. As of 0.2.1, every submitted nonce is exactly eight lowercase hexadecimal characters with leading zeroes preserved. On a new job the miner switches to the newest template. `clean_jobs=true` invalidates queued old shares; with `false`, still-valid queued shares can be submitted. Reconnection clears current work.

TCP only: **no TLS or Stratum V2**. The native code reports connection phase, numeric endpoint where available, errno/SO_ERROR and RPC diagnostics. Socket keepalive and `TCP_NODELAY` are enabled. Reconnection uses the configured host, including on `client.reconnect`; arbitrary server-directed endpoints are not followed.

Bounded outgoing queue and pending-share limits protect memory; these are not CPU caps. Hash counters are published in batches of 4,096, so short UI intervals can be quantized. Current counters are session-local; the last observed block candidate is persisted by the Android service. Candidate detection does not query the blockchain to confirm a block.

The portable SHA implementation now includes HeaderHasher: a cached first-block midstate and two fixed-size compression blocks per nonce for 80-byte headers. It is rebuilt whenever the header/extranonce2 changes. Generic sha/sha2 remain unchanged for merkle/coinbase work and correctness comparisons. ARM SHA intrinsics are not implemented. Performance is device-dependent. See [midstate notes](MIDSTATE-EXPERIMENT.md).

## Validation and limitations

Version 0.2.1 fixes a nonce wire-format bug that caused strict pools to reject short nonces with “No nonce”. Earlier mock tests validated the integer value but were too permissive about its encoded length. The mock now enforces exactly eight hex characters and also uses a nonzero previous hash, nonempty merkle branches and the pool-like eight-byte extranonce2. Host tests cover one and multiple workers, CPU discovery and regression vectors; the APK build/signature are verified. The first tester subsequently reported accepted shares on the selected public pool after this fix; see [the field report](benchmarks/first-device.md). This is a user report, not broad independent certification. See [incident notes](NONCE-FORMAT-FIX.md). Previous builds were reported working by the tester at roughly 290–300 kH/s; that is an informal device report, not an independent benchmark or broad compatibility certification. The new UI needs device testing. A generic Android 10 connection failure was not established to be OS-specific and should be diagnosed from the detailed error.

Monitor battery health and temperature. Stop if the battery swells or the device overheats. No profitability claims; no guarantee of a block, payout, pool availability, background uptime or store acceptance.

## License

MIT for this project and preserved NerdMiner reference material, with Bitmaker attribution. Vendored nlohmann/json 3.11.3 is MIT. Gradle Wrapper is Apache-2.0. See `LICENSE` and `../reference/*LICENSE*`. No affiliation or endorsement is implied.
