# NerdPhone

<img src="branding/nerdphone-icon-512.png" alt="NerdPhone app icon" width="96" height="96">

**Turn an old Android phone into an educational Bitcoin solo miner.**

NerdPhone is a native **C++ SHA-256d miner** with **Stratum V1 networking** and an **English Kotlin/Jetpack Compose interface**, running on the phone you already own. You choose how many mining workers to run, you can inspect the jobs your pool sends, and you can see shares actually accepted by the pool instead of trusting a local counter.

> **This is lottery mining, for learning.** It is not an investment and not a realistic source of income. On the tested device the expected time to find a block is measured in millions of years. Mining uses electricity and wears the battery. There is no yield, no return and no financial advice here.

**In a hurry?** The same build is available as a [signed APK for €5](https://xdrufox.gumroad.com/l/nerdphone); building from source is free.

## What it does

- **Configurable multi-threaded mining:** from one to every logical CPU currently available to the app, plus a separate thread dedicated to pool networking.
- **Per-device CPU detection** and a three-second offline hash-rate test that submits nothing to the pool.
- **Real share statistics:** submitted, accepted, rejected and unknown outcomes, with total and per-worker hash rates.
- **Live Stratum diagnostics:** jobs received/assigned/started, job IDs, hashes on the current job, difficulty, `clean_jobs` and message age.
- **Screen-off operation:** foreground service, persistent notification and partial wake lock, subject to Android and OEM background restrictions.
- **Block candidate detection:** the full header hash is compared against the network target, the alert is shown and the share is submitted. This is not a Bitcoin block confirmation.
- **No application-level CPU cap:** the selected workers hash continuously while valid work exists. Android thermal and power protection still applies.
- **Midstate optimisation** in the hashing hot path: the first SHA-256 compression state for each 80-byte header is computed once per job and reused for every nonce, so each nonce costs two compressions instead of three.

## Screenshots

Real captures from the UMIDIGI Power 3 used for the field test, mining with eight workers. The Bitcoin address field has been redacted.

<p>
  <img src="docs/images/01-dashboard.png" alt="NerdPhone dashboard showing 4010.16 kH/s and eight mining workers" width="240">
  <img src="docs/images/02-cpu-threads.png" alt="Eight active miners with individual hash rates" width="240">
</p>

<p>
  <img src="docs/images/03-shares-jobs.png" alt="56952 accepted shares, 47 rejected, Stratum job diagnostics" width="240">
  <img src="docs/images/04-pool-config.png" alt="Pool configuration with TCP port 3333 and redacted BTC address" width="240">
</p>

- **Dashboard:** 4,010 kH/s local, status *Mining*.
- **Workers:** eight miners, from 462 to 528 kH/s each, plus the dedicated pool thread.
- **Shares:** 56,952 accepted of 56,999 submitted; the 47 rejections were `Stale`.
- **Connection:** `pool.nerdminers.org` on TCP 3333, difficulty 0.001, address redacted.

**Pool side, from the same session:** one worker, ~4 MH/s in the pool's own estimate, best share difficulty 74.563. Each major grid interval is three hours.

<img src="docs/images/pool-stats.png" alt="Pool dashboard showing about 4 MH/s and best difficulty 74.563" width="900">

> Pool-side estimates depend on share arrivals and smoothing windows. They are not a local benchmark.

## Measured on real hardware

Everything below was observed on one **repurposed UMIDIGI Power 3 running Android 10**, with **eight mining workers**, screen off, connected to `pool.nerdminers.org:3333` with pool-assigned difficulty **0.001**. It is a single device and a single operator's session: treat it as a field report, not a compatibility or performance certification.

| Observation | Result |
|---|---|
| Duration of the extended run | About **12 hours** screen off after installing the current build, with each pool graph division equal to **3 hours** |
| Pool-side hash rate over that run | Repeatedly near **4 MH/s** (`hashrate1hr` typically 3.9–4.0 MH/s in the supplied snapshots) |
| Shares | **56,952 accepted** of **56,999 submitted** in the captured session; counters carried over from 186 at the first snapshot, an interval-equivalent average near **4.0 MH/s** assuming one difficulty-0.001 share per increment |
| Rejections | **47**, all reported as `Stale` — about **0.08%**; no other rejection pattern observed |
| Best share | **74.5631** difficulty (raised from 40.51 earlier in the same session) |
| Perceived heating | **Cold to the touch** throughout; no fan or cooling accessory used |
| Internal temperature, power at the wall | **Not measured** |

The initial gap in the pool graph corresponds to a deliberate stop to install the update, not to a miner failure.

### The two hash-rate numbers, and why they differ

The app counts hashes computed on the device; the pool estimates work from the shares it receives. They answer different questions and will not match exactly.

| Number | Where it comes from | What it means |
|---|---|---|
| **~4,000 kH/s** in the app | Native worker counters, shown in kH/s | Local computation rate; short intervals can be quantised by counter batching |
| **~4 MH/s** on the pool | Pool estimate from accepted shares | Throughput the pool actually observed; smoothed and share-luck dependent |

Both are reported here so neither is mistaken for the other. A local counter alone does not prove valid submission, and a pool estimate alone does not prove the local loop is efficient.

### About 70 NerdMiner-class devices, in hash rate only

```text
4,000 kH/s ÷ 57 kH/s ≈ 70
```

The **57 kH/s** figure is the reference rate supplied by the tester; not every NerdMiner model or firmware version reaches it. Against a seller-declared **307 kH/s** device the same throughput would be roughly **13 devices**, assuming that advertised rate is actually achieved.

This is **arithmetic on hash rate**, nothing more: it is not a side-by-side hardware test and it says nothing about energy use, purchase cost or reliability. A phone running NerdPhone was also **cold to the touch with no fan or cooling accessory**, whereas some small miners are sold with fans that add their own consumption—but that too is an observation from one device, not a measured efficiency comparison. ASIC products with similar names belong to a different performance class entirely.

## Battery and heat

A separate unplugged run started at **90% at 10:13**, with eight workers and the screen off:

| Time | Battery | Elapsed |
|---|---|---|
| 10:13 | 90% | start |
| 10:32 | 87% | 19 min |
| ~10:46 | 85% | 33 min |
| 11:15 | 80% | 62 min |

That is about **10% of battery per hour**, roughly **9 hours 20 minutes projected from 90%**, while the pool continued to report around 4 MH/s. The phone was recharged after the measurement window ended, so later battery readings in the screenshots (67% at 14:35) belong to a different, post-charge session and are not part of these figures.

For an order-of-magnitude power estimate, assuming the nominal **5,000 mAh at 3.8 V (≈19 Wh)**:

```text
10% of 19 Wh = 1.9 Wh over 1.03 h  →  ≈1.8 W average for the whole phone
```

**Caveats, stated plainly:** the battery is a used cell, so its real capacity is likely below 5,000 mAh and this figure is **most likely an overestimate**; the percentage gauge is coarse; and this is the total phone consumption, not the miner alone and not a wall-plug measurement. The honest measured facts are the percentage drop and the reported lack of noticeable heating—not a certified wattage.

## Lottery mining, honestly

`pool.nerdminers.org` is a **solo** pool: every worker mines to its own address, so a block found by that worker pays the whole reward to that worker. It is not split among the pool's participants.

- Subsidy since the April 2024 halving: **3.125 BTC**, plus the transaction fees in the block.
- The pool dashboard showed **0 blocks found** with about **7.5 GH/s** of combined hobbyist hashrate.
- At the network difficulty current in November 2026, the expected time for a single ~4 MH/s phone is on the order of **billions of years**, and for the whole pool millions of years.

So: someone eventually wins the SuperEnalotto with a single ticket, and small solo miners have found blocks before. That is exactly the spirit here—a ticket that also teaches you how Bitcoin's proof of work, nonces, extranonce and share difficulty actually work. Do not buy this expecting to win.

## Install and start

**The source is free: build it yourself at no cost** ([build instructions](docs/BUILDING.md)).

> ### Ready-to-install signed APK — €5
> **[xdrufox.gumroad.com/l/nerdphone](https://xdrufox.gumroad.com/l/nerdphone)**
>
> One ZIP: the signed APK, the installation guide, the SHA-256 checksums and the licence. Optional — the source below stays free and MIT, and the purchase restricts none of your rights under that licence.

This repository and its CI **do not distribute APK/AAB artefacts**.

1. Install on **Android 8.0 / API 26 or newer** and allow notifications.
2. Enter your **public mainnet Bitcoin address**. Never a seed phrase, never a private key.
3. Default connection:

   | Setting | Default |
   |---|---|
   | Pool host | `pool.nerdminers.org` |
   | TCP port | `3333` |
   | Pool password | `x` |
   | Requested difficulty | `0.001` |

4. In **CPU DETECTION & MINING THREADS**, start with one or two miners. You can run the offline three-second test before connecting, and change the worker count with **+ / − / Use all** while stopped.
5. Tap **START MINING** and watch **ACCEPTED**, not the hash-rate animation.
6. For screen-off use, allow unrestricted battery/background operation in Android settings, and test on your own device before leaving it unattended.

`pool.nerdminers.org` is **not** `pool.nerdminer.io`; the two endpoints can assign different difficulties. The pool decides the assigned difficulty—requesting `0.001` does not force it.

## How to interpret the numbers

- **Local hash rate:** hashes computed by the native workers. Short intervals can be quantised by counter batching.
- **Pool hash rate:** an estimate derived from submitted work. It fluctuates with share luck and smoothing windows.
- **Jobs received:** protocol activity only—not proof that submissions are correct.
- **Accepted shares:** the pool accepted that work. They are not block confirmations and, on a solo pool, not periodic payments.
- **Best share:** the difficulty achieved by a share. Not the assigned difficulty, not the network difficulty, and not progress towards a block.

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

Workers search **disjoint nonce residue classes** for each job and `extranonce2`, so they never repeat each other's work. Threads are scheduled by Android and are not pinned to physical cores. The portable SHA implementation reuses the per-header midstate and fixed-size buffers in the nonce loop; it does **not** use ARM SHA instructions.

Correctness is checked offline by **20,480 header/nonce equivalence cases**, known-answer tests, an independent mock pool that validates submitted shares, and strict eight-hex-digit nonce formatting.

## Build and test

Requirements: **JDK 17**, Android SDK **35**, NDK **27.0.12077973**, CMake **3.22.1**. The Gradle Wrapper **8.9** is included with its distribution checksum.

```sh
./gradlew :app:assembleDebug
# APK: app/build/outputs/apk/debug/app-debug.apk
```

Offline host tests need Python 3 and a C++17 compiler:

```sh
bash tests/run_host_tests.sh
```

GitHub Actions runs the offline tests and compiles a debug APK only to verify that the build works. It uploads no binaries, publishes no releases and does not touch the production signing key. See [build instructions](docs/BUILDING.md), [signing procedure](docs/DISTRIBUTION-SIGNING.md) and the [release checklist](docs/RELEASING.md).

## Known limitations

- Plaintext TCP **Stratum V1** only: no TLS, no Stratum V2.
- No guarantee of uninterrupted background operation; Android and OEM power management can throttle or stop the service.
- No automatic restart after reboot or force-stop.
- No persisted block-candidate dossier and no automatic blockchain confirmation.
- Performance and stability vary by device; validation so far covers a single physical device.
- Battery behaviour depends on the age and health of the cell.
- No ARM SHA intrinsic acceleration; the heavy path is portable C++.

## Documentation and contributing

- [Technical guide](docs/TECHNICAL.md)
- [Build instructions](docs/BUILDING.md)
- [Field report](docs/benchmarks/first-device.md)
- [Roadmap](docs/ROADMAP.md)
- [Contributing](CONTRIBUTING.md) · [Security and safety](SECURITY.md)

When reporting a problem include the app version, device model, Android version, worker count and a sanitized error message. Never post seeds, private keys, signing keys or pool passwords. Bitcoin addresses are public but can link mining activity to your identity—redact them if you prefer.

## Attribution and licence

Inspired by and written against the Stratum flow of [NerdMiner_v2](https://github.com/BitMaker-hub/NerdMiner_v2), pinned at `e3a04b7d9f0b6e60ca6e02f7433c94d78d9dded4` (`V1.8.3`). NerdPhone is an independent Android implementation, not the original firmware and not a byte-for-byte copy. Not affiliated with NerdMiner, its developers or any pool operator.

**MIT licence**, with Bitmaker attribution preserved for reference material. Vendored nlohmann/json is MIT; the Gradle Wrapper is Apache-2.0. See [LICENSE](LICENSE) and the third-party notices in [reference/](reference/).
