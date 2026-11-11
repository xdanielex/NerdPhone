# NerdPhone

<img src="branding/nerdphone-icon-512.png" alt="NerdPhone app icon" width="96" height="96">

**Give an old Android phone a second life as an educational Bitcoin solo miner.**

NerdPhone combines a native **C++ SHA-256d miner**, **Stratum V1 networking** and an **English Kotlin/Jetpack Compose interface**. Select how many mining threads to run, inspect the jobs received from your pool, and verify actual accepted shares—not just a local hash-rate counter.

> **Experimental, educational software.** Not a wallet, not a realistic source of income, and not affiliated with NerdMiner, its developers or any pool operator. Mining can heat your phone, consume electricity and wear its battery.

## What it does

- **Configurable multi-threaded mining:** one to the logical CPUs currently available to the app, with a separate pool-networking thread.
- **Per-device CPU detection** and a three-second offline hash-rate test.
- **Real share statistics:** submitted, accepted, rejected and unknown outcomes; total and per-miner hash rates.
- **Live Stratum diagnostics:** received/assigned/started jobs, job IDs, working-job hashes, difficulty, `clean_jobs` and message age.
- **Screen-off support:** foreground service, persistent Stop notification and partial wake lock, subject to Android/OEM restrictions.
- **Block candidate detection:** compare the full hash to the network target, display an alert and submit to the pool. This is not automatic confirmation of a Bitcoin block.
- **No application CPU throttle:** selected workers hash continuously while valid work is available. Android thermal and power protections still apply.

## Current status

**The midstate-optimized build, `0.2.4-midstate-test`, is now the preferred build for continued testing following a successful first-device screen-off trial.** It is a signed, non-debuggable release APK—not a debug build. Its existing version name and binary are unchanged; this documentation update does not create a new release or imply broad compatibility certification.

### First optimized-build field test

Tested by the project maintainer on a **repurposed UMIDIGI Power 3 running Android 10**, using **eight mining workers**, connected to `pool.nerdminers.org:3333` at reported assigned difficulty **0.001**:

| Observation | Reported result |
|---|---|
| Screen state | Screen off throughout the documented optimized trial |
| Interval between first and latest supplied snapshots | **21 minutes 59 seconds** |
| Worker share counter | **186 → 1,400**, an increase of **1,214** |
| Share-equivalent average over that interval | **Approximately 3.95 MH/s**, assuming one difficulty-0.001 share per counter increment |
| Latest five-minute pool estimate | **3.86 MH/s** |
| Highest supplied one-minute pool estimate | **4.67 MH/s**, a short-window estimate, not sustained throughput |
| Acceptance | Tester reported all shares accepted at successive checks; latest explicit all-accepted check was at counter **942** |
| Perceived heating | Tester reported the phone remained **cold to the touch**, with no noticeable heating |

The approximately **3.95 MH/s** interval estimate is about **2.2×** the approximately **1.78 MH/s** inferred from the documented earlier, unoptimized session on the same phone. These were separate field sessions, not a controlled simultaneous A/B test. The earlier approximately two-hour screen-off report belongs to the older engine and must not be attributed to the optimized build.

**Internal/battery temperatures and power consumption were not measured.** This successful first-device trial supports keeping the optimized build as the reference for further testing, not claims of universal performance, certified safety, measured energy efficiency or long-term stability. See the [field report](docs/benchmarks/first-device.md) and [optimization/rollback notes](docs/MIDSTATE-EXPERIMENT.md).

### Around 69 × 57 kH/s devices on the observed interval

At approximately **3.95 MH/s**, the optimized phone's share-equivalent throughput corresponds to about **69 devices each hashing at 57 kH/s**. The short one-minute pool estimate of **4.67 MH/s** corresponds to about **82**, but is not the sustained average.

```text
3,950 kH/s ÷ 57 kH/s ≈ 69
4,670 kH/s ÷ 57 kH/s ≈ 82 (short-window pool estimate only)
```

The **57 kH/s baseline was supplied by the tester**; not all NerdMiner models or firmware versions run at that speed. Against a seller-declared **307 kH/s** device, the same 3.95 MH/s interval is approximately **13 devices**, assuming that advertised rate is achieved. These are arithmetic comparisons, not side-by-side hardware tests, and **do not compare energy use, purchase cost or reliability**. ASIC-based products marketed with similar names are a different performance class.

The source remains free to build. The optimized implementation caches the first SHA-256 compression state for each header and uses fixed-size buffers in the nonce loop; networking, targets, worker partitioning and screen-off service behavior are unchanged. Generic hashing remains as a correctness reference. Offline verification includes **20,480 header/nonce equivalence cases**, known-answer tests and independent simulated-pool share validation. APK compilation and signature verification passed. More devices and longer-duration tests are welcome.

## Install and start

**The source code is free and open source.** [Build it yourself at no cost](docs/BUILDING.md), or support development by purchasing the official ready-to-install APK when it becomes available. The planned price is €5; the purchase link will be added when the download is published.

This repository and its CI do not distribute APK/AAB artifacts. Source tags and release notes can remain public. Purchasing a prebuilt package is optional and does not restrict the rights granted by the source license.

1. Install on **Android 8.0/API 26 or newer** and allow notifications.
2. Enter your **public mainnet Bitcoin address**. Never enter a seed phrase or private key.
3. Check the default connection:

   | Setting | Default |
   |---|---|
   | Pool host | `pool.nerdminers.org` |
   | TCP port | `3333` |
   | Pool password | `x` |
   | Requested difficulty | `0.001` |

4. In **CPU DETECTION & MINING THREADS**, start with one or two miners. Optionally run the offline test before connecting. Use **+ / − / Use all** to change the count while stopped.
5. Tap **START MINING**. Watch the working-job hash count and, most importantly, **ACCEPTED**.
6. For screen-off use, allow unrestricted battery/background operation in Android settings. Test on your device before leaving it unattended.

`pool.nerdminers.org` is **not** `pool.nerdminer.io`: the endpoints can assign different difficulty values. Existing saved settings survive upgrades. Verify your host after updating.

The pool controls the assigned difficulty. Requesting `0.001` does not force it. An RPC acknowledgement alone is not a target assignment. See [difficulty research](docs/DIFFICULTY-RESEARCH.md).

## How to interpret the numbers

- **Local hash rate:** hashes computed by the native workers; short intervals can be quantized by counter batching.
- **Pool hash rate:** an estimate based on submitted work. It fluctuates with share luck and smoothing windows.
- **Jobs received:** protocol activity, not proof that share submissions are correct.
- **Accepted shares:** evidence that the pool accepted that submitted work. They are not Bitcoin block confirmations or periodic payments on a solo pool.
- **Best share:** difficulty achieved by a share, not the assigned share difficulty or the Bitcoin network difficulty.

At a sustained **1.5 MH/s**, expected time to find a share is about **2.9 seconds at difficulty 0.001**, or **47.7 minutes at difficulty 1**. These are statistical averages, not deadlines; acceptance also requires valid, timely work.

## Architecture

```text
Compose UI / Android main looper
           │ JNI configuration + telemetry
           ▼
   C++ mining workers × N ──► thread-safe share queue
           ▲                           │
           │ immutable jobs             ▼
           └──────── dedicated C++ Stratum worker ── TCP pool

Foreground service + partial wake lock manage background operation.
```

Workers search disjoint nonce residue classes for each job/extranonce2, rather than repeating each other's work. CPU threads are scheduled by Android, not pinned to physical cores. The current portable SHA implementation reuses the first-block midstate for 80-byte headers and uses fixed-size buffers in the nonce loop; it does not use ARM SHA instructions.

## Build and test

Requirements: **JDK 17**, Android SDK **35**, NDK **27.0.12077973**, CMake **3.22.1**. Gradle Wrapper **8.9** is included with its distribution checksum.

```sh
./gradlew :app:assembleDebug
# APK: app/build/outputs/apk/debug/app-debug.apk
```

Offline host tests require Python 3 and a C++17 compiler:

```sh
bash tests/run_host_tests.sh
```

GitHub Actions runs the offline tests and compiles a debug APK solely to verify the build. It does not upload binary artifacts or publish APKs in releases; generated APK/AAB output directories are removed in an always-run cleanup step. It does not manage a production signing key. See [build instructions](docs/BUILDING.md) and the [release checklist](docs/RELEASING.md).

## Known limitations

- Plaintext TCP Stratum V1 only; **no TLS or Stratum V2**.
- No guarantees of continuous background operation, temperature, battery life or store eligibility.
- No automatic restart after reboot/force-stop or process termination.
- No complete persistent block-candidate dossier or automatic blockchain confirmation. See [technical limitations](docs/TECHNICAL.md).
- Only initial real-device validation; performance and compatibility vary.
- The private distribution key must be backed up by the maintainer; the security contact and customer-facing source/download URLs still need configuration before public sale.

Version 0.2.1 fixed a **nonce-format bug**: strict pools rejected short hexadecimal nonces as “No nonce.” Submissions now always use eight hex digits, and the mock enforces that requirement. See the [incident notes](docs/NONCE-FORMAT-FIX.md) and [changelog](CHANGELOG.md).

## Documentation and contributing

- [Technical guide](docs/TECHNICAL.md)
- [Build instructions](docs/BUILDING.md)
- [Private distribution signing procedure](docs/DISTRIBUTION-SIGNING.md)
- [Field report and benchmark methodology](docs/benchmarks/first-device.md)
- [Screenshot and photo guide](docs/SCREENSHOTS.md)
- [Roadmap](docs/ROADMAP.md)
- [Contributing](CONTRIBUTING.md) · [Security and safety](SECURITY.md)

When reporting a problem, include app version, device model, Android version, worker count and a sanitized error. Do not post private keys, seeds, signing keys or sensitive passwords. Bitcoin addresses are public but can link mining activity to your identity; redact them if desired.

## Attribution and license

Inspired by and adapted against the Stratum flow of [NerdMiner_v2](https://github.com/BitMaker-hub/NerdMiner_v2), pinned at `e3a04b7d9f0b6e60ca6e02f7433c94d78d9dded4` (`V1.8.3`). This is an independent Android port, not the original firmware or a byte-for-byte copy of it.

**MIT license**, with Bitmaker attribution preserved for reference material. Vendored nlohmann/json is MIT; Gradle Wrapper is Apache-2.0. See [LICENSE](LICENSE) and [third-party notices](reference/).
