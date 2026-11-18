# Validation summary

Artifact: **`0.3.2`**, versionCode **19**, package `it.nerdphone`, min API 26, target API 35. Release
build, `debuggable` disabled, distribution-signed (RSA 3072, certificate
`43bb0e9f...be2962`, the same key as `0.3.1`, so this installs as an update).

All measurements in this document were taken on **versionCode 18 (`0.3.1`)**. Build 19 differs
from it by one line: the sample counter on the THERMAL card is incremented only by the service,
not by the activity, so `N samples` now means "N samples taken every 30 s during mining" instead
of a number inflated about fivefold while the screen was on. The peak temperature is unaffected
either way, because it is a maximum over all samples from both sources. Nothing in the hashing
path changed.

- JDK 17, Gradle 8.9, Android SDK 35, NDK 27.0.12077973, CMake 3.22.1.
- Build, lint and signature verification passed. Three ABIs: `arm64-v8a`, `armeabi-v7a`, `x86_64`.
- Host tests passed: SHA-256 vectors and genesis block, target arithmetic, merkle/header construction, **20,480 optimised-versus-reference header comparisons**, eight-hex-digit nonce formatting, CPU detection and worker partitioning, duplicate-share checking, difficulty negotiation, and an independent mock pool that validates submitted shares.
- Field test: one UMIDIGI Power 3 on Android 10 with eight workers, screen off, pool-assigned difficulty 0.001. Pool estimate stayed near **4 MH/s** for about **12 hours**; share-equivalent averages from the counters were **4.00–4.08 MH/s**.
- Acceptance in the captured session: **56,952 accepted of 56,999 submitted**, **47 rejected** and all reported as `Stale` — about **0.08%**.
- Battery: about **10% per hour** with the screen off; ~1.8 W estimated for the whole phone under the stated nominal-capacity assumption. Not a wall-plug measurement.
- Tester reports the phone remained cold to the touch with no cooling accessory. Internal and battery temperatures were **not measured**.
- One device, one operator, one pool. Not a safety, energy-efficiency or compatibility certification.

APK SHA-256: `1d86ec3db9222c3865ede2ded1a082b198dfa5822fd58fb6469a109a2657ec69`
Distribution certificate SHA-256: `43bb0e9fd881b7bbe8f0739a03774e51319a94992e72efbc3e6dbaec69be2962`

## Extended field run (same device)

The same phone was then left running continuously for several days, still with eight workers and the screen off. Figures below are read from the pool's own worker counters, not from the app.

- Cumulative shares reported by the pool: **262,371** at the time of writing, which at difficulty 0.001 is **≈1.1 × 10¹² hashes**.
- Share rate over the full run: **≈1.00 share/s**. At difficulty 0.001 one share per second is 0.001 × 2³² hashes, i.e. **4.295 MH/s** — consistent with the pool's 1-hour estimate (4.02–4.10 MH/s) and with the app's own reading.
- Reject rate in the longest measured window: **106 rejected of 150,281 submitted (0.07%)**, all reported as `Stale`. The rate is quoted with its window on purpose: over longer periods a single unstable Wi-Fi evening moves it.
- Best share observed: **difficulty 94.03**. At the pool's difficulty this is a legitimate but unremarkable draw: after 262,371 shares the expected count of shares at or above 94.03 is **2.8**, and one was observed. (The expected count rises with the share count, so it is quoted against the same number of shares.)
- The phone stayed at ~4 MH/s for days without thermal shutdown, CPU caps or pause logic. No cooling accessory was used.

## Thermal behaviour with the hardware path (2026-11-17)

Eight workers, hardware path active, wired connection, ambient temperature 21 °C, running
continuously for about an hour and a quarter:

| Reading | Value |
|---|---|
| Battery temperature at the end of the run | **22.7 °C** |
| Battery temperature peak for the session | **25.9 °C** |
| Rise above ambient | **+4.9 °C** |
| Thermal throttling observed | none |

The throttling check does not rely on the thermometer. A phone that was thermally throttling
would show `hashrate1m` systematically below `hashrate5m`, and `hashrate1hr` flat or falling.
Over the run the pool reported the opposite order every time it was sampled, with all three
fields climbing (for example 9.14 / 8.75 / 6.94 at 21:30 CET).

Caveats: the battery is a slow, indirect thermometer; on this device the SoC thermal zone is
not exposed to the app; and one run in a 21 °C room is not a hot-climate test.

## Share quality with the hardware path

Over the full run the app counted **11 rejected shares out of 13,000 submitted**, i.e.
**0.85 rejections per 1,000 shares** — the figure used in public copy is "below 2 per 1,000".
The reference for the software path, on the same phone and the same wired fibre connection,
was 0.0825% (47 in 56,999).

**Result: the reject rate did not improve and did not worsen.** 0.0846% observed against
0.0825% of reference — a 2.6% relative difference, which is nothing at this sample size. The
95% Poisson interval for the observed rate is **0.042% - 0.140%**, which contains the
reference value comfortably, while a halving of the rate (0.041%) sits just outside it and is
therefore excluded at 95%.

Two models were registered before the measurement (window-limited losses versus
volume-proportional losses). Both point predictions missed — the volume-proportional model
predicted 7-10 and 11 arrived, the window-limited model predicted 5 or fewer. The data support
the proportional mechanism but neither forecast was accurate enough to be quoted, which is
itself the lesson: with 11,000 shares the resolution is roughly +/-3 rejections, not +/-1.

Limits:

- The pool's API does not expose a per-worker rejection count, so this number comes from the
  app's own counter. There is no independent confirmation of it.
- One device, one connection, one evening. The decisive test for the *mechanism* is to repeat
  the run at half the workers: same hardware path, half the hash rate, same link. At 8,000
  shares that run would expect ~7 rejections if the rate is volume-independent and ~3 if it
  scales with volume.

For context, the pool's own aggregate counters reported **12.9% rejected** across ~9,470
workers, almost all of them low-power devices on domestic Wi-Fi. That number is not a
benchmark for a miner: it mostly measures connection quality, and it is cited here only to
show that "below 0.2%" is not a typical figure for this pool.

## Update path, verified end to end

The distributed package was tested the way a customer experiences it, not with a local build:

1. The **0.2.4 package previously published on the shop** was installed on a device.
2. The Bitcoin address was entered, so there was a setting worth losing.
3. The **0.3.2 APK from the new package** was installed over it.

Result: the install completed **without asking to uninstall**, and the address field **still
contained the address** afterwards. Both facts matter. The first proves the two artifacts share
the distribution certificate (RSA 3072, `43bb0e9f...be2962`); the second proves Android treated
it as a genuine update rather than an uninstall-and-reinstall, so customers keep their settings.

The sample counter was also checked on the device: two minutes of mining with the screen on
produced **5 samples**, against roughly 25 before the fix. The counter now advances once every
30 seconds, which is what the field claims.

## Measurement conditions for the 2026-11-17 hardware run

Every figure in the two sections above was produced under these conditions. Change any of them
and the numbers change with them — in particular the reject rate, which is dominated by
round-trip latency rather than by hashing.

| | |
|---|---|
| Device | UMIDIGI Power 3, Android 9, arm64-v8a, Cortex-A53 |
| Workers | 8 |
| Pool | `pool.nerdminers.org:3333`, assigned difficulty 0.001 |
| **Network** | **wired Ethernet, FTTH fibre** — not Wi-Fi, not mobile |
| Ambient temperature | 21 °C, closed room |
| Window | 2026-11-17, 20:21 - 22:00 CET |

**The wired fibre connection matters.** The stale-share window is roughly the time a share
spends in flight while the pool announces a new job, which on this pool happens every 30
seconds. A mobile connection with 80-150 ms of round-trip latency, or a congested Wi-Fi link,
puts far more shares inside that window than a 2-3 ms wired link does. A reader on 2G, 3G or
saturated Wi-Fi should expect a **materially higher reject rate than 0.1%** — that is a
property of the link, not of the miner, and it is the reason the pool's own aggregate figure is
12.9%.

The hash-rate figures are less sensitive: they come from share counters, not from timing, and
a slower or lossier link reduces throughput only through retransmissions and dropped
connection time. Anyone reproducing this should report their connection type alongside the
number.
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

## SHA-256 hardware acceleration (0.3.0 and later)

The compression rounds for `arm64-v8a` can run on the ARMv8 SHA-2 unit. Nothing about
this is taken on trust:

- **Runtime gate:** the path is compiled only for `arm64-v8a` and is only reachable when
  `getauxval(AT_HWCAP)` reports `HWCAP_SHA2`. `armeabi-v7a` and `x86_64` are built
  without the crypto flag, so they cannot execute those instructions at all.
- **Self-test:** on first use, 1,030 hardware hashes are compared against the scalar
  reference. One mismatch disables the path for the whole session and the app reports
  `BROKEN` instead of silently hashing wrong.
- **Visible state:** the app shows `SHA-256 hardware path: ACTIVE`, `N/A - this CPU has
  no SHA-2 unit` or `BROKEN - self-test failed` on the CPU card, without running anything.
- **Thermal sampling runs in the service, not the activity** (0.3.1): every 30 s while
  mining, so the recorded peak is the screen-off peak, which is the one that matters.
- **Reproducible off-device check:** the real source file can be cross-compiled and run
  under QEMU, which implements the ARMv8 crypto extensions:

  ```
  aarch64-linux-gnu-g++ -std=c++17 -O2 -march=armv8-a+crypto \
    -DNERDPHONE_HAVE_SHA2_HW -static -I. sha256_armv8.cpp verify.cpp -o verify
  qemu-aarch64-static ./verify
  # hw_available() = 1
  # hardwareVerified() = 1
  # equivalenze: 20480 ok, 0 bad -> PASS
  ```

  This is how the first broken implementation was found and disproved: an ABEF/CDGH
  register packing produced wrong digests that the on-device self-test caught but that
  no host test could have shown.

Field results:

| Device | Test | Result |
|---|---|---|
| Oukitel T606, 8 workers | self-test | `ACTIVE` |
| Oukitel T606, 8 workers | shares at difficulty 0.001 | **400 / 400 accepted**, 0 rejected |
| UMIDIGI Power 3, 8 workers | self-test | `ACTIVE` |
| UMIDIGI Power 3, 8 workers | pool share counters, before and after | **4.14 -> ~9.4 MH/s (2.3x)**, same worker |

The Power 3 pair is measured by the pool, not by the app, on one worker and one phone. Both
windows are share-count deltas at difficulty 0.001, where one share is 2^32 x 0.001 hashes:

- **Before:** shares 477,640 -> 487,420 over 10,140 s (2026-11-17 17:32:17 -> 20:21:17 CET),
  i.e. 9,780 shares, 0.9645 shares/s, **4.14 MH/s**. This reproduces the historical baseline
  exactly, which is what makes it usable as a "before".
- **After, window A:** shares 487,420 -> 488,924 over 663 s (20:21:17 -> 20:32:20 CET),
  i.e. 1,504 shares, **9.74 MH/s** (2.35x).
- **After, window B:** shares 489,481 -> 495,709 over 2,761 s (20:37:19 -> 21:23:20 CET,
  after an app restart), i.e. 6,228 shares, **9.69 MH/s** (2.34x).
- **After, window C (conservative):** shares 487,420 -> 495,709 over 3,723 s, including a
  three-minute gap for the app update, i.e. 8,289 shares, **9.56 MH/s** (2.31x).

Uncertainty: Poisson error is 2.6% on window A, 1.3% on B and 1.1% on C. At the old rate
window C would have produced 3,590 shares instead of 8,289, i.e. **78 standard deviations**.

**On `hashrate1hr`, and a retracted claim.** During the run it was briefly claimed that the
pool's `hashrate1hr` confirmed the share-delta measurement (a prediction of 6.11 against an
observed 6.06). That comparison was invalid: the 6.11 was obtained by choosing a 22-minute
hardware window *after* seeing the result. `hashrate1hr` is in fact not a boxcar average — at
21:23, one hour after the change, a boxcar would read ~9.1 MH/s while the pool read 6.73; the
behaviour fits an exponential average with a time constant near 90 minutes. The field is
therefore not usable as a point prediction, and no such claim appears in this document. What
it does support is the shape: a monotonic rise (3.77 -> 6.73 over eight readings) with no
plateau, which is the opposite of what thermal throttling would produce.

Confirmation on `hashrate1hr` and `hashrate1d` requires the miner to stay on the hardware path
for an hour and a day respectively; those fields still contain mostly software-path history.

The app can also produce a back-to-back pair with no waiting: the same three-second test run
with the hardware path on and off, on the same device.

Not yet measured: sustained hash rate and die temperature over hours with the hardware
path active, and the effect of the extra throughput on battery drain.
