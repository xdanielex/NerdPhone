# Roadmap

Priorities, not promises or scheduled releases.

## Before a public pre-release

- Record exact tester device/Android details and a longer session.
- Add real, redacted screenshots and a photo; no fabricated telemetry.
- Confirm the maintainer identity, repository URL and private security contact.
- Choose a signing/update policy and upload a matching APK/checksum.

## Reliability

- Durable block-candidate event record, priority delivery and explicit candidate/submission/response states.
- Test the complete Android block-candidate notification path using clearly labelled offline test data.
- More tests for stale/clean job boundaries, reconnects and background lifecycle transitions.
- Broader real-device validation, including Android 10 connection diagnostics.

## Performance

- Completed in 0.2.4-midstate-test: SHA-256 midstate reuse, randomized equivalence tests and simulated-pool validation; initial screen-off field trial successful.
- Future: optional ARM SHA backend, with runtime feature detection, known-vector and share-validity tests.
- Longer comparative benchmarks without presenting short peaks as sustained throughput.
- No performance promises before device measurements.

## Usability

- Sanitized diagnostics export and persisted benchmark summaries.
- Optional TLS Stratum support.
- Better accessibility and UI resource/localization structure.
