# Screenshot and photo guide

Use only real captures from the released build and the real pool. Never edit numbers, invent acceptance counters or imply that a block was found. Redact identifying information **before** committing: removing a file later does not remove it from git history.

## For 0.3.2: the five existing files must be replaced, not reused

The captures currently in `docs/images/` were taken on 0.2.4: they show 4 MH/s, no SHA-2 status
line and no THERMAL card. Publishing a README that claims 9.6 MH/s next to screenshots showing
4 MH/s is a contradiction a reader will spot in five seconds. Recapture all five with the same
file names so the README keeps working, and add two new ones for the panels that did not exist
before.

| File | Action |
|---|---|
| `01-dashboard.png` … `04-pool-config.png`, `pool-stats.png` | **replace** (same names) |
| `05-thermal.png` | **new**: THERMAL card, with `Peak since mining started` and its sample count |

`02-cpu-threads.png` must now show the **`SHA-256 hardware path: ACTIVE (self-test passed)`**
line as well as `Selected miners: 8 + 1 dedicated pool thread`: scroll so both fit in one
capture. That line is the single most important thing this release adds, and it deserves a real
screenshot rather than a separate file that repeats the same panel.

Capture the new set after **at least 15 minutes of mining with 8 workers**, so the peak and the
hash rate in the images are steady-state and match the numbers in the README.

## Required set

All app captures: portrait, original resolution, mining running for at least 10–15 minutes, no
notifications in the status bar.

| File | Content |
|---|---|
| `01-dashboard.png` | Top of the screen: NERDPHONE header, **HASH RATE** card in kH/s, status line |
| `02-cpu-threads.png` | **CPU DETECTION & MINING THREADS** while mining: *Selected miners: 8 + 1 dedicated pool thread*, *Active miners: 8*, per-worker kH/s |
| `02b-offline-test.png` *(optional)* | Result of **Run 3-second offline test** taken while stopped. Label it **offline**, not live pool mining |
| `03-shares-jobs.png` | **SUBMITTED / ACCEPTED / REJECTED / SHARE DIFF** row plus the **STRATUM JOBS / DIAGNOSTICS** block |
| `04-pool-config.png` | **STRATUM V1 CONNECTION** with host and port visible, START/STOP button and the safety note. **Cover the BTC address** |
| `pool-stats.png` | ✅ present — pool dashboard from the same session, browser chrome removed |

Optional extras: a real desk photo of the phone (`phone-mining.jpg`) and a short screen recording showing ACCEPTED increment.

## Rules

- **Cover the BTC address, the worker name and any pool URL containing the wallet identifier
  before the first commit.** Deleting the file afterwards does not remove it from git history,
  and a branch on a public repository is readable at its URL by anyone before it is merged.
- Do not stretch, resize or stitch different moments into one image.
- Keep time-window labels so a one-minute peak is never presented as a long-term average.
- Keep the pool name visible in pool screenshots; remove any URL containing a wallet identifier.
- A photo cannot prove the phone is cold or energy-efficient; do not caption it that way.
- Worker IDs are scheduling identifiers, not physical core IDs.
