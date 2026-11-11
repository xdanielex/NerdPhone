# README screenshots and photo plan

Use real captures from the released app and actual pool. Never edit numbers, create fake acceptance counters or imply a block was found. Redact identifying information before adding assets to Git; deleting a file later does not remove it from repository history.

## Recommended set, in priority order

### 1. Real phone photo — hero

Suggested filename: `docs/images/phone-mining.jpg`.

A landscape 1600 × 900 or 1920 × 1080 photo of the actual repurposed phone on a simple desk, displaying hash rate and accepted shares. Use soft side light, avoid screen reflections, keep the phone sharp and leave some space around it. A visible charging cable is fine; do not stage broken hardware or hide unsafe battery conditions.

Suggested caption:

> An old Android phone running NerdPhone with eight mining workers. Performance varies by device.

Do not put the displayed Bitcoin address, private notifications, router labels or identifying papers in the frame. A photo cannot establish that the phone is cold or energy-efficient.

### 2. Main dashboard screenshot — most important UI proof

Suggested filename: `docs/images/dashboard.png`.

Use a native portrait screenshot (roughly 1080 px wide if the device supports it). Show hash rate and nonzero ACCEPTED, ideally with submitted/rejected counters. Capture real telemetry after the nonce-format fix. Do not stretch the image or stitch different moments into one misleading screen.

Suggested caption:

> Live mining dashboard. Accepted shares indicate work accepted by the configured pool; they are not Bitcoin block confirmations.

### 3. CPU panel screenshot

Suggested filename: `docs/images/cpu-workers.png`.

Show detected available CPUs, selected mining workers and per-worker rates. If showing the offline benchmark, label it **offline three-second test**, not live pool mining. Make clear that worker IDs are not pinned physical-core IDs.

Suggested caption:

> Per-device CPU detection and configurable mining workers. Android controls scheduling.

### 4. Pool dashboard screenshot — independent corroboration

Suggested filename: `docs/images/pool-stats.png`.

Show assigned difficulty if available, active worker status and useful hash-rate windows/share statistics. Prefer a longer session and document the timestamp. Keep the pool name visible, but redact payout address, any QR code, browser address bar or user-specific page URL. Retain the time-window labels so a one-minute peak is not presented as a long-term average.

Suggested caption:

> Pool-side estimates from the same test session. Estimates depend on share arrivals and smoothing windows.

### Optional: short video

A 15–30 second real screen recording showing worker settings, mining and an increment of ACCEPTED. Avoid recording the address entry or notifications. Screen-off support needs a separate timed demonstration; a screenshot alone does not establish it.

## Integrating real images

Once the real files exist, place the hero below the introduction and a compact screenshot row after Features. Do not add broken placeholder links beforehand. For example, only after adding the files:

```html
<img src="docs/images/phone-mining.jpg" alt="An old Android phone running NerdPhone" width="900">

<p>
  <img src="docs/images/dashboard.png" alt="NerdPhone dashboard with accepted shares" width="280">
  <img src="docs/images/cpu-workers.png" alt="CPU detection and worker controls" width="280">
</p>
```

Optimize PNG/JPEG without obscuring text, remove location/EXIF metadata from photos, and preview on mobile. Keep the original evidence privately if needed; only commit the sanitized assets. Ask for permission if a screenshot/photo contains another person's device or content.
