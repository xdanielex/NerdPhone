# NerdPhone 0.3.2 — installation

Thank you for downloading NerdPhone. This package contains an **already compiled and signed
APK**, ready to install: no Android Studio, no SDK, no build steps.

## Requirements

- An **Android 8.0 (API 26) or newer** phone or tablet.
- An internet connection (Wi-Fi or mobile data).
- A **public Bitcoin address** to receive a block if one is ever found. Any address from your own
  wallet is fine: **no private key is needed, and you must never enter one in the app.**

## Install

1. Copy **`NerdPhone-0.3.2.apk`** to the phone (USB cable, cloud drive or SD card).
2. Open the file from the *Files* or *Downloads* app.
3. Android will ask to allow installs from this source: tap **Allow** / *Install unknown apps*
   for the app you are using (Files, Chrome, etc.). This is the normal procedure for an APK
   outside the Play Store.
4. The **NerdPhone** icon appears in your launcher.

**Updating from an earlier version:** just install it over the old one. Every NerdPhone release
is signed with the same distribution key, so Android treats this as an update and your settings
are kept. You only need to uninstall if you ever installed a **debug** build, which is signed
with a different key.

## First run

1. Grant the **notification** permission (needed for the persistent mining-service notification).
2. In **BTC address / worker**, paste your public Bitcoin address, optionally followed by a
   worker name, for example `bc1....my-phone`.
3. Keep the default pool `pool.nerdminers.org`, port `3333`, password `x`, requested difficulty
   `0.001`.
4. In **CPU DETECTION & MINING THREADS** choose how many workers to run. Start with 2 and work
   up; **Use all** takes every core the app can see.
5. Tap **START MINING**. The number that matters is **ACCEPTED**: it means the pool accepted the
   work you sent.

## CPU panel: what the three states mean

| Shown | Meaning |
|---|---|
| `ACTIVE (self-test passed)` | this CPU has an ARMv8 SHA-256 unit and it produces correct hashes. This is the fast path: on a mid-range phone it roughly doubles the hash rate. |
| `N/A - this CPU has no SHA-2 unit` | no hardware unit; the app mines on the software path automatically. Nothing is broken. |
| `BROKEN - self-test failed` | the CPU advertises a SHA-2 unit but returns wrong hashes. Mining continues on the software path. Please report the exact CPU model. |

## THERMAL panel

| Field | Meaning |
|---|---|
| `Battery` | current battery temperature, as reported by Android |
| `SoC thermal zone` | chip temperature, when the system exposes it; on Android 10 and later the filesystem is usually restricted and the row shows `N/A` |
| `Peak since mining started` | the highest temperature seen since you pressed START, with the number of samples behind it (one every 30 seconds) |
| `Android thermal status` | the system's own verdict, from `None` to `Shutdown imminent` |

## Screen-off use

To keep mining with the screen off:
- use the **Android battery settings** button in the app and allow unrestricted background execution;
- on many manufacturers you also need to disable battery optimisation in the system settings;
- test on **your** device before leaving it unattended.

No app can guarantee this: Android and OEM power management can still suspend the service.

## What to expect

- **This is not an investment.** It is lottery mining: the chance of finding a block is
  vanishingly small and there is no expected income.
- On a **UMIDIGI Power 3 with eight workers**, version 0.3.2 sustained about **9.6 MH/s**
  measured at the pool for over an hour, against **4.1 MH/s** on the software path on the same
  phone, in the same room, on the same connection: roughly **2.3 times**. Other phones give
  different results, and a single-core cheap phone will be far slower.
- The battery stayed at **22.7 °C** with a peak of **25.9 °C** in a 21 °C room. The phone may get
  warm and the battery drains: that is normal. Stop mining if the device becomes too hot or if
  the battery swells.
- If the pool assigns a difficulty other than 0.001, the app uses the pool's value.

## Verify the file before you install it

The SHA-256 of the APK in this package is:

```
05caa450d10c300395b3790ea427ca21fef11bcedc6d74d84ac7cec8cd7f5d5d
```

`SHA256SUMS.txt` contains the same value. If the two disagree with your file, **do not install
it**: the download was corrupted or tampered with. Neither is likely, and both are worth thirty
seconds.

The APK is signed with the project distribution key, RSA 3072 bits, certificate SHA-256
`43bb0e9fd881b7bbe8f0739a03774e51319a94992e72efbc3e6dbaec69be2962`.

## Troubleshooting

| Symptom | What it usually means |
|---|---|
| "App not installed" | a debug build or a build from another source is already present: uninstall it. Otherwise the file is truncated: check the SHA-256. |
| Hash rate near zero, `N/A - this CPU has no SHA-2 unit` | no hardware SHA-256 on this CPU: the software path is in use, which is normal and slower. |
| Accepting more shares than before, same phone | a share is not a payment; only the pool's accepted count matters, and it grows with hash rate, not with luck. |
| A high reject rate at the pool | almost always the connection, not the app. Mobile data or congested Wi-Fi adds latency, and shares go stale when the pool changes job. A wired link on fibre measured 0.085%; a slow mobile link can be far worse. |
| Mining stops when the screen goes off | a battery-optimisation setting is stopping the service; exclude NerdPhone from it. |

## Licence and rights

MIT. The source code is public and you are free to build, modify and redistribute it under the
terms in `LICENSE`. Buying this package is optional support for the project, not a licensing
condition, and it restricts none of your rights under that licence.

## Privacy

The app has no account and no telemetry. It talks to the mining pool you configure, and to
nothing else. Your Bitcoin address is visible to that pool — that is how solo mining works.
