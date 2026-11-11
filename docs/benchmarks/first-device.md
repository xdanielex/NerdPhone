# First real-device field report — UMIDIGI Power 3

**Source:** reports and pool statistics supplied by the first tester during development on 2026-11-11. This is not an independently instrumented benchmark. Payout address and personal worker names are intentionally omitted.

## Configuration

| Field | Recorded value |
|---|---|
| Phone | **UMIDIGI Power 3**, identified by the tester |
| Android | **Android 10**, reported by the tester; exact build not recorded |
| APK | Post-nonce-fix sessions; exact installed version not confirmed for every later snapshot |
| App-available logical CPUs | Tester reports 8 |
| Selected mining workers | 8 |
| Pool | `pool.nerdminers.org:3333` |
| Assigned share difficulty | Tester reports `0.001` |
| Screen state | Both on and off; longer screen-off sessions reported |
| External power | Charger connected during part of the reported testing |
| Power at the wall / USB input | Not measured |
| Internal/battery temperature | Not measured |
| Ambient conditions / battery charge level | Not recorded |

The phone was repurposed because the tester no longer uses it as a working telephone. Its model is recorded; its exact age is not independently established. The results do not certify other phones or the exact distribution-signed 0.2.3 APK, which still needs device validation.

## Observations

- Early single-worker local measurements: approximately 290–300 kH/s.
- Eight-worker local reports: approximately 1.82 MH/s, later near 2 MH/s while connected to the pool.
- An early screen-off local estimate was approximately 1.4 MH/s; the first displayed sample after reopening the app is not a dedicated background-only measurement.
- Later pool snapshots during reported screen-off operation showed recent estimates around 1.7–1.9 MH/s. One-minute estimates sometimes exceeded 2 MH/s; those are not guaranteed sustained rates.
- The tester reported an approximately two-hour screen-off session and described the phone as cool to the touch, including while connected to a charger. This is **not an instrumented temperature measurement or a safety certification**.
- A low-rate tail of one session followed an intentional stop and a second-phone connection attempt. It must not be presented as an unexplained mining slowdown, nor as evidence of two hours with no interruptions.

## Initial pool snapshots

| Metric | Snapshot A | Snapshot B, 240 s later |
|---|---:|---:|
| One-minute estimate | 1.45 MH/s | 2.05 MH/s |
| Five-minute estimate | 469 kH/s | 1.24 MH/s |
| One-hour estimate | 44.8 kH/s | 161 kH/s |
| Active workers | 1 | 1 |
| Account `shares` counter | 3,685 | 3,792 |
| Active worker `shares` counter | 3,565 | 3,672 |
| Best share difficulty | 1.71271665244349 | unchanged |

Both share counters increased by 107 in 240 seconds. **If** each increment represents one accepted share at difficulty 0.001, that corresponds to approximately 1.91 MH/s of share-equivalent work. We have not independently established every detail of the pool's accounting semantics.

## Later session with a named worker

All rows below belong to the same UMIDIGI Power 3, not additional phones. Elapsed times are differences between supplied `lastshare` timestamps, not independently recorded wall-clock sampling times.

| Elapsed since first row | Worker `shares` counter | One-minute estimate | Five-minute estimate |
|---|---:|---:|---:|
| 0 s | 448 | 1.97 MH/s | 1.79 MH/s |
| 421 s | 612 | 1.91 MH/s | 1.72 MH/s |
| 2,879 s | 1,621 | 2.05 MH/s | 1.83 MH/s |
| 3,180 s | 1,744 | 1.68 MH/s | 1.77 MH/s |

Across the first and last rows, the counter increased by **1,296 over 3,180 seconds (53 minutes)**. Under the same single-share/difficulty-0.001 assumption, that implies approximately **1.75 MH/s** of average share-equivalent work. This is consistent with the recent pool windows, without treating a short peak as constant speed.

Pool estimates depend on stochastic share arrivals and smoothing. A five-minute estimate is not a precise instantaneous counter; an hour/day/week estimate can remain low while a new worker ramps up. Inactive historical worker entries do not establish multiple active phones.

## Rejected shares

The tester separately reported:

- About **4 rejected out of 5,000 submissions** in an approximately two-hour session: roughly **0.08% rejected / 99.92% accepted**; reasons were not supplied for that aggregate.
- **2 rejected out of about 1,700**, with a `Stale` response: roughly **0.12% rejected / 99.88% accepted**.

These are approximate user reports from separate counts, not independently exported event logs. Do not combine them into a single sample or claim a universal 3% rejection tolerance. Occasional stale submissions can coincide with job transitions/network delay; systematic format, target or duplicate errors require separate investigation.

## Throughput comparison

Using the tester's **57 kH/s per-device reference**, **1.7–1.8 MH/s** corresponds to approximately **30–32 such devices**. For example:

```text
1,710 kH/s / 57 kH/s = 30
```

This does not establish the speed of all NerdMiner variants. It is arithmetic, not a controlled comparison against thirty physical units. No conclusion about relative energy use, charger efficiency, hardware cost or lifetime is established. Many separate boards and displays might have a different combined consumption, but the phone and comparison devices were not measured.

## Multiple-device connection observation

The tester reported that a second phone on the same network could connect only after stopping the first, including an unsuccessful attempt with a different BTC address. One failure was a TCP timeout (`errno 110`), before Stratum authorization. A pool/network IP restriction is suspected but unconfirmed; no independent mobile-data comparison was completed. The one-phone results must not be presented as verified fleet support.

## What this supports

The tester observed accepted work on a real public pool after the nonce-format fix, and supplied pool data consistent with roughly 1.7–1.8 MH/s in the documented later interval. It does **not** establish universal 2 MH/s performance, profitability, energy efficiency, measured safe temperatures, broad Android compatibility or tested block-candidate confirmation.

## Reproducible follow-up

Record exact Android build and APK version, available CPUs, workers, screen/charging state, ambient conditions and run duration. Run comparable 1/2/4/all-worker sessions for at least 30–60 minutes, recording local/pool windows and acceptance counts. Measure USB input and/or wall power with the appropriate meter; account for battery charging/discharging when interpreting energy. If recording OS temperature, name the sensor and units. Do not infer internal temperatures from touch alone.

Report failures as well as successful results. Redact payout addresses, IPs, SSIDs, private notifications and identifying background details from screenshots.

## Optimized midstate build: first successful screen-off trial

The tester subsequently installed the signed release build `0.2.4-midstate-test` (versionCode 10), with eight workers on the same UMIDIGI Power 3. The tester explicitly confirmed screen-off operation repeatedly and reported the phone remained cold to the touch. No internal temperature or power measurement was made.

| Elapsed from first supplied optimized snapshot | Worker shares | One-minute pool estimate | Five-minute pool estimate |
|---|---:|---:|---:|
| 0 s | 186 | 3.61 MH/s | 1.86 MH/s |
| 121 s | 291 | 3.67 MH/s | 2.38 MH/s |
| 182 s | 348 | 3.86 MH/s | 2.65 MH/s |
| 360 s | 497 | 3.52 MH/s | 2.99 MH/s |
| 419 s | 553 | 3.99 MH/s | 3.19 MH/s |
| 541 s | 664 | 3.86 MH/s | 3.36 MH/s |
| 721 s | 827 | 4.09 MH/s | 3.55 MH/s |
| 840 s | 942 | 4.16 MH/s | 3.69 MH/s |
| 1,077 s | 1,171 | 4.67 MH/s | 3.85 MH/s |

Elapsed times are differences of `lastshare` timestamps. These snapshots cover 17 minutes 57 seconds, not the entire session since installation. The counter increased by 985; using difficulty 0.001 and the same accounting assumption as above gives approximately **3.93 MH/s** of average share-equivalent work. Relative to approximately 1.78 MH/s in the earlier session this is around **2.2×**. Conditions were not instrumented/controlled as a formal A/B benchmark.

The tester reported every share accepted at successive checks, most recently explicitly at counter 942. The later counter 1,171 is not itself a separate rejection-counter export; no later rejection was reported. These are user observations, not independently retained pool event logs.

The tester also observed approximately 4.1 MH/s in the app, but the exact display context was not established. Do not substitute it for a measured whole-session rate. The peak supplied one-minute pool estimate of 4.67 MH/s is a short stochastic estimate, not a constant rate.

The maintainer approved retaining this build as the preferred reference after the initial successful trial. No binary/version rename was made. Longer tests, additional devices, installation/upgrade checks and candidate-alert validation remain useful; earlier two-hour results belong to the unoptimized engine.

At a reference 57 kH/s per ESP32 miner, the optimized interval corresponds to approximately 69 devices and the brief one-minute estimate to approximately 82. Against a seller-declared 307 kH/s reference, the interval corresponds to approximately 13. No comparative power-efficiency claim is established.

### Latest optimized snapshot

At 1,319 seconds after the first supplied optimized snapshot, the worker counter reached **1,400**, with pool windows **3.97 MH/s (1 minute)** and **3.86 MH/s (5 minutes)**. The increase of **1,214** across **21 minutes 59 seconds** implies approximately **3.95 MH/s** under the difficulty/accounting assumption above. The previous 17-minute-57-second observation remains historical, not the latest endpoint. No additional rejection-counter export was supplied with this latest snapshot. Earlier screen-off and cold-to-touch observations remain tester reports.
