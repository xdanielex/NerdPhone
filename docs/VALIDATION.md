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

## Extended field run (same device)

The same phone was then left running continuously for several days, still with eight workers and the screen off. Figures below are read from the pool's own worker counters, not from the app.

- Cumulative shares reported by the pool: **262,371** at the time of writing, which at difficulty 0.001 is **≈1.1 × 10¹² hashes**.
- Share rate over the full run: **≈1.00 share/s**. At difficulty 0.001 one share per second is 0.001 × 2³² hashes, i.e. **4.295 MH/s** — consistent with the pool's 1-hour estimate (4.02–4.10 MH/s) and with the app's own reading.
- Reject rate in the longest measured window: **106 rejected of 150,281 submitted (0.066%)**, all reported as `Stale`. The rate is quoted with its window on purpose: over longer periods a single unstable Wi-Fi evening moves it.
- Best share observed: **difficulty 94.03**. At the pool's difficulty this is a legitimate but unremarkable draw: after 262,371 shares the expected count of shares at or above 94.03 is **2.8**, and one was observed. (The expected count rises with the share count, so it is quoted against the same number of shares.)
- The phone stayed at ~4 MH/s for days without thermal shutdown, CPU caps or pause logic. No cooling accessory was used.

## Protocol compatibility on a standard pool

The same build was pointed at a **standard ckpool-based solo pool** (`stratum.bitaxe.de:3333`, 0% fee, Frankfurt), which assigns the miner **difficulty 1** rather than the NerdMiner-specific 0.001:

- **Six shares accepted**, connection stable, no rejections observed in the session.
- The pool reported **4.32 MH/s** for the worker, an independent measurement that agrees with the app and with the other pool.

What this establishes: the Stratum v1 implementation works against an unmodified ckpool-style solo pool, not only against the NerdMiners fork that was patched for ultra-low difficulty. Share **counts** are not comparable across the two pools, and neither should be read as throughput: difficulty differs by a factor of 1,000.

## Power-source behaviour (observation, not a controlled measurement)

Running on battery produced a lower daily average than running on the charger, and the gap closed after the charger was reconnected — visible only because both windows come from the same pool counters:

- `hashrate1d` while on battery: **3.34 MH/s**, with `hashrate1hr` at **4.03–4.10 MH/s**.
- After roughly half a day on the charger, the 1-day average rose **3.34 → 3.81 MH/s** across consecutive readings, while the 1-hour figure stayed flat.

The most likely explanation is Android's battery saver throttling the process once the device is unplugged, i.e. **a 15–20% loss of hash rate on battery**. This has not been isolated: no `dumpsys battery` capture, no die-temperature reading and no power meter were used, so the cause is inferred from the shape of the curve rather than measured directly.

## Deployment note

`pool.nerdminers.org` refuses a second worker from the same public IP. Two phones on one home network will therefore not both connect to that pool by default. This is a pool behaviour, not an app limitation.

## Still open

Additional devices with different SoCs and Android versions, reconnect and job-transition coverage, Stop/update lifecycle tests, internal temperature and wall-plug power measurement, longer runs with battery-saver explicitly disabled, and a full block-candidate alert and submission test.

See the [field report](benchmarks/first-device.md) and the [release checklist](RELEASING.md). Public CI runs the tests and compiles a debug APK without uploading APK/AAB artefacts. Private signing credentials are never part of the source tree.
