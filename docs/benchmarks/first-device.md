# Real-device field report — UMIDIGI Power 3

**Source:** reports, screenshots and pool statistics supplied by the maintainer between 2026-11-11 and 2026-11-12, using the signed release build `0.2.4-midstate-test`. This is not an independently instrumented benchmark. The payout address is redacted and no personal worker name is published.

## Configuration

| Field | Recorded value |
|---|---|
| Phone | **UMIDIGI Power 3**, repurposed; no longer used as a daily phone |
| Android | **Android 10** |
| APK | `0.2.4-midstate-test`, versionCode 10, distribution-signed |
| Logical CPUs available to the app | 8 (`[0,1,2,3,4,5,6,7]` reported by the app) |
| Selected mining workers | **8**, plus one dedicated pool thread |
| Pool | `pool.nerdminers.org:3333` (solo pool) |
| Assigned share difficulty | **0.001** |
| Screen state | **Off** for the extended run and the battery run |
| Cooling | None. No fan or heatsink was used |
| External power | Charger disconnected during the battery run |
| Internal/battery temperature | **Not measured** |
| Power at the wall / USB input | **Not measured** |
| Ambient conditions | Not recorded |

## Extended run

The pool graph supplied for the session shows one worker. Each major grid interval is **three hours**. After an initial gap — caused by a deliberate stop to install the update, not by a miner failure — the pool estimate stayed near 4 MH/s for roughly twelve hours.

| Metric | Observation |
|---|---|
| Duration near the observed plateau | About **12 hours** |
| Pool `hashrate1hr` in supplied snapshots | **3.9 – 4.0 MH/s** |
| Highest supplied one-minute estimate | **4.67 MH/s**, a short stochastic window, not a sustained rate |
| Local app reading | **4,010 kH/s** at the moment of the screenshot |
| Per-worker spread | **462 – 528 kH/s** across the eight workers |

## Share accounting

Worker-share counter snapshots from the same session. Elapsed times are differences of `lastshare` timestamps, not wall-clock sampling. The share-equivalent rate assumes one difficulty-0.001 increment per counter unit, which multiplies to about 4.295 MH/s of hashing per unit per second.

| Elapsed (s) | Worker `shares` | `hashrate1m` | `hashrate5m` |
|---:|---:|---:|---:|
| 0 | 186 | 3.61 MH/s | 1.86 MH/s |
| 1,077 | 1,171 | 4.67 MH/s | 3.85 MH/s |
| 1,319 | 1,400 | 3.97 MH/s | 3.86 MH/s |
| 1,741 | 1,804 | 4.09 MH/s | 3.90 MH/s |
| 2,280 | 2,326 | 4.51 MH/s | 3.95 MH/s |
| 5,582 | 5,420 | 4.29 MH/s | 3.87 MH/s |
| 6,363 | 6,112 | 4.04 MH/s | 3.71 MH/s |
| 52,995 | 50,489 | 4.30 MH/s | 4.12 MH/s |
| 54,495 | 51,938 | 3.92 MH/s | 3.91 MH/s |

- First to last row of the first block: **+5,926 increments in 6,363 s**, about **4.00 MH/s** share-equivalent.
- First snapshot to the 50,489 snapshot: **+50,303 in 52,995 s**, about **4.08 MH/s** share-equivalent.

The interval rate and the pool windows agree within a few percent, which is the expected behaviour for a steady worker; they are still estimates, not instruments.

## App-side counters

| Counter | Value |
|---|---:|
| Submitted | **56,999** |
| Accepted | **56,952** |
| Rejected | **47** |
| Share difficulty | **0.001** |
| Best share difficulty | **74.5631** |
| Rejections | All reported as `Stale`; **≈0.08%** of submissions |

Occasional stale submissions are expected around job transitions and network delay. No target, format or duplicate errors were observed. The counters are those displayed by the app; the pool's own event log was not exported.

**Best share is a luck record, not progress.** 74.5631 is roughly 74,563 times the assigned difficulty of 0.001 and still several orders of magnitude below the Bitcoin network difficulty.

## Battery run

Charger disconnected at **10:13** with the battery at **90%**, eight workers, screen off, pool still showing about 4 MH/s.

| Time | Battery | Elapsed |
|---|---|---:|
| 10:13 | 90% | start |
| 10:20 | 89% | 7 min |
| 10:32 | 87% | 19 min |
| ~10:46 | 85% | 33 min |
| 11:15 | 80% | 62 min |

**≈10% of battery per hour**, i.e. roughly **9 h 20 min projected from 90%**.

An order-of-magnitude power estimate, assuming the nominal **5,000 mAh at 3.8 V (≈19 Wh)**: 10% of 19 Wh is 1.9 Wh over 1.03 h, so **≈1.8 W average for the whole phone**.

This figure has three limits and they matter:

1. The cell is **used**, so its real capacity is probably below 5,000 mAh and the number is **more likely an overestimate than an underestimate**.
2. The battery percentage gauge is coarse.
3. It is **total phone consumption**, not the miner alone, and not a wall-plug measurement.

The measured facts are the percentage drop over 62 minutes and the reported absence of noticeable heating.

The phone was recharged after this window; later screenshots showing 67% belong to a different, post-charge session and are not part of these figures.

## Heat

The maintainer reports the phone remained **cold to the touch** throughout, with no cooling accessory. Touch is not a thermometer: no internal, surface or battery temperature was logged, and no comparison against a fan-cooled device was measured.

## Hash-rate comparison, arithmetic only

Using the tester's **57 kH/s** reference for a small ESP32 miner:

```text
4,000 kH/s ÷ 57 kH/s ≈ 70
```

Against a seller-declared **307 kH/s** device the same throughput would be roughly **13 devices**, assuming that advertised rate is achieved.

This is arithmetic, not a controlled comparison against 70 physical units. It establishes nothing about energy use, purchase cost, reliability or lifetime. Devices sold with similar names that use ASICs are a different performance class.

## What this supports and what it does not

**Supports:** on this phone, with this build, the app sustained roughly 4 MH/s of pool-side work with eight workers and the screen off for about twelve hours; share acceptance was about 99.92%; the phone used roughly 10% of battery per hour in the measured window.

**Does not support:** universal 4 MH/s on other Android devices, profitability of any kind, verified energy efficiency in watts, measured safe temperatures, long-term stability beyond the observed sessions, or any claim about finding a block. One phone, one operator, one pool.

## Reproducible follow-up

Record the exact Android build, APK version and signature, available CPUs, worker count, screen and charging state, ambient conditions and duration. Run comparable 1 / 2 / 4 / all-worker sessions of at least 30–60 minutes, recording local counters, pool windows and acceptance counts. Measure USB input or wall power with a suitable meter and account for charging while interpreting energy. If recording temperature, name the sensor and the units.

Report failures as well as successes. Redact payout addresses, IPs, SSIDs, notifications and identifying background details from screenshots.
